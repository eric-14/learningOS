/*
linux kernel if loaded to the physical address
0x100000 (1 MB) if a ram disk image is specified it is also loaded to high memory

since pc is in 16 bit real mode it cannot access memory above 1MB directly
instead of switching to protected mode then to real mode again 

use INIT15-87 --> which is designed to copy memory between real and protected modes

paramters of INT 15-87 ARE SPECIFIED IN A GLOBAL DESCRIPTOR TABLE (GDT)


*/
struct GDT 
{
    u32 zeros[4]; //16bytes 0's for BIOS to use 
    //src address
    u16 src_seg_limit; // 0xFFFF = 64KB
    u32 src_addr; // low 3 bytes of src addr, high_byte = 0x93
    u16 src_hiword; //0x93 and high byte of 32-bit src addr
    //deset address
    u16 dest_seg_limit; // 0xFFFF = 64KB
    u32 dest_addr; //low 3 bytes of dest addr, hogh byte=0x93
    u16 dest_hiword; //0x93 and high byte of 32-bit dest addr 

    //BIOS CS DS 

    u32 bzeros[4];
}
int_gdt(struct GDT *p)
{
    inti i;
    for(i=0;i<4;i++){
        p->zeros[i] = p->bzeros[i] = 0;
    }
    
    p->src_seg_limit = p->dest_seg_limit = 0xFFFF; //64KB SEGEMENTS
    p->src_addr = 0x93010000; //bytes 0x00 00 01 93
    p->dest_addr = 0x93000000; //bytes 0x00 00 00 93
    p->src_hiword = 0x0093; // bytes 0x93 00
    p->dest_hiword = 0x0193; //bytes 0x93 01
}


code :
    struct GDT gdt; //define a gdt struct
    init_gdt(&gdt);// initalize gdt as shown 
    cp2himem(); //assembly code that does the copying

Assembly code:
    .globl _cp2himem, _gdt ! _gdt  is a gloabl GDT from c 

_cp2himem:
        mov cx, #2048 !  CX = number of 2-byte words to copy
        mov si, #_gdt ! (ES,SI) point to gdt struct 
        mov ax, #0x8700 !aH=0x87
        int 0x15 ! call BIOS INIT15-87
        jc _error 
        ret 

   









