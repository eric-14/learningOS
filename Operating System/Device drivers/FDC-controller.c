/*
                FDC R/W OPERATION SEQUENCE
1. Turn on the drive motor with the IRQ_DMA bit set, 
wait until drive motor is uo to running speed, 
which can be done by either a delay or a timer request.
Also, set a timer request to turn off the drive motor with an appropriate delay time, e.g. 2 s after the command is finished

2. If needed, reset, recalibrate the drive and seek to right cylinder
3. Set up the DMA controller for data transfer
4. Poll the main status register until drive is ready to accept command
5.Write a sequence of command bytes to the DATA register.

For specific drive tyoe, the last 4 command bytes are always the same and can be regarded as constants

6. After executing the command, the FDC will interrupt at IRQ6.
    when a FD interrupt occurs, if the command has a result phase, the driver can read the result phase
    .e.g. recalibrate and seck, which require an additional sense of command to be sent in order to reaf the status bytes ST0-ST2

7. Wait fo FDC interrupt; send a sense command if needed. Read results from DATA register to get status ST0-ST2
    and chack for error. If error, retry steps (2) - (7)

8. If no error, the command is finished. Start next command. If no more commands in 2s , turn off the drive motoe by timer

*/

//Floppy Driver data structure 

struct floppy{          //drive struct , one per drive
    u16 opcode;         //FDC_READ or FDC_WRITE
    u16 cylinder;       //CYLINDER NUMBER
    u16 sector;     //sector: counts from 1 NOT 0
    u16 head;       //head number
    u16 count;       //byte count (BLOCK_SIZE)
    u16 address;        //virtual address of data area
    u16 curcyl;         //current cylinder number
    u8 results[7];      //each cmd may generate up to 7 result bytes
    u8 calibration;     //driver is CALIBRATED or UNCALIBRATED flag

}floppy[NFDC];          //NFDC = 1


//SEMAPHORES 

SEMAPHORE fdio =1 ;         //process execute FD driver one at a time
SEMAPHORE fdsem =0;         //for process to wait for FD interrupts

/*
Initialize the FD structure, including semaphores and status flags.
Write 0x0C to DOR register, which turns A: drive's motor off and sets the drive with 
interrupts enabled and using DMA
*/
int fd_init()
{
    struct floppy *fp = floppy[0];
    fdio.value =1; fdsem.queue=0;
    fdsem.value = fdsem.queue =0;
    fp->curcyl =0; fp->calibration =0;
    need_reset = 0;
    motor_status = 0x0C;        //0x0C (0x0D for B drive)
    out_byte(DOR, 0x0C);            // DOR = 00001100 (0X0D FOR B DRIVE)
}




int fdrw(u16 rw, u16 blk, char *addr)
{
    struct floppy *fp = floppy[0];
    int r, i;
    P(&fdio);           //one process executes fd_rw() at a time
    motor_on();         //turn in motro if needed, set 2 sec . turn off time

    fp->opcode = rw;

    r = (2*blk) % CYL_SIZE;         //compute CHS 

    fp->cylinder = (2*blk) / CYL_SIZE;
    fp->head= r / TRK_SIZE;
    fp-> sector = (r % TRK_SIZE) + 1;       //sector counts from 1, not 0

    fp->count  = BLOCK_SIZE;                //BLOCK SIZE

    fp->address =  addr;                    //address

    for(i=0; i <MAX_RETRY; i++){    //try MAX_RETRY times
        ///First check to see if FDC need reset
        if(need_reset) {
            printf("fp reset\n");
            fp_reset();
        }
        //may also need to recalibrate, expecially after reset

        if(fp->calibration == UNCALIBRATED)
        {
            calibrate(fp);
        }
        //seek to the correct cylinder if needed
        if(fp->curcyl != fp->cylinder)
        {
            seek(fp);
        }
        //set up DMA controller
        setup_DMA(fp);
        //write commands to FDC to start RW 

        r = commands(fp);
        if(r==OK) break;

    }
    if(i>=MAX_FD_RETRY)
    {
        printf("FDC error\n");  //error, mosy likely hardware failure
        V(&fdio);               // unlock fdio semaphore
    }

}
/*
For a recalibrate or seek command ot issues a sense command before reading the status
ST0-ST2
Error conditions in the recalibration and seek stages are checked but ignored
*/