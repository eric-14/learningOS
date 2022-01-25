/*
ADVANTAGES OF THREADS

1. thread creation and switching are faster 
2. Threads are more responsive 
3. Threads are better suited to parallel computing
*/
#define NPROC 9
#define NTHREAD 16

PROC proc[NPROC+NTHREAD];


/*
Free thread PROCs are maintained in a separate tfreelist

The main thread of a process cannot die if there are other threads still exiting in the process

From a thread PROC we can always find the main thread PROC by following the parent PROC pointers
Therefore, there is no need to actually implement the thread family-tree in a process.

In the main thread PROC, tcount records the total number of threads in the process.

in every PROC the resourceptr points to a resource structure containing the per-process resources, such as uid, opened file desciptors 
The per-proces resource is allocated once only in the main thread.

All other threads in a process point to the same resource.
            
            THREAD CREATION

int kthread(int fn, int *stack, int flag, int *ptr)
{ create a thread, where
    fn = function entry address to be executed by the thread
    stack = high end addres of a ustack area
    flag = thread options; 0 or  1 to determine thread's ustack
    ptr = pointer to a list of parameters for fn (ptr)

} 

kthread creates a thread to execute the functions fn(ptr) with a ustack at the high end of the stack area

flag parameter allows the thread to have many options .e.g. keep newly opened file descriptors private

The ustack of each thread is a section of the total ustack area determined by its pid.

This simplifies the ustack area management when threads are created and terminated dynamically.

    Algorithm of thread creation in MTX

1. if process tcount > TMAX return -1 for error
2. allocate a free THREAD PROC and initialize it ready to run as before,i.e. initialize its kstack[ ] to resume to goUmode() directly
3.determine its ustack high end, initialize the ustack for it return to execute fn(ptr) in the umode image of the caller
. enter thread PROC inot readyQueue, increment process t_count by 1
5. return thread pid;


------->when a process does fork or vfork it creates a new process woth only the main thread, regardless if how many threads are in the caller's process
------->A thread is allowed to exec if and only if the total number of threads in the process is 1.
        this is clearly necessary because exec changes the entire execution image of a process. If a thread changes the process image, other threads cannot survive.
        So we modify kexec() in the MTC kernel to enforce this rule, which effectively says that only one main thread woth no child threads may change the image.

*/

/*
                    THREAD TERMINATION

    the main thread of a process cannot die if there are other threads still existing in the process address space would be freed.
    a dying thread should not send its orphans to P1 but to the main thread.


*/





typedef struct proc{
    int type;       //PROCESS|THREAD
    int tcount;     //total number of threads in PROC
    struct proc *tlist;         // alist of all threads in PROC
    struct resource *resourcePtr;       //pointer to process "resource"
    int kstack[SSIZE];          //KSTACK[] must be the last entry
}PROC;


// In order for processes with threads to terminate properly , we modify kexit() in the MTX kernel 
/*
kexit(int exitValue)
{
    if(caller is a THREAD){
        dec process t_count by 1
        send all children, if any to main thread process

    }else{
        caller is a process
    
    while(process t_count > 1){wait(&status);} 
    wait for all other threads to die 
    caller is a process with only the main thread 

    send all children to P1;
    } 
    record exitValue in PROC;
    become a ZOMBIE
    wakeup parent;
    if(has sent children to P1) wakeup P1
    tswitch();
}
*/

/*
Thread join suspends a thread until the designated thread terminates 
a thread may join with its child or children 

join(int n) wait for n children threads to terminate
{
    int pid,status;
    while(n--) pid = wait(&status)
}
*/

/*
            THREADS SYNCHRONIZATION
    
    since threads execute in the same address space of a process, they share all the global variables in a process
    when several threads try to modify the same shared variable or data structure, if the outcome depends on the execution order of the threads it 
    is called a race condition.

    In concurrent programming, race conditions must nit exist.
    Otherwise the results would be inconsistent

    In order to prevent race conditions among threads we implement mutex and mutex operations in MTX


*/


struct mutex{
    int status;         //FREE| inUSE | LOCKED | UNLOCKED | etc
    int owner;          //owner pid, only owner can unlock
    PROC *queue;        //waiting proc queue
}mutex[NMUTEX];         //all initialized as FREE

// MUTEX OPERATIONS

struct mutex *m = mutex_create();   //create a mutex, return pointer
mutex_lock(m)   : lock mutex m;
mutex_unlock(m) : unlock mutex
mutex_destroy(m)    : destroy mutex

/*
Threads use mutex as locks to protect shared data objects.

a newly created mutex is in an unlocked state and without an owner

when a thread executes mutex_lock(m) it locks the mutex, becomes the owner and continues.
Otherwise, it blocks and waiys in the mutex(FIFO) queue

Only the thread which has acquires the mutex lock can access the shared data object.

When the thread finishes with the shared data object it calls mutex_unlock(m) to unlock the mutex 

A locked mutex can only be unlocked by the current owner

when unloacking a mutex, if there are no waiting threads in the mutex queue it unlocks the mutex and the nutex has no owner 

Otherwise it unblocks a witing thread which becomes the new owner and the mtex remains locked

when all threads are finished the mutex may be destroyed for possible reuse

    THREAD SCHEDULING






*/