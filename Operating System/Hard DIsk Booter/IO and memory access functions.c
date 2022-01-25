#define MAXLEN 128
typedef unsigned short u16;



char * gets(char s[ ]) //caller must provide real memory s[MAXLEN]
{
    char c , *t =s; int len=0;
    while((c=getc()) != '\r' && len < MAXLEN-1) {
        *t++ =c;putc(c);len++;
    }
    *t =0; return s;
}

//OUTPUT

char *ctable = "0123456789ABCDEF";
u16 BASE = 10; //for deciaml numbers

int rpu(u16 x)
{
    char c;
    if(x){
        c = ctable[x % BASE];
        rpu(x /BASE);
        putc(c);
    }
    int printu(u16 x)
    {
        (x==0) ? putc('0') : rpu(x);
        putc(' ')
    }
}
/*
assume we have prints(),printd(,
printu(),printx(), printl(),
printX(),print1() 
*/


// DOES NOT SUPPORT FIELD WIDTH OR PRECISION 


int printf(char *fmt,...)//some c compiler reuires 3 dots
{
    char *cp = fmt; //cp points to the format string
    u16 *ip = (u16 *)&fmt +1 ; //ip points to the first item

    u32 *up; //for accessing long paramenters on stack
    while (*cp){ //scan the format string 
        if(*cp != '%'){ //scann the format string
            putc(*cp); //spit out ordinary characters
            if( *cp=='\n') //for each '\n'
                {putc('\r');} // print a '\r'
                cp++; continue;
        }
        cp++; //print item by %FORMAT symbol
        switch (*cp){
            case 'c': putc(*ip); break;
            case 's': prints(*ip); break;
            case 'u': printu(*ip);break;
            case 'd': printd(*ip); break;
            case 'x': printx(*ip); break;
            case 'l': printl(*(u32 *) ip++); break;
            case 'X': printX(*(u32 *) ip++); break;
        }
        cp++; ip ++; //advance pointers

    }
}

/*
AFTER LOADING THE IMAGE THE BOOTER MUST SET THE BOOT PARAMETERS 
IN THE LOADED BOOT AND SETUP sectors for the inus kernel tp use.
*/




// this two functions allow the booter to access memory outside its owm segment
u8 get_byte(u16 segement, u16 offset)
{
    u8 byte;
    u16 ds = getds(); //getds() in assembly returns DS value
    byte = *(u8 *)offset;
    setds(ds);//setds() sin assembly restores DS 
    return byte; 
}

void put_byte(u8 byte, u16 segement , u16 offset)
{
    u16 ds = getds(); //save DS 
    setds(segement); //save DS to segment

    (*u8 *)offset = byte;
    setds(ds); //restore DS

}