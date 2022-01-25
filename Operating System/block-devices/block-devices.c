/*
A well designed I/O buffering scheme can significantly improve I/O 
efficiency and increase system throughput
*/

BUFFER *bread(dev, blk) //return a bufffer containing valid data
{
    BUFFER *bp = getblk(dev,blk);   //get a buffer for dev, blk
    if(bp data valid)
        return bp;
    bp->opcode =  READ;             //issue READ operation
    start_io(bp);                   //start I/O on device
    wait for I/O completion;
    return bp;
}

write_block(dev, blk, data) // wriet data from U space 
{
    BUFFER *bp = bread(dev, blk);       //read in the disk block first
    write data to bp;
    (synchronous write)?bwrite(bp) : dwrite(bp);
}

//bwrite is for synchronous write and dwrite is for delay-write 

bwrite(BUFFER *bp)
{
    bp->opcode = WRITE;
    start_io(bp);
    wait for I/O completion;
    brelse(bp); //release bp
}

dwrite(BUFFER *bp)
{
    mark bp dirty for delay write;
    brelse(bp);                     //release bp 
}

/*
Since dirty buffers contain valid data , they can be used to satisfy subsequent read/write requests if the same block

This not only reduces the nuber of physical disk I/O but also improves the buffer cache effect

A dirty buffer will be written to disk only when it is to be reassigned to a different disk block
*/

awrite(BUFFER *bp)
{
    bp->opcode =  ASYNC;        //for SYNC write
    start_io(bp);
}

//      BUFFER MANAGEMENT ALGORITHM

//buf structure

typedef struct buf{
    struct buf *next_free;      //freelist pointer
    struct buf *next_dev;       //dev_list pointer
    u16 dev, blk;               // assigned disk block
    u16 opcode;
    u16 dirty;
    u16 async; 
    u16 valid;
    u16 busy;
    u16 wanted;                 //some process needs this buffer
    struct semaphore lock=1;    //buffer locking sempahore; value =1;
    struct sempahore iodone =0; // for process to wait for I/O completion
    char buf[BLKSIZE];          //block data area

}BUFFER;

//      DEVICE TABLE - each device is represented by a device table structure

struct devtab{
    u16 dev;            //major device number
    u32 start sector;   // for hard disk partitions
    u32 size;           // size of device in blocks
    BUFFER *dev_list;   // device buffer lisy 
    BUFFER *io_queue;   // device I/O queue
}devtab[NDEV];


/*
BUFFER *getblk(dev, blk)
{
    while(1){
        1.search dev_list fr a bp=(dev,blk)
        2.if(bp in dev list)
        {
            set bp WANTED flag;
            sleep(bp);
            continue;
        }
        //not busy
        take bp out of freelist;
        mark bp BUSY;
        return bp;
        }

        3.bp not in cache; try to get a free buf from freelist

        if(freelist empty)
        {
            set freelist WANTED flag;
            sleep(freelist);
            continue;
        }

        4. // freelist not empty
        bp = first bp taken out of freelist
        mark bp BUSY;
        if(bp DIRTY)
        {
            awrite(bp);
            conntinue;              
        }
    5. reassign bp to (dev, blk);
    return bp;

    //brlese: releases a buffer as FREE to freelist 
    brelse(BUFFER *bp)
    {
        if(bp WANTED)
            wakeup(bp);
        if(freelist WANTED)
        {wakeup(freelist);   }

        clear bp and freelist WANTED flags;
        insert bp to (tail of)n freelist

    }
    
}

1.Data Consistency : getblk must never assign more than one buffer to the same (dev,blk)
                    This is achieved by having the process re-execute the "retry loops" after waking up from sleep.
                    The reader may verify that every assigned buffer is unique.
                    dirty buffers are written out befire they are re-assigned, which guarantess data considtency

2.Cache effect  :  A released buffer remains in the device list for possible reuse.
                    Buffers are released to the tail of freelist but allocated from the front of the freelist.
                    this is based in the LRU (Least-Recent-Used) principle, which helps prolong the lifetime of assigned buffers, thereby increasing their cache effect

3.Critical Regions : Device Interrupt handlers may manipulate the buffer lists,
                    remove a bp from a devtab's I/O queue, change its status and call brelse(bp).
                    do in getblk and brelse, device interrupts are masked out in these critical regions


        SEMPAHORES

when processes wait for a resource, the V operation unblocks only one waiting process, which is guaranteed to have the resource.



*/

BUFFER buf[NBUF];       //NBUF I/O buffers
SEMAPHORE free  = NBUF;      // counting semaphore for FREE buffers 
SEMAPHORE buf[i].sem  = 1;  //each buffer has a lock sem =1;



/*          SIMPLE P/V ALGORITHM */
BUFFER *getblk(dev, blk)
{
    while(1)
    {
        P(free);                        //get a free buffer first
        if(bp in dev list)
        if(bp not BUSY)
        {
            remove bp from freelist;
            P(bp);                  //lock up but does not wait
            return bp;
        }
        // bp in cache but BUSY
        V(free);                    // give up the free buffer
        P(bp);                      //wait in bp queue
        return bp;
    

    // np not in cache, try to create a bp=(dev, blk)
    bp = first buffer taken out of freelist;
    P(bp);                  //lock b, no wait
    if(bp dirty){
        awrite(bp);         //write bp out ASYNC, no wait 
        continue;       //continue from 1
    }
    reassgin bp to (dev, blk);  //mark bp data invalid, not dirty
    return bp;
}

}

brelse (BUFFER *bp)
{
    8. if(bp queue has waiter){ V(bp); return;}
    9. if(bp dirty && free queue has waiter){awrite(bp); return;}
    10. enter bp into (tail of) freelist; V(bp); V(free);
}
/*
wehna a dirty buffer is to be released as freee and there are waiters 
the buffer is not released but written out directly.

This avoids unnecessary wakeup. This is because the process my have gone to sleep for different unresolved issues 
That may still be unresolved thus waking up the process is not necessary(deadlock)
*/