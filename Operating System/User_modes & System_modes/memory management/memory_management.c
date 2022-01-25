/*
Variable Sized Partitions 
        - in this scheme, each process image is 
        loaded to a contiguous piece of memory, which is allocated by image size
        When a process terminates, its memory is released as free for reuse

OVERLAY
    allows a program to use the same memory area at different stages of execution.
    In this scheme, a program is organized to consist of several disjoint pieces on demand, 
    overlying the program memory areas that are not needed by the current execution
    this is the basis of dynamic overlaying

SWAPPING
 Swaps process images between memory and a storage area, which can either be a swap disk or a swap file.
 canditates for swap-out are usually non-runnable processes i.e processes that are sleeping or blocked.

SEGMENTATION
    a program image is divided into many independent parts.
    During execution, the CPU is told which segment to use and where the segment is in memory.
    Each VA is an offset relative to the beginning of a segment.
    the segement's physical address is kept in either a segment register or a segment table

    To remedy lack of memory protection problem some COUs designed for segmentation allow aprogram to have a large number
    .e.g 256 segments and define by a pair of segment descriptors

    Segment Address Descriptor  = Segment begin address
    Segment Usage Descriptor = Segment Size, Acess R/W

    corresponding to these a loaded program image has a segemnt table containing up to 256 pairs if segment Descriptors
    Assuming 4 bytes per entry the segment Table has a size of 2KB which may be too large to be kept in the CPU all at once.

    So the CPU uses a Segment Base Register to point to the segment Table of the running process

    Assume that the segments are numbered 0 -> 25.
    Each VA is of the form of VA=[Segment#,Offset]

    CPU uses the corresponding segment table entry to validate the intended memory access.
    If VA exceeds the segment size it would be an invalid memory access 

    Similarly, the CPU can check the access field for READ_ONLY or READ/WRITE of the 
    Segment Usage Descriptor to ensure that the intended access is valid.

    Segments are best suited to logical units of a program image, such as Code, Data, Stack ,Procedures, common data etc.

    An excellent usage of segmentation is to allow processes share segments, such as dynamic Linking modules.


        PAGING

        One-Level Paging

        segments are of variables sizes and must be loaded to a contiguous memory area
        The disadvantage of segmentation is that it may cause memory fragmentation,
         in which the memory contains many unused spaces or holes but none big
          enough for the current needs

        Paging is designed to reduce memory fragmentation. 
        Instead of variabe sized segments, all pages are the same size .g. 4KB each.

        Physical memory is divided into 4KB page frames. then loading pages into memory becomes trivial:

        1.find enough page frames to load the pages. Allocation?deallocation of page frames can be done by either a bit-map or a link list.
        2. It does not matter which page is loaded to which page frame, as long as we record in apage Frame

    Multi-Level Paging
        The one-level paging scheme is impractical because, with 256 4KB-pages, each process image is limited to only 1024 KB.
        with 32-bit memory addressign, the maximal VA space of a
         process needs 1M pages or 4MB memory just for its Page Table alone.

         The solution is to use multiple levels of paging.

         In a 2-level paging scheme ,each VA is divided into 3 parts: VA =[page#1, Pag#2, Offset]
         given a VA  the CPU    first uses Page#1 into a 
         level-1 page table to find the physical address of the level-2 page table.
        
        To speed up address translation, the CPU typically keeps the most recently used page
        table entries in a translation-lookaside buffer(TLB)
        which is a small cache memory within the CPU.

Virtual Memory is based on the following:
    Multiplexing --> try to satisfy the needs for many items with only a few at different time
    Program Locality --> During exectuion a program tends to access instructions and data in a small 
                        area in memory, but nit everything within its VA space all at once.
    
    Trade time with space --> if we try to run a large process image with less rela memory, 
                            the process must stop from time to time, waiting for the needed pages to be loaded into memory


DEMAND- PAGING
        the OS loads only a few pages of a program image for execution to start. It loads additional pages only when they are needed by the program execution
        this allows a system to run programs with VA much larger than the actual amount of physical memory.

PAGE-REPLACEMENT RULES

In demand paging when a process attempts to reference a page that is not present in memory it generates a page fault, which traps to the OS kernel.
the kernel's page fault handler can load in the needed page to a page frame, update the page table entry and let the process continue.
Loading in a new page is straightforward, except when all the page frames are in use.
if that case, a loaded page must be evicted to make room for the page.
this is known as the Page Replacement Problem.

1.Optimal Replacement Rule : Replace those pages which will cause the least number of page faults in the future. this is unrealizable because it depends on the future bahviuor of processes.

2.FIFO Algorithm: This is the simplest page replacement algorithm to implement but it has been shown to have poor performance.

3.Second Chance Algorithm : Same as FIFO but give each referenced page a second chance.
                If th oldest page's Reference bit R is 0, choose it to replace.
                Otherwise, clear its R bit to 0 and continue to look for page with  0 R bit. if all pages' R bits are 1 this reduces to FIFO but with one round of searching overhead
            
4. LRU algorithm : Replace the Least Recently Used page.LRU is realizable but expensive to implement.
5.Use R and M bits: Each page has a Referenced bit and a Modified bit. Page replacement is based in the (R,M) bits.


6.Use a fixed size pool of free page frames.  
    The OS kernel maintains a certain number of free page frames in a free pool. During page replacement, a victim is selected as before.
    Instead of waiting for a free a page to become available,
    the needed page is loaded into a frame allocated from the free pool. This allows
    the page faulted process to continue as soon as possible. When the victim's page frame is freed, it is added to the free pool for future allocation.

7. use a dynamic free page frame pool: The free page frame pool is maintained dynamically.
            when a page is released it goes to the free pool. whenever the free pool reaches a low-watermark, a special process called PAGE STEALER, is scheduled to run, which collects the pages of idle(sleeping or blocked)
            processes into the free pool. page frames in the free pool are maintained in a ordered lsit to honor LRU (Least Recently Used)



*/