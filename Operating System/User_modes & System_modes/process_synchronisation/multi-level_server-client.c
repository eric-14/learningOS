#define NMBUF NPROC

MBUF mbuf[NPROC], *freeMbuflist;

SEMAPHORE mlock;

//utility functions

int emqueue(MBUF **queue, MBUF *p){}// enqueue p by priority }
PROC *demqueue(MBUF **queue){}//return first MBUF IN queue
int cpfu(char *src, char *dest){}//copy from uspace 
int cp2u(char *src, char *dest){} // copy to u space

int printmlist(MBUF *p){} //print message list

MBUF *get_mbuf(){}//return a free mbuf
int put_mbuf(MNUF *p){} //release a mbuf

MBUF *findClient(int pid){} //find a client mbuf in server replyList

int mbuf_init(){} // initialize freeMbuflist and semaphores

int send(char *msg, int pid){   //send msg to target pid
    MBUF *mp;
    PROC *p = &proc[pid];

    /* check not allowed to send cases */

    if(p->status == FREE || p->status == ZOMBIE){
        printf("target %d invalid\n", pid); return -1
    }
    if( running->level == p->level){
        printf("cant send to the same level\n");return -1;

    }
    if(running->S==1 && running->R ==0){
        printf("%d:(s,r)=(0,0), can't send\n",running->pid);return -1;
    }
     if(running->S==0 && running->R ==0){
        printf("%d:(s,r)=(1,0), can't send\n",running->pid);return -1;
    }
     if((running->S == running->R)&& running->S > 0 ){
        printf("%d:(s,r)=(%d,%d), can't send\n",running->pid);return -1;
    }

    // OK TO SEND
    
    if(running->S < running->R){// PROC.S < PROC.R
        if(running->level > proc[pid].level){
            printf("%d send reply \n", running->pid);
            if(( mp = findClient(pid)==0)){
                //find client's request
                printf("%d[%d] sending reply %s to %d[%d]\n",running->pid,runnig->level,mp->contents, p->pid,p->level);

            }else{
                printf("send request to server\n");
                if(! (mp=get_mbuf())){
                    printf("%d[%d] sendding reply %s to %d[%d]\n",running->pid,running->level,mp->contents,p->pid,p->level);
                    return -1;
                }
                mp->pid = running->pid;
                cpfu(msg, mp->contents);    //copy server's msg from U space
                running->S++;
            }
            //deliver mp to pid's mqueue
            P(&proc[pid].mlock);
                enmqueue(&proc[pid].mqueue, mp);
            V(&proc[pid].mlock);
            V(&proc[pid].nmsg);
            return 1;
        }
        printf("S > = R: can't send\n");
        return 0;

    }
    

}  


int recv(char *msg) //receive a mbuf from proc's own mqueue
{
    MBUF *mp;
    if(running->S == 0 && running->R > 0){
        printf("(s r) = (%d %d): can't recv()\n", running->S, running->R);
        return -1;
    }
    if(running->S > running->R){// this case should never happen
        printf("PANIC:(%d>%d)\n", running->S, running->R);
        return -1;
    }
   // OK to receive: running->S <= running->R

    P(&running->nmsg);
    P(&running->mlock);
        mp = demqueue(&running->mqueue);
    V(&running->mlock);

    printf("%d[%d] recv: from %d[%d] contents= %s\n", running->pid,running->level, mp->pid, pric[mp->pid].level, mp->contents);

    cp2u(mp->contents, mp);

    if(running->level < proc[mp->pid].level){ //reply from a server
        printf("received a reply\n");
        running->S--;
        put_mbuf(mp); //release mbuf

    }else{//received a client request
            printf("received a client request\n");
            //enter client request into replyList; do not free mbuf yet
            enmqueue(&running->replyList , mp);
            running->R++;

    }
    return 1;
}
int sendrecv(char *msg, int pid) // sendrecv: for level -1 clients only
{send(msg,pid);recv(msg);}

/*
Multi-level server-clent message passing is not intended as a genearal mechanism for IPC.
It is suitable for systems with a hierachial communication organization, such as microkernel base operating systems.

A micro-kernel is a kernel with only the essential functions of an operating system,
 such as process scheduling and message passing

All other functions such as process management, memory management , device drivers and files system
can be implemented as a server processes

In a micro-kernel user processes do not execute kernel functions directly.

They send request messages to various servers and wait for answers and results.
*/

