/*
ELF executable file format. begins with an elf-header followed by one or more program headers

*/

typedef unsigned int  u8;
typedef unsigned long u16;
typedef unsigned double u32;


struct elfhdr{
    u32 magic;
    u8 elf[12];
    u16 type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoffset; //byte offset of program header 
    u32 shoffset; //byte offset of sections 
    u32 flags;
    u16 phentsize; //program header size
    u16 phnum;     //Number of proogram section headers
    u16 shentsize;
    u16 shnum;
    u16 shtstrndx;
};

//ELF Program section header 

struct proghdr{
   u32 type;        //l = loadable image 
   u32 offset;      // byte offset of program section 
   u32 vaddrr;      //virtual address 
   u32 paddr;       //physical address
   u32 filesize;    //number of bytes of program section 
   u32 memsize;     //load memory size
   u32 flags;       //R|w|Ex flags 
   u32 align;       //alignment 
};
/*
When loading an ELF file the loader must load the various sections of an elf file to their specified virtual addresses. 
In addition, each loaded section should be ,arked with appropriate R|W|Ex attributes for protection.

Code section pages should be marked as REx, data section as RW 
*/