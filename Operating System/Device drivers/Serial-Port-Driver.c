/*
RS232C is the standard protocol for serial communication.
It defines 25 signal lines between a Data Terminal Equipment(DTE)
and Data Communication Equipment(DCE).

To establish a serial communication between a DTE and a DCE, a sequence of events goes like this.

1. DTE asserts the DTR(DATA TERMINAL READY), which in-forms the DCE that the DTE is ready.
2.The DCE repsonds by asserting DSR (Data Set Ready) and DCD(Data Carrier Detect), 
    which informs the DTE that the DCE is also ready.

3.When the DTE has data to send, it asserts RTS(Request To Send)
        which asks DCE for permission to send.if DCE is ready to receive data, it asserts CTS(Clear To Send).

4. The DTE can senf data through TX while CTS is on .
5. The DCE can send data through Rx to DTE anytime without asking for permission.


            SERIAL DATA FORMAT

Bits timing by a local(clock), which determines the baud rate.
Baud rate -- refers to the number of signal changes per second.

For binary signals, baud rate is the sname as bit rate.Each 8-bit data requires a start bit and at least one stop bit.

with one stop bit and a baund rate of 9600, the data rate is 960 chars per second.

make 2 DTE'S communiacte using serial cable:
    --configure PC's serial port as a DCE.
    --Make both DTE's think the other side is a DCE by crossing some wired in the cable

A serial terminal generates ASCII code directly.
The most widely used serial terminal standard is the VT100 protocol.
VT100 also supports escape sequence codes

*/

//each serial port is represented by a stty structure

struct stty{
    //input section
    char inbuf[BUFLEN];
    int inhead, intail;
    SEMAPHORE inchars;

    //output section
    char outbuf[BUFLEN];
    int outhead, outtail;
    SEMAPHORE outspace;

    //control section
    char erase, kill, intr, x_on, x_off, tx_on;

    //I/O port base address
    int port;
}stty[NSTTY];   //NSTTY =  number of serial ports



//Serial Port Initialization
/*

IER -- Interrupt Enable

To initialize serial ports, write values to their control registers in the following order:
    0x80 to DATA : bit#7 = 1        //Use DATA , IER registers
    0x00 to IER : 0 value           // for divisor 
    0x0C to DATA : divisor=12       // divisor =12 for 9600 bauds
    0x03 to LCR : line control = 00000011   //no parity, 8_bit data
    0x0B to MSR : Modem Control = 00001011  // turn on IRQ, FTS , DTR
    0x01 to IER : Int Enable = 00000001        // TX off , Rx on
    tx_on  = 0;                                 // Tx interrupt disabled

*/

/*
The lower-half of the serial port drivers is the interrupt handlers, which is set up as follows

1. Set interrupt vectors for COM1(IRQ4) and COM2(IRQ3) by 

        set_vectors(12, s0inth);            vector 12 for COM1
        set_vectors(11, s1inth);            vectors 11 for COM2

2. Install interrupt handler entry points by the INTH macro in assembly

            _sointh: INTH s0handler     //entry points of serial inerrupt handlers
            _s1inth: INTH s1handler

Write s0handler() and s1handler() functions, which call shandler(int port)

            int s0handler(){ shandler(0);}
            int s1handler(){ shandler(1);}

3. Serial Port interrupt handler Function in C;
        The actions of the interrupt handler are as follows:
                --Read Interrupt ID register to determine the interrupt case
                --Read line and modem status registers to check for errors
                --Handle the Interrupt
                --Issue EOI to signal end of interrupt processing 



*/

int shandler(int port){ //port=0 for com1; 1 for COM2
    struct stty *tty = &stty[port];
    int IntID, LineStatus , ModemStatus, intType;
    IntID = in_byte(tty->port+IIR);     //read interruptID
    LineStatus = in_byte(tty->port+LSR);    //read Linestatus
    ModemStatus = in_byte(tty->port+MSR);   //read modemmStatus

    intType = IntID & 0x07;                 // mask in lowest 3 bits of IntID

    switch(intType){
        case 0 : do_modem(tty); break;
        case 2 : do_tx(tty); break;
        case 4 : do_rx(tty); break;
        case 6 : do_errors(tty); break;

    }
    out_byte(0x20,0x20);                    //Issue EOI to PIC controller


}
/*
We shall ignore errors and modem status changes and consider inly rex and tx interrupts for data transfer

in do_rx(), it only catches Control_C as the interrupt key, which send a SIDINT(2)
signal to the process on the terminal and forces a line for the unblocked process to get 

It considers other inputs as ordinary keys. The circular input buffer is used in the same way as in the keyboard driver.

Similar to keyboard driver case, we assume that there is a process runnning on the serial terminal.

SO do_rx() only eneters raw input buffer. The process will cook the raw inputs into lines and echo chars

*/

int do_rx(struct stty *tty)
{
    char c =  in_byter(tty-> port); //get char from data register
    if(tty-> inchars.value >= BUFLEN){ //if inbuf is full
            out_byte(tty, BEEP);        // SOUND BEEP = 0x7
            return;
    }
    if(c==0x3){                     //Control_C key
        send SIGINT(2);             //signal to processess
        c='\n';             //force a new line
    }
    tty->inbuf[tty->inhead++] = c;      //put a char into inbuf
    tty->inhead %= BUFLEN;              //advance inhead
    V(&tty->inchars);                   //inc inchars and unblock sgetc()
}


int do_tx(struct stty *tty)
{
    if(tty->outspace.value == 0)    //no more outputs
    {   
        tty->tx_on = 0;         // turn of Tx interrupt
        return;
    }
    char c = tty->outbuf[tty->outtail++];    //output next char
    out_byte(tty->port,c );
    V(&tty->outspace);
}

/*
if there are no more chars to output, it turns of Tx interrupt and returns.
Otherwise, it putputs the next char from putbuf and V the outspace semaphore 
to unblock any process that may be waiting for room in outbuf

*/

/*
                UPPER-HALF SERIAL PORT DRIVER

Includes sgetc() and sputc() which are callled by processes to input/output chars from/to serial port

*/

char sgetc(struct stty *tty)        //return a char from serial port
{
    char c;
    P(&tty->inchars);               //wait if no input char yet

    lock();
    c = tty->inbuf[tty->intail++];
    tty_>intail %= BUFLEN;
    unlock();
    return c;

}

int sputc(struct stty *tty, char c)
{
    P(&tty->outspace);          //wait for space in outbuf[]
    lock();                     //disable interrupts
    tty->outbuf[tty->outhead++] =c ;
    tty->outhead %= BUFLEN;
    if(!tty->tx_on){enable_tx(tty);}        //turn on tty's tx interrupt

    unlock();                               //enable interrupts
}