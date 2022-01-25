main()
{
    printf("MTX starts in main()\n");
    lock();                         //CLI to mask out interrupts
    init();                         //initialize and create P0 as running
    set_vector(8, int80h);          //set syscall vector
    set_vector(8, tinh);            //install timer interrupt handler

    kfork("/bin/init");             //P0 kfork() P1 to execute /bin/init
    timer_init();                   //initialize and start timer

    while(1)
    {
        while(!readyQueue);             //global tick count
        tswitch();
    }
}


/*              Timer interrupt handler             */
int tick  = 0;              //gloabal tick count
int thandler()
{
        tick++; tick %=60;
        if(tick == 0){
            printf("1 second timer interrupt in ");
            running->inkmode > 1 ? putc('K') : putc('U'); printf("mode\n");

        }
        out_byte(0x20,0x20);        //issue EOI to 8259 PIC
}
/*
The timer above is a simple tier interrupt handler.
At each second, ot displays a line to show whether the timer interrupt occurred in Kmode or Umode.

Despite frequent timer interrupts, the reader may enter commands to run the system as usual.

The timer interrupt handler may be modified to perform other time dependent tasks
    1.display wall clocks
    2.Let a running process sleep for a few seconds and wakeup the process when sleeping time expires

*/