/*
1.get filename from Umode space;
2.load filename to running proc's segment; return -1 if fails;
3. initialize proc's ustack for it to execute from VA = 0 in Umode



*/

int kexec(char *y)  //y points at a filename in Umode space
{
    int i, length = 0;
    char filename[64], *cp = filename;

    u16 segment = running->uss;         //same segment

    // get filename from U space with a length limit of 64

    while( (*cp++ = get_byte(running->uss, y++))  && length++ < 64);

    if(!load(filename, segment));  //load filename to segment
            return -1;          // if load failed , return -1 to Umode

    //re-initalize process ustack for it return to VA=0

    for(i=1; i <=12;i++){ put_word(0, segment , -2*i); }

    running->usp = -24;

    put_word(segment , segment, -2*12); //saved uDS= segment
    put_word(segment, segment, -2*11);  //saved uES=segment
    put_word(segment, segment, -2*2);   //uCS=segment; uPC=0
    put_word(0x0200, segment, -2*1);    //Umode flag= 0x0200
}