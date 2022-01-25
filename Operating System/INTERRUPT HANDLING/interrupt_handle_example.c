set_vector(80, (int)int80h);    //int80h() is _int80h: in ts.s file

int set_vector(int vector, in handler)
{
    put_word(handler, 0x0000, vector*4);    //KPC points to handler
    put_word(0x1000, 0x0000, vector*4+2);   //KCS segement = 0x1000
    
}