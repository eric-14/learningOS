/*
1. Initialize HD: write 0x08 to Control register(0x3F6):(E bit=0 to enable interrupt)

2. Read Status register (0x1F7) until drive is not BUSY and READY
3. Write sector count, LBA sector number, drive(master=0x00, slave=0x10) to command registers(0x1F2-0x1F7)
4.Write READ|WRITE command to command Register(0x1F7)
5. For a write operation, wait until drive is READY and DRQ(drive request for data)
    then, write data to data port
6. Each (512-byte) sector R|W generates an interrupt. I/O can be done in two ways:
    7(a) Process waits for each interrupt. 
        When a sector R/W completes, interrupt handler unblocks process, which continues to read/ wrte the next sector of data from/to data port
    7(b) process starts R/W. for write(multi-sectors) operation , process writes the first sector of data, then waits for the FINAL status interrupt. 
        Interrupt handler transfers remaining sectors of data on each interrupt.
        When R/W of all sectors finishes, it unblocks the process. This scheme is bettter because it does not unblock or wakeup processes unnecessarily

8. Error Handling: After each R/W operation or interrupt, read status register.
                    If status.ERROR bit is on, detailed error information is in the error register(0x1F1).
                    Recovery from error may need HD reset 

*/

//simple hd DRIVER based on 7(a)👆

#define HD_DATA 0x1F0           //data port for R/W
#define HD_ERROR 0x1F1           //error register
#define HD_SEC_COUNT 0x1F2       //R/W sector count
#define HD_LBD_LOW   0x1F3       // LBA low byte
#define HD_LBA_MID  0x1F4       //LBA mid byte
#define HD_LBA_HI 0x1F5         //LBA high byte
#define HD_LBA_DRIVE 0x1F6      //1B1D0000=>B=LBA, D= DRIVE=>0xE0 OR 0xF0
#define HD_CMD   0x1F7         // command: R =0X20 W= 0X30
#define HD_STATUS 0x1F7         // STATUS REGISTER
#define HD_CONTROL 0x3F6        //0 x08(0000 1RE0): Reset , E=1: NO interrupt

/*      HD disk controller command bytes   */

#define HD_READ 0x20    //read
#define HD_WRITE 0x30    //write
#define BAD -1             //return BAD on error
struct semaphore hd_mutex;      //for procs hd_rw() ONE at a time
struct semaphore hd_sem;    //for proc to wait for IDE interrupts

//read_port() reads count words from port to (segment, offset)

int read_port(u16 port, u16 segment , u16 *offset, u16 count)
{
    for(i=0; i<count; i++)
    {
        put_word(in_word(port), segment , offset++); //data to read, segment_to_write, offser
    }
}

//write_port() writes count words from (segment, offset) to port

int write_port(u16 port, u16 segment, u16 *offset, u16 count)
{
    int i;
    for(i=0; i<count; i++){
        out_word(port, get_word(segment, offset++));
    }
}

int delay() { }

int hd_busy() {return int_byte(HD_STATUS) & 0x80;} //test BUSY

int hd_ready() {return in_byte(HD_STATUS) & 0x40;}  //test READY

int hd_drq() {return in_byte(HD_STATUS) & 0x08; }  // test DRQ

int hd_reset()
{
    /*  
    control register(0x3F6) = (0000 1RE0); R=reset, E=0:enable interrupt 
    Strobe R bit from HI to LO; with delay time in between;
                write 0000 1100 to Controlreg; delay();
                write 0000 1000 to ControlReg; wait for notBusy & no error
    */

    out_byte(0x3F6 , 0x0C); delay();    //  reset HD 
    out_byte(0x3F6, 0x08); delay();     //initialize HD 

    if(hd_busy() || cd_error())
    {
        printf("HD reset error\n");
        return(BAD);
    }
    return 0;       //return 0 means Ok
}
int hd_error()      //test for error
{
    int r;
    if(in_byte(0x1F7) & 0x01)   //status.ERROR bit on ---Error 1st bit Track 0 Not Found
    {
        r = in_byte(0x1F1);     //read error register
        printf("HD error= %x\n", r); 
        return r;

    }
    return 0;                      // return 0  if no error
}

int hd_init()
{
    printf("hd init\n");
    hd_mutex.value = 1;
    hd_mutex.queue = 0;
    hd_sem.value = hd_sem.queue = 0;
}
int hdhandler()
{
    printf("hd interrupt!");
    V(&hd_sem);     //unblock process
    out_byte(0xA0, 0x20);       //send EOI
    out_byte(0x20, 0x20);
}

int set_ide_regs(int rw, int sector , int nsectors)
{
    while(hd_busy() && !hd_ready());    //wait until notBUSY & READY
    printf('write to IDE register \n');
    out_byte(0x3F6, 0x08);              //control = 0x08; interrupt
    out_byte(0x1F2, nsectors);          //   sector count
    out_byte(0x1F3, sector);            // LBA low byte
    out_byte(0x1F4, sector>>8);         //LBA mid byte
    out_byte(0x1F5, sector>>16);        //LBA high byte

    //0x0F    ---   0000 1111  0xE ----0000 1110

    //0x1F6 = 1B1D TOP4LBA: B=LBA, D=drive

    out_byte(0x1F6, ((sector>>24) & 0x0F) | 0xE0);    //use LBA for drive 0
    out_byte(0x1F7, rw);                    // READ | WRITE command

}

int hd_rw(u16 rw, u32 sector, char *buf, u16 nsectors)
{
    int i;
    P(&hd_mutex);           //procs execute hd_rw() ONE ata a time
    hd_sem.value =  hd_sem.queue = 0;
    set_ide_regs(rw, sector, nsector);      //set up IDE registers

    //ONE interrupt per sector read|write; transfer data via DATA port

    for(i=0;i<nsectors;i++){ //loop for each sector
        if(rw==HD_READ)
        {
            P(&hd_sem);     // wait for interrupt
            if(err = hd_error()){break;}

            read_port(0x1F0,getds(),buf,256);   //256 2-byte words
            buf+=512;

        }else{ //for HD_WRITE must wait until notBUSY and DRQ=1
            while(hd_busy() && !hd_drq());
            write_port(0x1F0, getds(), buf, 256);
            buf+=512;
            P(&hd_sem);                     //wait for interrupt
            if(hd_error()){break;}

        }

    }
    V(&hd_mutex);
    if(hd_errror()) return BAD;
    return 0;
}


/*
the second HD driver is base in 7(b). In this case, the process sets up data area containing disk I/O parameters such as R/W operation
starting sector, number if sectors e.t.c. Then it starts R/W of the first sectors and blocks untill all the sectors are read or written.
On each interrupt, the interrupt handler performs R|W of the remaining sectors.

When all sectors R|W are finished, it unblocks the process if the last interrupt
*/

/**    HD driver for synchronous  disk I/O          **/
/**         HD I/O  parameters common to hd_rw() and interrupt handler **/

u16 opcode;         // HD_READ | HD_WRITE
char *bufPtr;       // pointer to data buffer 
int ICOUNT;         //sector count 
u16 hderror;        // error flag
int hdhandler()     // HD interrupt handler
{
    printf("HD interrupt ICOUNT=5d\n", ICOUNT);
    if(hderror = hd_error())        //check for error
    {
        goto out;
        //ONE interrupt per sector read|write; transfer data via DATA port

    }
    if(opcode == HD_READ){
        read_port(0x1F0, getds(), bufPtr, 512);
        bufPtr += 512;
    }else{  //HD_WRITE
    if(ICOUNT > 1){
        unlock();           //allow interrupts
        write_port(0x1F0, getds(), bufPtr,512);
        bufPtr +=512;
    }

    }
    if(--ICOUNT==0)
    {
        printf("HD inth: V(hd_sem)\n");
        V(&hd_sem);
    }
    out:
        if(hderror && ICOUNT){
            V(&hd_sem);     //must unblock process evem if ICOUNT > 0
        }
        out_byte(0xA0, 0x20); //send EOI
}

int hd_rw(u16 rw, u32 sector , char *buf, u16 nsectors)
{
    P(&hd_mutex);           // one proc at a time executes hd_rw()
    hd_sem.value =  hd_sem.queue = 0;
    printf("hd_rw: setup I/O parameters for interrupt handler\n");
    opcode = rw;                //set opcode
    bufPtr  =buf;               //pointer to data buffer
    ICOUNT =nsectors ;          //nsectors to R/W
    hderror = 0;                //initialize hderror to 0
    set_ide_regs(rw, sector , sectors);     //set up IDE registers
    if(rw==HD_WRITE){// must wait until notBUSY and DRQ=1
        while(hd_busy() && !hd_drq());
        write_port(0x1F0,getds(),buf,512);
        bufPtr += 512;
    }
    P(&hd_sem);                 //block until r/w of ALL nsectors are done
    V(&hd_mutex);               // unlock hd_mutex lock
    return hderror;

}











