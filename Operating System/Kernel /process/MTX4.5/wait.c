int ksleep(int event) {};
int kwakeup(int event){};
int ready(PROC *p){p->status=READY;enqueue(&readyQueue,p);}


int kexit(int exitValue){
    if(running->pid == 1 && nproc >2){
        printf("other procs still exist, P1 can't die yet\n");
        return -1;
    }
    /* send children (dead or alive) to P1's orphanage     
        */
    for (i=1;i <NPROC; i++){
        p = &proc[i];
        if(p->status != FREE && p->ppid == running->pid){
            p->ppid = 1;
            p->parent = &proc[1];
            wakeupP1++;
        }
        running->exitCode = exitValue;
        running->status = ZOMBIE;

        //wake up parent and also P1 if necessary
        kwakeup(running->parent); //parent sleeps on its PROC address
        if(wakeupP1){
            kwakeup(&proc[1]);
        }
        tswitch();
    }

}

int kwait(int *status)
{
    PROC *p; int i, hasChild = 0

    while(1){
        for(i=1; i< NPROC; i++){
            p = &proc[i];
            if(p->status != FREE && p->ppid == running->pid){
                hasChild = 1; 
                if(p->status == ZOMBIE){    //lay the dead to rest
                    *status = p->exitCode;  //collect the exitCode
                    p->status = FREE;   //free its proc
                    put_proc(&freeList, p); //to freeList
                    nproc--;    //ONCE LESS PROCESSES
                    return(p->pid);     //return its pid

                }
            }
        }
        if(!hasChild) return -1; //no child return error
        ksleep(running);    //still has kids alive: sleep on PROC address
                            // on their way to becoming ZOMBIES
    }
}