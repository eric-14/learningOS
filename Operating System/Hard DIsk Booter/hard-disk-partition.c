struct partition{
    u8 drive; //0x80 - active
    u8 head; //starting head
    u8 sector; //starting sector
    u8 cylinder; //starting cylinder
    u8 sys_type; //partition type
    u8 end_head; //end head
    u8 end_sector; //end sector
    u8 end_cylinder; // end cylinder
    u32 start_sector; //starting setor
    u32 nr_sectors; //number of sectors

};

/*
if a parition is EXTEND type(5) it can be divided intp more partitions 
Assume that partition P4 is EXTEND type and it is divided into extend partitions P5,P6, P7

The extend partitions for a link list 

PARTITION TABLE    -
    conatins the start sector and number and size of the extend partition

*/



// given a group number, we must find its group descriptor and use it to finf the groups's inodes start block.
// How to convert inode number to an inode 

//          ALGORITHM THAT AMOUNTS TO MAILMAN'S ALGORITHM TWICE


//compute group# and offset# in that group
 
group = (ino-1) / inodes_per_group;
inumber =  (ino-1) % inodes_per_group // offset from group number

//Find the group's group descriptor

gdblk = group / desc_per_block; //which block this GD is in
gdisp = group % desc_per_block; //which GD in that block


//compute the inodes's block # and offset in that group

blk = inumber / inodes_per_block; //blk# r.e to group inode_table
disp = inumber % inodes_per_block; //inode offset in that block

//Read group descriptor to get group's inode table start block #

getblk(1+first_data_block+gdblk,buf, 1); //GD begin block

gp = (GD *)buf + gdsip

blk += gp->bg_inode_table; //blk is r.e to group's inode_table

getblk(blk, buf, 1); //read the disk block containing inode

INODE *ip = (INODE *)buf+ (disp*iratio); //iratio=2 if inode_size =256

//when the algorithm finishes the input should point to the file's inode in memory 

/*
intial RAM DISK (initrd) - a small file system which is used by the linus kernel as a temporary root file system when the kernel 

it contains a minimal set of directories and executables such as sh , ismod tool and the neede driver modules 

while running initrd the linuz kernel typica;;y executes a sh script, initrc , to install the neededdriver modules and activeate the real root device 

kernel abandons intrd and mounts the real root file system

intrd is mainly used to increase boot up speed
by allowing a single generic linux tobe used on all linux distributions

all other drivers may be installed as modules from intrd

Assume initrd.img is a RAM disk image file

loading initrd image is similar to loading kernel image, only simpler

has a mx limit address of 0xFE000000

after loading completes the booter must write the loading address and size of the initrd image to SETUP at 
the byte offset 24 and 28, respectively

Then it jumps to execure SETUP at 0x9020

in kernel SETUP requires DS= 0x9000 in order to access BOOT as the beginnig of its data segement

*/


