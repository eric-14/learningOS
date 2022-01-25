/*
In The IDT, the first 32 trap gates point to the entry of trap handlers.
The remaining 224 entries are filled with a default trap gate. 

this is done by trap_install(), which calls trap_entry() to install trap handlers to install trap handlers in the IDT 
*/
u32 IDT_ADDR  = 0x9F040;
int trap0x00(){};
int trap0x01(){};
int trap0x02(){};
int trap0x03(){};
int trap0x04(){};
int trap0x05(){};
int trap0x06(){};
int trap0x07(){};
int trap0x08(){};
int trap0x09(){};
int trap0x0A(){};


int(*trapEntry[ ])(){
    trap0x00,trap0x01,trap0x02,trap0x03,trap0x04,trap0x04,trap0x05,trap0x06,trap0x07,
    trap0x08,trap0x09,trap0x0A,trap0x0B,trap0x0C,trap0xoD,trap0x0E,trap0x0F,
    trap0x10,trap0x11,trap0x12,trap0x13,trap0x14,trap0x15,trap0x16,trap0x17,
    trap0x18,trap0x19,trap0x1A,trap0x1B,trap0x1C,trap0x1D,trap0x1E,trap0x1F
};

struct idt_descr{
    u16 length; 
    u32 address;
} __attribute__((packed)) idt_descr = {256*8 -1, IDT_ADDR};

void trap_entry(int index, u32 entryPoint){
    u64 idt_gate = 0x00008F0000080000ULL;
    u62 addr = (u64) entryPoint;
    idt_gate = (addr<<32) & 0xffff
    idt_gate|= addr & 0xffff000000000000ULL; 
    idt[index] = idt_gate;
}
void trap_install()
{
    int i;
    for(i=0;i<32;i++){
        trap_entry(i, (int)(trapEntry[i]));
    }
    for(i=32;i<256;i++){
        trapEntry(i,(int)default_trap);

    }
    asm("lidt %0\n\t"::"m"(idt_descr));
}