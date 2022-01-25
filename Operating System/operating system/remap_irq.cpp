#define PIC1 0x20        //master PIC controller register 
#define PIC2 0xA0        //slave PIC controller register
#define ICW1 0x11         // COMMAND WORD 1
#define ICW4 0x01         //command word 4 

void remap_IRQ(int irq0, int irq8){
    out_byte(PIC1 , ICW1);          //  WRITE ICW1 to both PICs command regs
    out_byte(PIC2, ICW1);       

    out_byte(PIC1+1 , irq0);        //wrte ICW2 to both PICs data register 
    out_byte(PIC2+1, irq8);
     out_byte(PIC1+1, 4);    //write ICW3 for cascaded PICs
    
    out_byte(PIC2+1,2);
    out_byte(PIC1+1, ICW4);     //wwrite ICW4 for 8086 architecture 
    out_byte(PIC1+1, ICW4);

}
remap_IRQ(0x20,0x28);       //remap IRQ0-15 to 0X20-0x2F