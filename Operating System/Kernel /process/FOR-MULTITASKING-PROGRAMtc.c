//

#define NPROC 9
#define SSIZE 1024

typedef struct proc{
    struct proc *next;
    int *ksp;
    int status;     // FREE | READY | STOP | DEAD
    int priority;      // proc scheduling priority
    int ppid;           //parent id
    int pid;            //add pid as proc's ID
    int kstack[SSIZE];  //proc stack area
}PROC;

PROC proc[NPROC] , *running; //define NPROC proc structure

int procSize = sizeof(PROC); //PROC size, needed in assembly code

int body(){
    char c;
    int pid = running->pid;
    printf("proc %d resumes to body()\n",pid);

    while(1){
        printf("proc %d running, enter a key:\n", pid); c=getc();
        tswitch();
    }
}

int init(){         //initialize PROC structures
        PROC *p; 
        int i, j;
        for(i=0; i<NPROC;i++)//initialise all processes
        {
            p = &proc[i];
            p->pid = i; //pid = 0,1,2,3,....NPROC-1
            p->next = &proc[i+1];       //point to the next PROC
            if(i){              //for PROCs other than P0
                p->kstack[SSIZE-1]=(int) body;       //entry address of body()
                for(j=2;j<10;j++){      //all saved registers = 0
                        p->kstack[SSIZE-j] = 0;
                }
                p->ksp = &(p->kstack[SSIZE-9]); //the address of bottom of the stack  the sp saves sp in PROC.ksp


            }


        proc[NPROC-1].next = &proc[0];  //the last proc points to the first proc all PROCS form a circular list
        running = &proc[0]; //PO is running
        printf("init complete\n");

}
int scheduler()
{
    running = running->next; //scheduler() function 
}
main()
{
    init();
    while(1){
        printf("PO running\n");
        tswitch();
    }
}
