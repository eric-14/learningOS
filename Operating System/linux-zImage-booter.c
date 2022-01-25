int setup ,ksectors, i ;
main(){
    prints("boot linus\n\r");
    setup = *(char *) (512+497);//number of setup sectors

    ksectors = *(int *)(512+500) >> 5; //number of kernel sectors
    setes(0x9000); //load BOOT+SETUP to 0x9000

    for(i=1; i<=setup+ksectors+2; i++) // 2 sectors before setup
    {
        getsector(i);       //load sector i 
        i<= setup ? putc('*') : putc('.'); //show * or .

        inces();        // inc ES by NSEC sector

        if(i==setup+1){setes(0x1000);} //load kernel to ES = 0X1000
    }
    prints('\n\ready to go??'); getc();
}