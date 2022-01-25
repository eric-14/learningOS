/*  Scan codes to ASCII for unshifted keys */
char unshift[NSCAN] = { //NSCAN=58
    0,033,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w',
    'e','r','t','y','u','i','o','p','[',']','\r','o','a','s','d','f','g','h','j'
    ,'k','l',';',0,0,0,0,'z','x','c','v','b','n','m',',','.','/',0,'*',' '};

/*  scan codes to ASCII for shifted keys*/

char shift[NSCAN] = {
    0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\r',0,'A','S','D','F','G',
    'H','J','K','L',':',0,'|','Z','X','C','V','B','N','M','<',
    '>','?',0,'*',0,' ',
};




#define KEYBD 0x60        //I/O port for keyboard data
#define PORT_B  0x61            //port_B OF 8255
#define KBIT  0x80              // bit used to ack chars to keyboard
#define KBLEN 64
#define NSCAN   58              // number of scan codes
#define SPACE  0X39             // space char, above which are special char

struct kbd{
    char buf[KBLEN];        //CIRCULAR BUFFER FOR INPUT CHARS
    int head, tail;
    int data;               //number of characters in buf[ ]
}kbd;

int kbd_init()
{
    kbd.head = kbd.tail = kbd.data = 0;
    enable_irq(1);          //enbale IRQ1
    out_byte(0x20, 0x20);
}

int kbhandler()
{
    int scode, value, c;
    //get scan code from keyboard's data port and ack it

    scode = in_byte(KEYBD);  //get scan code
    value = in_byte(PORT_B);     //strobe PORT_B to ack key

    out_byte(PORT_B, value | KBIT);       //first, set the bit high
    out_byte(PORT_B, value);            //then set it low

    //printf("kbd interrupt %x\n", scoder);

    if(scode & 0x80 || scode > SPACE)//ignore release OR special keys
    { goto out;} 

    c  = unshift[scode];         //map scan code to ASCII char

    if(kbd.data == KBLEN){//      sound warning if buf is full
        printf("%c kbd buf FULL!\n",007);
        goto out;

    }
    kbd.buf[kbd.head++] = c;        //enter ASCII char into buf [ ]

    kbd.head %= KBLEN;               //buf[ ] is circular
    kbd.data++;
    wakeup(&kbd.data);               //wakeup process id upper half
out:
        out_byte(0x20, 0x20);         //send EOI to 8259 PIC controller
 

}

/**         upper-half  driver routine          **/
int getc()
{
    u8 c;
    lock();         //kbd.buf[ ] and kbd.data from a CR
    while(kbd.data == 0)//between process and interrupt handler
    {   unlock();
        sleep(&kbd.data);
        lock();
    }
    c = kbd.buf[kbd.tail++];
    kbd.data--;
    unlock();
    return c;

}