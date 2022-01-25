/*
Therefore a message passing must go through the kernel.

The contents of a message can be designed to suit the needs ofthe communicating processes

*/

typedef struct mbuf{
    struct mbuf *next;      //pointer to the next buf
    int pid;                 //sender pid
    int priority;           //message priority 
    char contents[128];     //message contents
}MBUF;

MBUF mbuf[NMBUF];           //NMBUF =  number of mbufs


/*
Initially, all message buffer are in a free mbuflist.
To send a message, a process must get a free mbuf first.

After receiving a message, it releases the mbuf as free.
Since mbuf is accessed by many processes it is a CR and must be protected.
*/
 MBUF *get_mbuf()
 {
     P(mlock);
     MBUF *mp = dequeue(mbufList);
     V(mlock);
     return mp;
 }
 int put_mbuf(MBUF *mp)
 {
     P(mlock);
        enqueue(mbufList);
     V(mlock);
 }
 /*
 Instead if a centralised message queue we assume that each PROC has a provate message queue which contains mbufs delivered to, but not yet received by, the process.
 Initially every PROCs mqueue is empty

 the mqueue if each process is also a CR because it is acessed by all the sender processes as well as the process itself


 Asynchronous communication is intended mainly for loosely-coupled systems, in which interprocess communication is infrequent
 Processes do not exchange messages on a planned regular basis

 */

int a_send(char *msg,int pid)       //send message to target pid
{
    MBUF *mp;
    //validate target pid .e.g. proc[pid] must be a valid process

    if(!(mp = get_mbuf()))      // try to get a free mbuf 
    {
        return -1;      //return -1 if no mbuf
    }
    mp->pid = running->pid;     //running pid is the sender
    mp->priority =1 ;       //assume same priority for all message
    cpfu(msg, mp->contents);        //copy message from Umode space to mbuf

    //deliver mbuf to target proc's message queue

    P(proc[pid].mlock);         //enter CR
        //enter mp into PROC[pid].mqueue by priority
    V(proc[pid].lock);  //exit CR
    V(proc[pid].message);   //V the target proc's message semaphore
    return 1;               // return 1 for SUCCESS

}
int a_recv(char *msg)       //receive a msg from proc's own mqueue
{
    MBUF *mp;
    P(running->mlock);      //enter CR
    if(running->mqueue==0)  // check proc's mqueue
    {
        V(running->mlock);  //release CR lock
        return -1;
    }
    mp =dequeue(running->mqueue);   //remove first mbuf from mqueue
    V(running->mlock);          //release mlock
    cp2u(mp->contents,msg);     //copy contents to msg in Umode
    return 1;5
}


//          SYNCHRONOUS MESSAGE PASSING
/*
A Sending process must wait if there are nof ree mbuf vice versa applies to receiver

It is suitable for tightly coupled systems in which message exchange is in a planned regular basis

To support message passing, we define additional sempahores for process synchronization and redesign the send-receive algorithm


*/
SEMAPHORE nmbuf = NMBUF;        //number of free mbufs
SEMAPHORE PROC.nmsg = 0;        //for proc to wait for messages

MBUF *get_mbuf()        //return a free mbuf pointer
{
    P(nmbuf);            //wait for free mbuf
    P(mlock);
        MBUF *mp = dequeue(mbufList);
    V(mlock);
    return mp;

}
int put_mbuf(MBUF *mp)    //free a used mbuf to freembuflist
{
    P(mlock);
    enqueue(mbufList,mp);
    V(mlock);
    V(nmbuf);
}
int s_send(char *msg, int pid)       // sychronous receive from proc's own mqueue
{
    //validate target pid, e.g. proc[pid] must be a valid processes
    MBUF *mp = get_mbuf();      //BLOCKING: return mp must be valid
    mp->pid = running->pid;     // running proc is the sender
    cpfu(msg, mp->contents);        //copy msg from Umode space to mbuf
    // deliver msg to target proc's mqueue

    P(proc[pid.mlock]);         //enter CR
        enqueue(proc[pid].mqueue, mp);
    
    V(proc[pid].lock); //exit CR
    V(proc[pid].nmsg);      //V the target proc's nmsg sempahore
}

int s_recv(char *msg)       //synchronous receive from proc's own queue
{
    P(running->nmsg);       // wait for message
    P(running->mlock);         //lock PROC.mqueue
        MBUF *mp = dequeue(running->mqueue);    //get a message
    V(running->mlock);      //release mlock
    cp2u(mp->contents,msg);     //copy contents to Umode
    put_mbuf(mp);               // free mbuf 
}




