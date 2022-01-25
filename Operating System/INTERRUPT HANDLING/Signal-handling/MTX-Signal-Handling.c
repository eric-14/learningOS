
#define NSIG 16
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGBUS 7 
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12 
#define SIGPIPE 13
#define SIGALARM 14
#define SIGTERM 15

/*
            ALGORITHM OF KILL SYSYCALL

int kkill(int pid, int sig_number)
{
    1. validate signal number and pid 
    2. check permission to kill;  //not enforced may kill any pid
    3.set proc.signal[bit_sig_number] to 1
    4.if proc is SLEEP, wakeup pid;
    5. if proc is BLOCKED for terminal inputs, unblock proc;
    6. return 0 for success  
}

            CHANGING SIGNAL HANDLER

int ksignal(u16 sig_number, u16 catcher)
{
    1.validate sig number, e.g. cannot change signal number 9;
    2. int oldsig = runnning->sig[sig_number]
    3. running->sig[sig_number]  = catcher;
    4. return oldsig;
}

*/

//          ALGORITHM OF CHECK SIGNALS
int check_sig()
{
    int i;
    for(i=1;i<NSIG;i++){
        if(running->signal & (1 << i)){ //checks whether the SIG exists
            running->signal &= ~(1 << i); //it only clears the identified SIG that exists at that moment and deletes those that dont
            return i;
        }
    }
    return 0;
}

/*
            HANDLING EXISTING OUTSTANDING SIGNALS BY THE CODE SEGMENT

Algorithm psig()

int psig()
{
    while(int n=check_sig()){ //for eacg pending signal do

        1. clear running PROC.signal[bit_n];    //clear the signal bit
        2. if (running->sig[n]==1)                //clear the signal bit
                    continue;
        3. if(running-sig[n]  == 0 )    //  DEFalut : die with sign#
                kexit(n<<8);            //high byte of exitStatus= signal number
        4. //execute signal handler in Umode

            fix up running PROC's "interrupt stack frame" fro it ro return to execute catcher(n) in Umode;

        

    }
}

*/

/*
        MODIFYING THE STACK TO PERFORM CATCHER IN UMODE

In order fir the process to return catcher() with the signal number as a parameter, we modify the interrupt stack frame

    1.replace the uPC with the entry address if catcher()
    2Insert the original return address, uPC,  and the signal number after the interrupt stack frame


*/

//          TIMER AND ALARM SIGNAL CATCHER
void catcher()
{
    printf("proc %d in a catcher: sig=%d\n", getpid(), sig);
    itimer(1);          //set a 1 second itimer again
}

main(int argc, char *argv[])
{
    int t =1;
    if(argc>1) t = atoi(argv[1]);       //timer interval
    p rintf("install catcher? [y|n]");

    if(getc()=='y'){
        signal(14, catcher);        //install catcher() for SIGALRM(14)
    }
    itimer(t);                      //set interval timer in kernel
    printf("proc %d looping until SIGALRM\n", getpid());
    while(1);                       //looping until killed by a signal
    
}

