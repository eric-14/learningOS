/*
CDROM drives use the TAPI(AT attachment packey Interface) protocol

At the hardware level, ATAPI allows ATAPI devices to be connected directly to the IDE bus.

All the I/O port addresses are the same as in PATA 
The major difference between PATA and ATAPI is that ATAPI ussus SCSI-like packets
to communicate with ATAPI devices.

A packet consists of 12 bytes, which are written to the data port when the drive is ready 
to accept the packet.

                ATAPI PROTOCOL

The status(0x177) and error(0x171) registers are the same as in PATA, 
excpet that some of the bits, 
especially those in the error register are redefined for ATAPI devices

The interrupt reason register(0x172) is new in ATAPI
When an ATAPI interrupt occurs,
 IO indicates the data direction(0=toDEVICE), 1=toHOST)

 and CoD indicates whether the request is for Commanf packet(0) or DATA(1)

                ATAPI SEQUENCE
1.host: poll for BSY=0 and DRQ=0 then write to feature, byteCount, drive registers,then, 
write packet command 0xA0 to command register(0x177)

2. drive: set BSY, prepare for command bytes transfer. Then set(IO,coD) 
to (01) and assert DRQ cancel BSY

3. host: when DRQ=1 write 12 command byters to Data register 0x180; then wait for drive interrupt

4. drive: clear DRQ, set BSY, read feature & byterCount regs. For PIO mode
    put byter countinto byteCOunt regsm set(IO,coD)=(10) DRQ=1, BSY=0, then genearte interupt INTRQ

5host: interrupt handler: read status register(to clear INTRQ) to get DRQ.
        DRQ=1 means data reqdy for PIO. Read data port by actual byte count

6.drive: Sfter data read by host , clears DRQ=0 if more data, set BSY =1 , repeat (4) to (6) until all needed bytes are transferred

7. drive: When all needed data are transferred,issue final status interrupt by BSY=0 (IO,Cod,DRQ)=(110) and INTRQ

8. hist: final status interrupt read status register and error register 



*/
#define CD_DATA 0x170          // data port for R/W
#define CD_ERROR 0x171          //error register/ write-feature register
#define  CD_INTREA 0x172        //Interrupt reason 
#define CD_NO_USE  0x173        //not used by ATAPI
#define CD_LBA_MID 0x174        //byte_count low byte
#define CD_LBA_HI 0x175         //byte_count HI byte
#define CD_DRIVE 0x176          //drive selct (0x00 == master 0x10 =-slave)
#define CD_CMD  0X177           //command: 0xA0 (packet command)
#define CD_STATUS 0x177         //status register 

//      UTILITY FUNCTIONS

int cd_busy() {return (in_byte(0x177) & 0x80);}

//BUSY and READY are complementary; only need cd_busy
int cd_ready() {return (in_byte(0x177) & 0x40);}

int cd_DRQ(){return (in_byte(0x177) & 0x08);}

int cd_error()
{
    int r = in_byte(0x177);     //read status REG
    if(r & 0x01){               //if status.ERROR bit on
        r =  in_byte(0x171);        //read error REG
        printf("CD RD ERROR = %x \n",r);
        return r; 


    }
    return 0;
}

//      COMMON DATABASE OF ATAPI DRIVER
int cderror;
char *bufPtr;
u16 byteCount;
u16 procSegment;            //calling process Umode segment

//              ATAPI DRIVER UPPER-HALF

int getsector(u32 sector, char *buf, u16 nsector)
{
    int i; 
    u16 *usp;
    u8 *cp;
    printf("CD/DVD getsector : sector = %l\n",sector);
    cderror = 0;               // clear error flag
    bufPtr = buf;                               //pointer to data area
    procSegment  =  running->uss;           //if buf is in Umode
    cd_sem.value = cd_sem.queue =0 ;        //initialize cd_Sem to 0


    //create packet : packet.lba[4]; low-byte= MSB hi-byte= LSB

    zeropkt();
    pkt[0]  =   0x28;           //READ 10 opcode in pkt[0]
    cp = (u8 *)&sector;         //sector LBA in pkt[2-5]
    pkt[2] = *(cp+3);           //low-byte = msb
    pkt[3] = *cp(cp+2);         // LBA byte 1
    pkt[4] = *(cp+1);           // LBA byte 2
    pkt[5] = *cp;               //high byte= LSB
    pkt[7] = 0;                 // pkt[7-8] = number if sectors to READ
    pkt[8] = nsector;           //nsectors's LSB
    out_byte(0x376, 0x08);      //nInt = 0 => enable drive interrupt
    out_byte(0x176, 0x00);      // device : 0x00 = master

    //poll for notBUST and then ready;
    while(cd_busy() || cd_DRQ()); // wait until not bust and DRQ=0

    // write to features regs 0x171 , byte count , drive registers

    out_byte(0x171 ,0);     //feature REG  = PIO (not DMA)

    //Write host, NEEDED byte_count into byteCount rgs 0x174 - 0x175

    out_byte(0x174, (nsector*2048) & 0xFF);     //LOW BYTE
    out_byte(0x175, (nsector*248)>> 8);         //high byte

    //3. write 0xA0 to command register 0x177
    //printf("write 0xA0 to cmd register\n")

    out_byte(0x177, 0xA0);                  //drive starts to process packets

    //wait until notBUSY && READY && DRQ is on

    while(cd_busy() || !cd_DRQ());  //wait until notBUSY and DRQ=1

    //printf("write 6 word of packets to 0x170\n")

    usp = &pkt;
    for(i=0; i<6;i++)
    {
        out_word(0x170, *usp++);
    }
    P(&cd_sem);         //process "wait" for final status interrupt

    return cd_error;

}
int cdhandler()
{
    u8 reason , status, err;
    u16 byteCount;
    //read status, interrupt reason and error registers; get status.DRQ

    status = in_byte(0x177);

    reason = in_byte(0x172) & 0x03;
    err = in_byte(0x171);

    if(status & 0x01){
        //status.ERROR bit on ==> set cderror flag
        cderror=1;
        V(&cd_sem);         //unblock process
        if(err & 0x08)      //err=xxxxMyy : M=need media change => ignore
        {
            printf("media change error\n");
            printf("CD error : status=%x err=%x \n",status ,err);
            goto out;
        }

    }
    if((reason ==0x02) && (status & 0x08) && !(status & 0x80)){
        //printf("CD data ready for PIO");
        byteCount=(in_byte(0x175) << 8) | (in_byte(0x174));
        printf("byteCount=%d\n", byteCount);

        /*      PIO data to Kmode or Umode    */

        read_port(0x170, getds(),bufPtr,byteCount);     //PIO to Kmode 

        read_port(0X170, procSegment, bufPtr, byteCount);//to Umode
        goto out;

        if(reason==0x03 && !(status & 0x08)){
            //110: final status interrupt
            printf("final status interrupt:status=%x:V(cd_sem)\n",status);
            V(&cd_sem);
        }
    }

out:
    out_byte(0xA0, 0x20);       //EOI to slave 8259 PIC
    out_byte(0x20, 0x20);       //  EOI to master PIC    
}
