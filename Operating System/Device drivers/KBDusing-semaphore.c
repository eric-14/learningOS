stuct kbd{
    char buf[KBLEN];
    int head, tail;
    SEMAPHORE data;         //semaphore for data synchronization
}kbd;

int kbd_init()
{
    kbd.head = kbd.tail = 0;
    kbd.data.value = kbd.data.queue = 0;    //initialize sempahore to 0
}

int kbhandler()
{
    V(&kbd.data);
}
int getc()
{
    u8 c;
    P(&kbd.data);   //wait for input key if necessary
    lock();
    c= kbd.buf[kbd.tail++];
    unlock();
    return c;
}