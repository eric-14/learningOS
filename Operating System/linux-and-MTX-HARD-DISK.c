// The booter can boot both MTX and Linux with initial RAM disk support
// it can also boo windows by chain-booting 

/*
    ALGORITHM OF HD -BOOTER'S BC.C FILE
*/
#define BOOtSEG 0x9800
#include 'bio.c' //i/o functions such as printf()
#include "bootlinux.c" //c code of linux booter

int main(){
    /*
    initialize dap for INIT13-42 calls
    read MBR sector
    print partition table
    prompt for a partition to boot
    if(partition type=LINUX)
        bootLinus(partition); no return
    load partition's local MBR to 0x07c0;
    chain boot from partition's local MBR
 
    */
}

/*      ALGORITHM OF BOOTLINUX.C
boot-Linux-zImage Algorithm

    read superblock to get blocksize, inodesize, inodes_per_group
    read Group Descriptor 0 to get inode start block
    read in the root INODE anf let INODE *ip poiny at root INODE

    prompt for a linux kernel image file to boot
    tokenize image filename and search for image's INODE

    handle symboliz-link filenames
    load BOOT+SETUP of linux bzImage to 0x9000

    set video mode word at 506 in BOOT to 773 (for small font)
    set root dev word at 508 in BOOT to (0x03, pno) (/dev/hdapno) --> optional set if initrd is unavailable

    set bootflags word at offset 16 in SETUP to 0x2001 --> tells SETUO that the kernel image is loaded by an "up-to-date" boot loader, otherwise SETUP would consider the kernel image invalid and refuse to start the linux kernel
    compute number of kernel sectors in last block of SETUP

    load kernel sectors to 0x1000, then c2phumem() to 1MB
    load kernel blocks to high memeory, each time load 64KB

    load initrd address and size to offsets (24,28) in SETUP

    jmp_setup() to execute SETUP code at 0x9020


*/