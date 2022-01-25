#define KEYBD  0x60     //I/O port keyboard data
#define PORT_B  0x61     //port_B of 8255
#define KBIT 0x80          //ack bit
#define NSCAN 58  //number of scan codes
#define KBLEN       64
#define BELL    0x07    
#define F1   0x3B       //scan code of function keys
#define F2  0x3C        
#define CAPSLOCK 0x3A       // scan code of special keys
#define LSHIFT 0X2A
#define RSHIFT 0x36
#define CONTROL 0x1D
#define ALT 0x38
#define DEL 0x53

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


struct kbd{
    char buf[KBLEN];
    int head,tail;
    SEMPAHORE data;             //sempahore between inth and processes
    SEMAPHORE lock;             //ONE active process at a time
    struct proc *blist;         //(background) procs ON THE kbd
    int intr, kill,xon, xoff;

}kbd;
int alt, capslock, esc, shifted, control, arrowKey; //STATE VARIABLES

int kbd_init()
{
    esc=alt=control=shifted=capslock=0;
    kbd.head=kbd.tail =0;
    //define defualt control keys
    kbd.intr = 0x03;        //Control-C
    kbd.kill = 0x0B;        //Control-K
    kbd.xon = 0x11;         //Control-Q
    kbd.xoff = 0x13;        //Control-S

    //initialize sempahores 

    kbd.data.value = 0;
    kbd.data.queue = 0;
    kbd.lock.value= 1;
    kbd.lock.queue = 0;
    kbd.blist = 0;          //no background PROC'S initially
    enable_irq(1);
    out_byte(0x20,0x20);

}

//              LOWER HALF DRIVER
int kbdhandler()
{
    int scode,value,c;
    //Fetch scan code from the keyboard hardware and acknowledge it

    scode = in_byte(KEYBD);     //get scan code
    value = in_byte(PORT_B);        //strobe PORT_B to ack the char

    out_byte(PORT_B, value | KBIT);
    out_byte(PORT_B, value);

    if (scode == 0xE0)// ESC key
    {esc++};            //inc esc count by 1
    if(esc && esc < 2)  //only the first ESC key, wait for next code
    {goto out;}

    if (esc == 2){                 // two 0xE0 means escape sequence key release
        if(scode == 0xE0){       // this is the 2nd ESC, real code comes next
            goto out;}
            //WITH ESC==2 THIS MUST BE THE ACTUAL SCAN CODE, SO HANDLE IT
            scode &= 0x7F;              //leading bit off

            if(scode == 0x38)           //right alt
            {
                alt = 0;
                goto out;
            }
            if(scode == 0x1D)        //right Control release
            {
                control = 0;
                goto out;
            }
            if(scode == 0x48)       //up arrow
            {
                arrowKey = 0x0B;
            }
            esc= 0;
            goto out;
       
    }

    //0x80 --  1000 0000
    if(scode & 0x80){ //checks whether scode has the 7th bit 
        //key release : ONLY catch shift, control,alt
        scode &= 0x7F;      //mask out bit 7
        if(scode == LSHIFT || scode == RSHIFT){
            shifted = 0;            //released the shift key
        }
        if(scode == CONTROL)
        {
            control = 0;            //released Control key
        }
        if(scode == ALT){
            alt =0 ;                //released alt key
        }
        goto out;
    }

    //from here on, must be key press
    if(scode == 1) //Esc key on keyboard
    {goto out;} 
    if(scode == LSHIFT || scode == RSHIFT){
        shifted =1;                 //set shifte flag
        goto out;
    }
    if(scode == ALT){
        alt =1;
        goto out;
    }
    if(scode == CONTROL){
        control=1;
        goto out;
    }
    if(scode ==0x3A){
        capslock =1 - capslock;             //capslock key acts like a toggle
        goto out;
    }
    if(control && alt && scode ==DEL){
        printf("3-finger salute\n");
        goto out;
    }

}

// CATCH AN HANDLE F KEYS FOR DEBUGGING

if(scode ==F1){do_F1(); goto out;}
if(scode ==F2){do_F2(); goto out;}

//translate scan code to ASCII, using shift [ ] table if shifted

c = (shifted ? shift[scode] : unshift[scode]);
//convert all to uppercase if capslock is on

if(capslock){
    if( c>= 'A' && c<='Z')
    {
        c += 'a' - 'A';
    }else if(c >= 'a' && c<='z')
    { c-= 'a' - 'A';}
           
}
if(control && (c=='c' ||c=='C')){// Control-C on PC are 2 keys 
//Control-C Key; send SIGINT(2) signal to processes on console;
c='\n';         //force a line, let procs handle SIG#2 when exit Kmode

}
if(control && (c=='d' || c=='D')){
    //Control-D these are 2 keys
    printf("Control-D: set code=4 to let process handle EOF\n");
    c=4; 
}

/*      enter the char in kbd.nuf[ ] for processs to get */

if(kbd.data.value ==  KBLEN){
    printf("%c\n",BELL);
    goto out;                       //kb.buf[ ] already full
}

kbd.buf[kbd.head++] = c;
kbd.head %= KBLEN;
V(&kbd.data);

out:
    out_byte(0x20,0x20);


/*         UPPER-HALF DRIVER   */
int getc()
{
    u8 c;
    if(running->fground==0){
        P(&kbd.lock);           //only foreground proc can getc() from KBD
    P(&kbd.data);
    lock();
    c = kbd.buf[kbd.tail++];
    kbd.tail %= KBLEN;
    unlock();
    return c;
    }
}

/*
Control variables are usually managed by stty() and ioctl() syscalls

The KBD driver uses the state variables, 
esc, control, shifted , alt to keep track of the states of the driver.

For example, esc = 1 when it sees the first ESC(0xE0)

with esc=2, the next scan code determines the actual special key , which also clears esc to 0

Key press or release: Key release is of interest only to shift, control and Alt keys
because releasing such keys rests the state variable shifted, control and alt.

Special key processing examples:

    .Function Keys: These are the simplest to recognize. Some of the function keys may be used as 
    hot keys to perform specific functions.
    For example, pressing the F1 key may invoke ps() to print the process status in kernel

    .Shift key: (LeftShift=0x2A, RightShift=0x36): when either shift key is pressed but not yet released,
     set shifted to 1 and use the shifted keymap to translate the input chars.
    Releasing either LSHIFT or RSHIFT key resets shifted to 0.

    .Control key sequence : Control-C Left CONTROL key (0x1D) down but not released, set control=1. Releasing the CONTROL key resets control to 0.
    With control=1 and a key press, determine the Control-key value

    .Esc Key sequence: Many special keys , e.g. arrow keys, Delete , etc. generate scan code E0xx, where E0 is the ESC key.
    After seeing 2 ESC keys, set esc =2. With esc=2, get the next key press, e.g. Up arrow=E048, to determine the special key 



*/