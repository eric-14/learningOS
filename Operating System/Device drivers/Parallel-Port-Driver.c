/*
A pc usually has 2 parallel prot denoted as LPT1 and LPT2
which supports parallel printers.

Each parallel port address has a :
                        ---data register
                        ---status register
                        ---command register


Before printing, the printer must be initialized once by :
        INIT: write 0x08 to COMMAND register first then,
        select: write 0x0C to COMMAND register 

To print, write a char to DATA register. Then strobe the printer once by writing as 1 , 
followed by 0 , t bit 0 of the command register,
 which simulates a strobe pulse of width > 0.5 usec. 

 For example, writing 0x1D= 00011101 followed by 0x1C=00011100 strobes the printer with interrupts enabled.
 When the printer is ready to accept the next char, it interrupts enabled.

 When the printer is ready to accept the next char, it interrupts at IRQ7, which uses vector 15.

 In the interrupt handler, read the status register and check for error. if no error, send the
 next char to the printer and strobe it again    

*/
#define NPR 1
#define PORT 0x378      //#define PORT 0x3BC for LPT2
#define STATUS PORT+1
#define COMD PORT+2
#define PLEN 128

#include "semaphore.c"      //semaphore type and P/V operations

struct para{                //printer data structure
    int port;               //I/O port address
    int printing;             //1 if printer is printing
    char pbuf[PLEN];          //circular buffer
    int head, tail;
    SEMAPHORE mutex, room, done;        //control sempahores
}printer[NPR];

pr_init()
{
    struct para *p;
    p = &printer[0];
    printf("pr_init %x\n", PORT);
    p->port = PORT;
    p->head = p->tail =0;
    p->mutex.value = 1;
    p->mutex.queue =0 ;
    p->room.value = PLEN;
    p->room.queue = 0;

    p->done.value = 0;
    p->done.queue = 0;

    /*  INITIALIZE PRINTER AT PORT */

    out_byte(p->port+2, 0x08);  //init

    out_byte(p->port+2, 0x0C);  //int, init, select on

    enable_irq(7);
    p->printing = 0;            //is NOT printing now

}
int strobe(struct para *p)
{
    out_byte(p->port+2, 0x1D);          //may need delay time here
    out_byte(p->port+2, 0x1C);
}

/*      lower-half printer driver           */
int phandler()
{
    u8 status, c;
    struct para *p = &printer[0];
    status =  in_byte(p->port+1);
    //printf("printer interupt status = %x\n",status);

    if(status & 0x08){  //check for noError status only
        if(p->room.value  == PLEN){ //pbuf[]  empty, nothing to print
            out_byte(p->port+2, 0x0C); //turn off printer interrupts
            V(&p->done);                //tell process print is DONE

            p->printing = 0;
            goto out;

        }
        c = p->pbuf[p->tail++];
        p->tail %= PLEN;
        out_byte(p->port,c);
        strobe(p);

        V(&p->room);
        goto out;

    }
    //abnormal status: should handle it not ignored here

    out: out_byte(0x20, 0x20);
}

/*      Upper-half printer driver */

int prchar(char c)
{
    struct para *p = printer[0];
    P(&p->room);                    //wait for room in pbuf[]

    lock();
    if(p->printing=0){                 //printig the char
        out_byte(p->port,c);
        strobe(p);
        p->printing =1;
        unlock();
        return ;

    }
    //already printing, enter char into pbuf[]
    p->pbuf[p->head++] = c;
    p->head %= PLEN;
    unlock();
}

int prline(char *line)
{
    struct para *p = &printer;
    P(&p->mutex);               //one process prints LINE at a time

    while(*line)
    {
        prchar(*line++);//print chars
        P(&p->done);            //wait until pbuf[ ] is DONE
        V(&p->mutex);           //allow another process to print
    }
}
/*
Phandler is the primary function that operates the printer 


Prchar checks whether the phandler is running then writes directly to the port 
Otherwise the character is written to the buffer

Phandler has the character to printed to PORT 
which is the last charcter  in the buffer since it follows a FIFO scheme

Pchar is the next
*/