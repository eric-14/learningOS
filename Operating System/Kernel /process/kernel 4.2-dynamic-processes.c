





//🧧🧧🧧🧧🧧🧧🧧🧧🧧🧧🧧🧧🧧as of now we assume that mtx runs in a uniprocessor and this implemenation has some serious flaws but it works for us 😀








/*st
PROC *get_proc(PROC **list): return a free proc pointer from list
int put_proc(PROC **list,PROC *p) :e neter p into a list
int enqueue(PORC **queue,PROC *p) : eneter p into queue by priority
PROC *dequeue(PROC **queue) : return first element removed from queue
printList(char *name, PRoc *list): print name=list contents
*/

#define NPROC 9
#define SSIZE 1024

//PROC status 

#define FREE 0
#define READY 1
#define STOP 2
#define DEAD 3

#define SLEEP 4


typedef struct proc{
    struct proc *next;
    int *ksp;
    int pid;
    int ppid;
    int status; //FREE|ready |stopped|dead
    int priority;   //scheduling priority
    int event;
    int kstack[SSIZE];
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue;
int procSize = sizeof(PROC);

//#INCLUDE "io.c" //include I/O functions based on getc()/putc()
//#include "queue.c" //implement your own queue functions

int body(){
    char c;
    printf("proc %d starts from body()\n",running->pid);
    while(1){
        printList("free List",freeList);
        printList("readQueue", readyQueue);
        printf("proc %d running: parent=%d\n", running->pid, running->ppid);

        c= getc(); printf("%c\n",c);
        switch(c){
            case 'f' : do_kfork(); break;
            case 's' : do_tswitch();break;
        }    }

}
PROC *kfork()//create a child process, begin from body()
{
    int i;
    PROC *p = get_proc(&freeList);
    if(!p){
        printf("no more PROC , kfork() failed\n");
        return 0;
    }
    p->status = READY;
    p->priority =1; // for all processes except P0
    p->ppid = running->pid; //parent = running

    /*initialize new proc's kstack[ ] */
    for(i=1;i<10;i++) 
    {p->kstack[SSIZE-i] = 0;}  //saving CPU REGISTERS

    p->kstack[SSIZE-1] = (int)body; //resume point= address of body()
    p->ksp = &p->kstack[SSIZE-9];   // second item in the stack proc saved sp

    enqueue(&readyQueue, p );   //eneter p into readyQueue by priority
    return p;
}

int init()
{
    PROC *p; int i;
    printf("init ....\n");
    for (i=0; i <NPROC; i++){ //initialize all process
         p = &proc[i];
         p->pid = i;
         p->status = FREE;
         p->priority = 0;
         p->next = &proc[i+1];

    }
    proc[NPROC-1].next = 0; // we have made the processes to be circular linked list
    freeList = &proc[0]; // all procs are in freeList
    readyQueue = 0;

    /**create P) as running**/
    p = get_proc(&freeList); //allocate a PROC from a freeList
    p->ppid =0;
    p->status = READY;
    running = p;
}


// I think in the function dequeue we should change status to 
// 
int scheduler()
{
    if(running->status == READY){enqueue(&readyQueue,running);}//enter it into readyQueue

    running = dequeue(&readyQueue);
}
main()
{
    printf("MTX starts in main()\n");
    init();
    kfork();
    while(1){
        if(readyQueue){tswitch();}
    }
}
/*** kernel command functions  ***/
int do_fork()
{
    PROC *p = kfork();
    if(p == 0) {printf("kfork failed\n"); return -1;}
    printf("PROC %d kfork a child %d\n",running->pid,p->pid);
    return p->pid;
}
int do_switch(){ tswitch(); }

/*
Implement do_exit function to change the status of the process and
call tswitch() for it to give uo the CPU
*/
/*
similarly which stops the current running process and 'c' command which lets a stopped process continue 


do_continue(){
    ask for a pid validate the pid 0< pid< NPROC;
    rind the proc by pid ; if proc.status is stop change the status to ready then push it to the readyqueue
}
*/
/*
        SLEEP AND AWAKE

        sleep(int event){
            record event value in running PROC.event;
            change running PROC.status to SLEEP;
            switch process;
        }

        wakeup(int event)
        {
            for every proc in the PROC array do{
                if(proc.status ===SLEEP && proc.event ==event){
                    change proc.status to READY 
                    enter proc into readyQueue
                }
            }
        }


---> when process must wait for something e,g a resource, identified by an event value it calls slepp(event) to go to sleep
---->An event is any value a process may sleep on as long as an another event willi ssue a wake up call on the event value
    it is up to the system designer to associate each resourec with a unoque evant value.
     for example when a process is waits for a child process termination it usually sleeps on its own on the specified PROC address which is uniqu and also known to uts children 
     when a process terminates it issues wake up (&parentPROC) to wake up its parent

-----> many processes may sleep on the same event which is natural beacuse many processes may need the same resource that is cuurentlu busy or unnavailable

-----> when an event occurs, someone will call wakeup event which wakes up all the processes sleeping on that event.if no process is sleeping on that event wakeup has no effect
    when an awekened process runs it musy try to get the resource again since the resource may already be obtained by another process

------> in a multiprocessor the sleep_first_wake_up later order cannot be guaranteed because process may run in parallel in real-time.
        in muliprocessor we need anothe process synchronization schemes e.g semaphores

-------> a process as it goes to sleep it is given a priority in the impartance the resource the process is waiting for.
            if awakened process has a higher priority than the current running process, process swith does not take place immediately . 
            it is deferred until the current running process is about to exit the kernel mode to return to user mode 


--------> The assigned priority classifies a sleeping process as either SOUND sleepr or a LIGHT sleeper. A sound sleeper can only be woken up by its awaited event.
        A sound sleeper can only be woken up by its awaited event. A light sleeper can be woken up by other means, which may not be event it is waiting for.
        For example, when a process waits for disk I/O, it sleeps with a high priority and should not be woken up signals. if it waits for an input key from a terminal which may not come for a long time.
        If it waits for an input key from a terminal, which may not come for a long time , it sleeps with a low priority and can be woken up by signals.


*/

// 
/*                     IMPLEMENTING SLEEP AND WAKE UP 
------>Modify the sleep function to enter the sleeping process into the sleepList.Modify the wake up sleeping processes in order

------> In the body() function, display sleeping processes and the events they are sleeping on

------>Instead of waking up all processes sleeping an event, wake up only one such process and discuss its implications

*/




int sleep(int event){
    running->event = event; //record event in the PROC.event
    running->status = SLEEP; // change status to sleep
    tswitch();
}

int wakeup(int event)
{
    int i; PROC *p;
    for(i=1;i<NPROC;i++)
    {
        p = &proc[i];
        if(p->status==SLEEP &&& p->event == event){
            p->event = 0; //cancel PROC'S event
            p->status = READY; //make it ready to run again
            enqueue(&readyQueue,p);
        }

    }
}
/*






