#include "include.h"

int init(){
    //initialize display driver 
    vid init();        //initialize display driver 
    printf("Welcome to MTX in 32-bit protectede mode\n");
    kernel_init();   //initialize kernel data structs, create and run P0

    // initialize IRQ AND IDT 
    printf("remap IRQs; installl IDT vectors \n");
    remap_IRQ();                            //rempa IRQ0-15 TO 0X20 - 0x2F
    trap_install();                         //install trap handlers
    printf("install interrupt vectors and handlers\n");
    int_install(0x20,(int)tinth);         //timer IRQ was 0 
    int_install(0x21, (int)kbinth);        //KBD IRQ was 1
    int_install(0x23, (int)slinth );        //serial port 1
    int_install(0x24,(int)s0linth);         //serial port 0 
    int_install(0x26,(int)fdinth);          //FD IRQ was 6 
    int_install(0x27, (int)printh);         //printer IRQ was 7 
    int_install(0x2E,(int)hdinth);          //IDEO was IRQ 14 
    int_install(0x2F,(int)cdinth);             //IDE1 was IRQ 15
    int_install(0x80,(int)int80h);              //syscall = 0x80


    printf("intialize I/O buffers and device drivers\n");
    binit();        //I/O buffers 
    fd_init();
    hd_init();
    kb_init();
    serial_init();
    //etc all the device drivers 
    timer_init();       //timer ; 8-byte BIOS TOD at 0x9000
    fs_init();          //initialize FS and mount FS 
    running->res->cwd = root;       //set P0's CWD to root
    main();                         //call main in t.c

}