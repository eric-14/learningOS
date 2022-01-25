
/*
Since sleep and wakeup may be called
with interrupts disabled, 
they shold restore CPU's original interupt mask rather than always enable interrups at the end

*/

sleep(int event)
{
    int_off;
    
    int_on;
}

wakeup(int event)
{
    int_off;

    int_on;
}


/*
A (counting) semaphore is a data structure
*/

struct semaphore{
    byte lock;          //represents a CR; initial value = 0 ;
    int value;          // initial value of sempahore
    PROC *queue;        // a FIFO queue of BLOCKed processes
}s;
/*
all operations on a sempahore must be performed in the same CR of the semaphore's lock
*/

enterCR(byte *lock){
    int_off;
    while(TS(*lock));
}

exitCR(byte *lock)
{
    *lock = 0;
    int_on;
}

P(struct semaphore *s)
{
    enterCR(&s->lock);
    s->value--;
    if(s->value < 0){BLOCK(s);}
    else { exitCR(&s->lock);}
}

V(struct sempahore *s)
{
    enterCR(&s->lock);
    s->value++;
    if(s->value => 0){SIGNAL(s);}

    exitCR(&s->lock);
}
/*
where BLOCK(s) blocks the process in the semaphore's FIFO queue, and SIGNAL(s)
unblocks the first process from the semaphore's FIFO queue

*/

BLOCK(struct semaphore *s)
{
    running->status = BLOCK;
    enqueue(&s->queue, running);

    s->lock = 0; //only for MP
    switch process;     //int on
}

SIGNAL(struct semphore *s)
{
    PROC *p;
    p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
}

/*
Any operations on a sempahore must be implemented as a CR.
*/
/*
On machines with atomic update instructions .eg. XADD, we may even assume that both --s and ++s are atomic, but this still does not proevent the race conditions

*/


//      SEMAPHORES OPERATIONS FOR MULTIPROCESSOR SYSTEMS 

/*
Semaphores operations are not linited to P and V. we may define additional sempahores operations to suit the needs, provided that they operate in the CR of the
Semaphore.
For multiprocessor sysytems , we define the following additional semaphore operations

1.Conditional_P (CP) and Conditional_V(CV) opeartions

*/

int CP(SEMAPHORE s){
    if(s>0){
        s--;
        return -1;
    }
    return 0;
}

int CV(SEMAPHORE s)
{
    if(s<0){
        s++;    SIGNAL(s);
        return 1;

    }
    return 0;
}

/*
CP(s) operates exactly as P(s) and returns 1 if s>0. Otherwise, it returns 0 without changing the sempahore value.

Likewise CV(s) operates exactly as V(s) and returns 1 if s<0 .

Otherwise it returns 0 without changing the sempahore value

CP can be used to avoid deadlocks in concurrent programs.

CV can be used to simulate wakeup in a concurrent process environment

PV() operation: in a multiprocessor system, it is often necessary for a process to release one lock and wait for another lock
using conventional P/V on semaphores may cause race conditions in the time gap after the process has done V on one sempahore but before it has completed P 
on another semaphore. In order to prevent race conditions, we ddefine a PV(s1,s2) operation as follows.

PV(SEMAPHORE s1,SEMAPHORE s2){atomically P(s1), V(s2) }

PV(s1,s2) allows a process to complete the blocking operation on semaphores s1 before releasing the sempahore s2 in a single indivisible operation

It eliminates the timegap between V(s2) and P(s1), thereby preventing any race condition.
PV(s1,s2) requires a procss to acquire the spinlock of s1 while holding the spinlock of s2

we shall ensure that it is used in such away that the locking order is always unidirectional, so that cross-locking can never occur

Although we may define any new sempahore operations we must ensure that the operations ar meaningful and their usage is correct.

For instance we may deifne a VALUE(s) operation, which returns the current value of a sempahore

            APPLICATIONS OF SEMAPHORES

1.Sempahore lock    -- lock CR for long durations. The reader may verify that only one process can be inside th CR at any time

2. Mutex Lock       -- is a lock sempahore withc an additional owner field, which identifies the current the current process that hodls the mutex lock.

if the mutex can only be unloacked by its owner. Unlocking a mutex wakes up a waiting process, if any, which becomes the new owner and the mutex remains locked

A major difference between mutex and semaphore is that processes must use mutex_lock() followed by mutex_unlock() in matched pairs,
 a process may do V on a semaphore 
without having done P on the semaphore 

Thus, mutexes are strictly for locking, sempahores can be used for both locking and process cooperation

        RESOURCE MANAGEMENT

as long as s>0 a process can suceed in P(s) and get a resource. when all resources are in use requesting proecesses will be blocked at P(s).

when a resource is released by V(s) , a blocked process, if any, will be allowed to continue to use a resource. At any time the following invariants hold.

    s >= 0 : s = the number of resources still available
    s < 0 : |s| = number of processes waiting in a queue

        WAIT FOR INTERRUPTS AND MESSAGES

a semaphore with initial value =0  is often used to convert an external event
to unblock a process that is waiting for the event

        PROCESS COOPERATION

A set of consumer processes share a finite number of buffers.
Each buffer contains a uniqu item at a time

A producer must wait if there are no empty buffers 
similarly a consumer must wait if there are no full buffers

*/

DATA buf[N];  //N buffer cells
int head=tail = 0;  //index to buffer cells

SEMAPHORE empty = N; full = 0;pmutex = 1; cmutex=1;

//  PRODUCER

while(1)
{
    produce an item;
    P(empty);
     P(mutex);
     buf[head++] = item;
     head%= N;
    V(pmutex);
    v(full);

}

//  CONSUMER

while(1){
    p(full);
     p(cmutex);
     item = buf[tail++];
     tail %= N;
     V(cmutex);
     V(empty);
}

//  READER -WRITER PROBLEM

SEMAPHORE rwsem = 1, wsem = 1, rsem = 1;

int nreader = 0 //Number of activer reader

ReaderProcess{
    while(1){
        P(rwsem);
        P(rsem);
            nreader++;
            if(nreader ==1){ P(wsem); }
        
        V(rsem);
        V(rwsem);
    }
}

WriterProcess{
    while(1){
        P(rwsem);
        P(wsem);
        //Write data
        V(wsem);
        V(rwsem);
    }
}
/*
Other processes of synchronization
1. Event FLags 
    OpenVMS provodes service functions for processes to manipulate their associated event flags

    set_event(b) ;;; set b to 1 and wakeup waiter(b) if any

    clear_event(b) ;;;;clear b to 0

    test_event(b)   ;;;;;test b for 0 or 1

    wait_event(b)       ;;;;wait until b is set;

    Unix events are only available in kernel mode

2. Event Variables in MVS
3.ENQ/DEQ in IBM MVS





*/