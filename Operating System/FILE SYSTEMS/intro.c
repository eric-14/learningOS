/*
File Operation levels
1. Hardware level
    fdisk : divide a hard disk or usb drive into partitions for file systems
    mkfs : format a disk partition to make it readt for file system
    fsck : check and repair file system
    defragmentation : compact files in a file system

2. File System Functions In OS kernel:
    every operating system kernel provides support for basic file operations
        -mount/ unmount file systems
        -make/remove directory 
        -change directory, get CWD pathname 
        -change r|w|x permissions, owner , time
        -create/open file for R,W,RW, APPEND
        -read/write opened files
        -lseek/close file descriptors
        -create/ read symbolic link files
        -get file status/information
        -open/read directories

3. System Calls: User mode programs use system calls to access kernel functions
4. Library I/O Functions:  
    system calls allow user to read/write chunks of data, 
    which are just a sequence of bytes

5,User Commands Users may use UNIX/LINUX commands to do file operations 

6. Sh scripts: programs written in sh programming language, which can be executed by the command interpreter sh.

*/

//              ALGORITHM OF MKFS DEVICE, NBLOCKS , [NINODES]
/*
    1.open device for RW mode, write to last disk block;
    2.if ninodes is not specified, compuute the ninodes=8*((nblocks/4)/8)
    3. based in ninodes, compute nimber of inode bloxks and used blocks
    4.create a super block, write to block#1
    5.create a group desciptor 0, write to block#2
    6.create blocks bitmap; write to block#3
    7.create inodes bitmap; write to block#4
    8.clear inodes blocks on disk; allocate a data block, create a root inode in memory 
      with i_block[0] = allocated data block, write the root inode to #2 inode on disk

    9. Initialize root inode's data block to contain and ..entries;
        write data block to disk

*/

//          IMPLEMENTATION OF LEVEL-1 FS

/*
type.h file  - this contains the data structure types if the EXT2 file system,
                such a s super block, group descriptor, inode and directory entry structures
                it also contains the open file table, mount table , pipes and PROC structures and constants of the MTX kernel

global.c file - examples
                MINODE minode[NMINODES];    //in memory INODES
                MOUNT mounttab[NMOUNT];     //number of mount table
                OFT oft[NOFT];              //opened file instance

util.c file -  contains utility functions of the file system
                the most important utility functions are 
               u32 getino() --- returns the inode number of a pathname the dev number might change when the pathname crosses different mounting points

                MINODE iget() -- returns a pointer to the in-memory INODE of(ddev,ino)
                                the returned inode is unique only one copy of the INODE exists in memory.
                                in addition the minode is locked for exclusive use until is either released or unlocked
                
                iput(MINODE *mip)  : releases or unlocks a minode pointed by a mip

            
*/