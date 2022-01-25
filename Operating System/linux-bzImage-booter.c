// Another way is to load the block that contains the last SETUP and the first kernel sector t 0x1000,
// followed by loading blocks. when loading completes, if the number of SETUP sectors is even, we simply move the loaded image downward (address-wise) by one sector

// the move(segement) function is in assembly which is trivial and therefore not shown

typedef unsigned short u16;
typedef unsigned long u32;

typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;

u16 iblock, NSEC =2;

char b1[1024], b2[1024] , b3[1024];//b2[] and b3[] are adjacent

main()
{
    char *cp, *name[2];
    u16 i,ino,setup,blk,nblk;
    u32 *up;
    INODE *ip;
    GD *gp;
    name[0] = "boot";
    name[1] = "linux"; //hard coded /boot/linux so far

    getblk(2,b1);

    gp = (GD *)b1; //get group0 descriptor to find inode table start block
    // read inode start block to get root inode
    iblock = (u16)gp->bg_inode_table;
    getblk(iblock, b1);

    ip = (INODE *)b1 + (ino % 8);// ip points at root node
    // search for image fie name 

    for(i=0; i <2;i++)
    {
        ino = search(ip, name[i]) - 1;
        if(ino < 0) error(); //if search() failed 
        getblk(iblock + (ino /8), b1);
        ip = (INODE *)b1 + (ino % 8);
    }
    // get setup_sectors from linux BOOTsector[497]
    getblk((u16)ip->i_block[0],b2);
    setup = b2[497];
    nblk = (1+ setup) /2; //number if [boot+setup] blocks

    //read indirect & double indirect blocks before changing ES

    getblk((u16)ip->i_block[12], b2);// get indirect block into b2[ ]

    getblk((u16)ip->i_block[13], b3);//get db indirect block into b3[ ]

    up =(u32 *)b3;

    getblk((u16)*up, b3) //get first double indirect into b3

    setes(0x9000); //loading segment of BOOT+SETUP

    for(i=0; i < 12;i++){
        if(i==nblk){
            if((setup & 1)==0)// if setup=even => need 1/2 block more
            getblk((u16)ip->i_block[i],0);
            setes(0x1000); //set ES for kernel image at 0x1000

        }
        getblk((u16)ip->i_block[i],0); //setup=even: 1/2 setup
        inces();
    }

    //load indirect and double indirect blocks in b2[]b3[]
    up = (u32 *)b2; //access b2[] b3[ ] as u32's

    while(*up++){
        getblk((u16)*up, 0); //load block to (ES, 0)
        inces();
        putc('.');
    }

    //finally , if setup is even, move kernel image DOWN one sector
    if ((setup & 1) ==0)
        for(i=1;i<9;i++)
            move(i*0x1000); //move one 64KB segment at a time
            




}
