/*
when a process terminates or changes image size it calls kmfree(beginAddress,size)
which releases the area back to the free memory list.

In the free memory list every hole is of maximal size, so there are no adjacent holes.
When releasing a piece of memory three possible casess may arise
        1.we create a new hole to represent the released area
        2.We absorb the released area into an adjacent hole
        3.we consolidate the three adjacent hokes in a single hole, which actually deletes a hles firm the free memory list

kmfree(address, size) //chunck=[address,size] = released area
{
    if(chunk has no adjacent holes)
        insert chunk as a new node into free memory list
    else if( chunk is adjacent to ahole on both left and right)
        combine[left_hole,chunk,right_hole] into a single node
    else if(chunk has a left adjacent hole)
        absorb chunk into right hole;

}

the free memory list can be implemented into 2 different ways.

    1.maintain a liknk list inside the holes. each hole contains a triple=(address,size, next) at the beginnign, in which next is the beginning address of the next table
    2.Implement it as a link list containing memory node structures if the form


    tpedef struct mnode{
        struct mnode *next;
        u16 addr;
        u16 size
    }MNODE;
    MNODE mnode[NPROC+1];



*/
/*
To support memory mamagement the MTX kernel is modified as follows

PROC.res : pointer to a per-process "resource structure"
tyoedef struct res{
    u16 segment, size , SEP;    //segment, size , image type
    u16 tsize , dsize , bsize; //image's size in bytes
    u16 brk, splimit;       //brk mark and stack low limit
    OFT *fd[NFD];           //opened file descriptors
    //signal and handlers
}RES;

*/
/*
Memory compaction is a a scheme that which consolidates all the holes into a single hole of maximal size.
In this scheme whenever the memory manager can not find enpugh memory and there are multiple holes it moves all the process images to one enf of the memory
and adjusts their saved segemnt registers to suit the new segment

Then it tries to allocate memory again
*/


//       SIMPLE ALGORITHM FOR THE SWAPPER PROCESS
/*
while(1){
    sleep(need_to_swap_in);
    while(there are processes need to swap in){
        swap in a process image; continue
        if(no_memory){
            swap out a non_runnable process; continue
            swap out a ready process; continue;
            break;      //if cant swap out any , to wile(1) loop
        }
    }
}

A process may voluntarily swap itself out voluntarily , 
when a process tries to expand its image size but there is not enough
 memory it may write its own image out, including the expanded size
to swap disk, and wakes up the swapper process.

When a swapped out process becomes ready the swapper process is woken up to run which tries to swap in as many ready process images as possible

The choice of which process it may swap out usually involves some time information.

*/


/*
whenevr there is not enough space in the heap area malloc() issues a brk() or sbrk() syscall to increase the heap size
Since brk() and sbrk() change the Umode Image Size and relocate, if necessary the memory manager in kernel to adjust the Umode image size and relocate, if necessary, the Umode image

For clarity the MTX kernel implements two separate syscalls sbrk() which increases the heap size and rbtk() which reduces the heap size both in 16-byte clicks

When a process calls sbrk() to increase the heap size, it allocates a larger memory area for the new  image, copies the old image to the new memory area and releases the old image

During Image expansion, the maximal size if the combined data and stack is limited to full segment of 64KB.

when a process calls rbrk() it reduces the heap size, it shrinks the image soze and releases the extra memory space

//          ALGORITHM OF SBRK IN MTX

int srbk(u16 v) //request v is in 16-byte clicks
{
    if(v==0) return current brk mark;
    newsize =  PROC.size + v;
    if(image has separate I&D space)
        newsize = newsize - tsize
    if(newsize > 4096) //max. 64KB limit
        newsize = 4096;
        v = newsize - size;
    new_segemnt = kmalloc(newsize);     //allocate newsize
    copy | code | data | bss| heap to new_segment;
    copy old stack to the high end of the new_segment;

    mfree(PORC.segment, PROC.size);

    change PROC.CS to new_segment
    change PROC.USS to new_segment if not separate I&D space
    change PROC.USS to new_segment + PROC.tsize if separate I&D space
    adjust saved stack frame pointer in new image;

    set image's new segment, size and brk mark; return brk;
}
Moving the stack area alone would cause
references to loacl variables to be 
incorrect due to the changed stack position in the 
new image.
Although the stack has been moved all the saved stack frame pointers still point to locations in the old stack.

we must adjust the saved stack frame pointers to suit the new image.

The stack frame pointers form a linked list whihc ends in 0. It is fairly easy to traverse the link list to adjust all the saved stack frame pointers by a linear shift
Pointers which already point at local variables must alos be adjusted.
Since it is impossible to know whether there are such pointers the only solution are
    1.do not use pointers to point to local variabels 
    2. after each sbrk() call,such variables should be reassigned

    
        REDUCE HEAP SIZE BY RBRK()
    int rbrk(u16 v) //request v in 16-byte clicks
    {
        newsize = PROC.sixe -  v;
        if(newsize < 1024){//assume: minimal size=16KB=1024 clicks}
            newsize = 1024;
            v = Porc.size - newsize;
    }
    copy old stck forward by v;
    mfree(PORC.segment + newsize, v);
    adjust PROC.usp;
    adjust saved stack frame pointer in new image;
    set image's new size and brk mark; return brk;

*/