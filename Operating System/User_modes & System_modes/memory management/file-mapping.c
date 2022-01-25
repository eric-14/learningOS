/*
Virtual memory maps process images from disks into memory.
File I/O maps file contents from disks into memory 
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

int fd , i , BLK = 4096;

main(int argc, char *argv[ ])   //usage: a.out filename
{
    if((fd = open(argv[1], O_RDONLY)) < 0)
    { printf("open %s failed\n",argv[1]);
        exit(1);
    }
    /*     nmap(startAddr, nbytes, usage flags, fileDes, offset )*/

    char *p = nmap(0, BLK , PROT_READ, MAP_SHARED, FD, 0);
    printf("-------------file contents------------\n");
    for(i=0;i<BLK;i++){ putchar(*p++); }
}
/*
The example program maps the first 4096 bytes of an opened file (descriptor) to memory at the starting address p.
This allows the process to access the file contents as if they were memory locations.
When running the program as a.out filename, it diplays the first 4096 bytes of the opened file.

File mapping is a natural and powerful extension of the VA concept. In addition to its execution image in memory, a process may include all opened files in its VA space
*/

/*

Usage if the objects must meet the following requirements:
1. Consistency : the objects must be used in a consistent manner ,e.g. every free page frame is represented by a unique free object, evry disk block is represented by a unique I/O buffer
2. Cache effect: the objects must retain their contents for reuse until they are reassigned 

*/