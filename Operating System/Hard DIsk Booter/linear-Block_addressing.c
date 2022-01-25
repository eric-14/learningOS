// the parameter of INIT13-42 are specified in a Disk Address Packet (DAP)
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

struct dap{
    u8 len; //DAP structure for INT13-42
    u8 zero; //dap length=0x10 (16bytes)
    u16 nsector; //actually u8; sectors to read=1 to 127
    u16 addr; //memory address = (segment, addr)
    u16 segment; //segemnt value
    u32 sectorLo; //low 4 bytes of LBA sector#
    u32 sectorHi; // High 4 bytes of LBA  sector#
};

// to call INIT13-42 we define a gloabal dap structure and initialize it at once
struct dap dap, *dp=&dap; //dap and dp are global variables in c

dp->len = 0x10; //dap length
dp->zero = 0; //this field must be 0
dp->sectorHi =0 ; //assume 32-bit LBA, higj 4-byte always 0

// other fields will be set when the dap is use in actual call

// within c code we may set dap's segemnt then call getsector to load one more memory disk sector into the memeory location

int getSector(u32 sector, u16 offset)
{
    dp->nsector = 1;
    dp->addr = offset;
    dp->sectorLo = sector;
    diskr();
}

// loads nblk contiguos disk blocks into memory, beginnig form (segement , offset),
where nblk<= 15 because dp->nsectors<+127
int getblk(u32 blk , u16 offset, u16 nblk)
{
    dp->nsectors = nblk*SECTORS_PER_BLOCK; //max value=127
    dp->addr = offset;
    dp->sectorLo = blk*SECTORS_PER_BLOCK;
    diskr();
}

//DISKR IS IN ASSEMBLY WHICH USES THE GLOBAL DAP TO CALL BOIS INT13-42


/*

!---------------ASSEMBLY CODE
*/

    .global _diskr,_dap !_dap is a global dap struct in c 

_dickr:
        mov dx, #0x0080 ! device= first hard drive
        mov ax, #0x4200 ! ah = 0x42
        mov si, #_dap ! (ES,SI) points to _dap

        int 0x13 ! call BIOS INIT13-42 to read sectors 

        jb _error ! to error() if CarryBit is set (read failed)
        ret























