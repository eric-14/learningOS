/*
in EXT4 the i_block[15] array of an inode conatins a header and 4 extents 
structures, each 12 bytes long
*/

typedef unsigned long u32
// |header|extent1|extent2|extent3|extent4|

struct ext3_extent_header{
    u16 eh_magic; //0xf30A
    u16 eh_entries; //number of valid entries
    u16 eh_max; //capacity of store in entries
    u16 eh_depth; //has tree real underlaying blocks?
    u32 eh_generation; // generation of the tree
};

struct ext3_extent {
    u32 ee_block; //first logical block extent covers
    u16 ee_len; //number of blocks vovered by extent
    u16 ee_start_hi; //high 16 bits of physical block
    u32 ee_start;   //low 32 bits of physical block
}

/*
GD and INODe types are the same as they are in EXT2 but the INODE
size if 256 bytes.

The super block's magic number if also the same as in EXT2,

but we may test s_feature_incompat(>0x240) to determine whethe it is an EXT4 file system

---> Blocks in each extent are contiguos. There i sno need to scan for contiguos blocks when loading an image 
just load the sequence of blocks directly.
 for HDs the block size is 4KB 

The maximum number of blocks per loading is still limited to 16 or less

*/

//          SEARCH FOR NAME IN AN EXT4 DIR INODE

//          ***intergrate it to the HD BOOTER

U32 search(INODE *ip, char *name)
{
    u16 i; u32 ino;
    struct ext3_extent_header *hdp;
    struct ext3_extent *ep;

    char buf[BLK];

    hdp = (struct ext3_extent_header *)&(ip->i_block[0]);
    ep = (struct ext3_extent *)&(ip->i_block[3]);
    for(i=0; i<4;i++){
        if(hdp->eh_entries == 0 ){
            getblk((u32)ip->i_block[0],buf ,1);
            i=4; // no other extents
        }
        else{
            ep = (struct ext3_extent *)&(ip->i_block[3]);
            getblk((u32)ep->ee_start, buf , 1);
        }
        if(ino = find(buf, name)) //find name string in buf [ ]
            { return ino;}
    }
    return 0;
}


//          load blocks of an inode with EXT4 extent

int loadEXT4(INODE *ip, u16 starblk)
{
    int i ,j ,k remain; u32 *up;
    struct ext3_extent_header *hdp;
    struct ext3_extent *ep;

    int ext; 

    u32 fblk, beginblk;

    hdp = (struct ext3_extent_header *) ip->i_block;
    ep = (struct ext3_extent *)ip->i_block +1;

    ext = 1;
    while(1){
        if (ep->ee__len==0)
        break;
    beginblk =0 ;
    if(ext ==1) // if first extent: begin form startblk
        beginblk = startblk;
    
    }
    k =16; //load 16 contiguos blocks at a time
    fblk = ep->ee_len - beginblk;
    remain = ep->ee_len - beginblk;

    while(remain >= k){
        getblk((u32)(fblk),0 , k);
        cp_vm(k,'.');
        fblk += k;
        remain -=k;
    }
    if(remain){
        getblk((u32)(fblk),0,remain);
        cp_vm(remain, '.');
    }
    ext++;ep++; // next element
}