int do_tswitch(){}
int do_kfork(){}
int do_exit(){}
int do_stop(){}
int do_continue(){}
int do_sleep(){}
int do_wakeup(){}

int reschedule()
{
    PROC *p, *tempQ = 0;
    while ((p=dequeue(&readyQueue))){ //reorder readyQueue
        enqueue(&tempQ, p);
    }
    readyQueue = tempQ;
    rflag = 0;      //Globale reschedule flag
    if(running->priority < readyQueue->priority) rflag = 1;
}
int chpriority(int pid, int pri){
    PROC *p; 
    int i, ok= 0, reQ=0;

    if(pid == running->pid){
        running->priority = pri;
        if(pri < readyQueue->priority){
            rflag = 1;
        }return 1;
    }
    // if not for running, for both READY and SLEEP procs

    for(i=1;i<NPROC;i++){
        p = &proc[i];
        if(p->pid == pid && p->status != FREE){
            p->priority = pri;
            ok = 1;
            if(p->status == READY) //in readyQueue ===> redo readyQueue
            { reQ=1; }
        }
    }
    if (!ok){
        printf("chpriority failed\n");
        return -1;
    }
    if (reQ){
        reschedule(p);
    }
}


int do_priority(){
    int pid, pri;
    printf("input pid");
    pid = geti();
    printf("input new priority ");
    pri = geti();
    if(pri<1){pri =1;}
    chpriority(pid, pri);
}


int body(){
    char c;
    while(1){
        if(rflag){
            printf("proc %d: reschedule\n", running->pid);
            rflag = 0;
            tswitch();
        }
        printList("freeLsist ",freeList);       //show freeList
        printQ("readyQueue",readyQueue);    //show readyQueue
        printf("proc %d running: priority = %d parent = %id enter a char [s|f|t|c|z|a|p|w|q]:",
            running->pid, running->priority, running->parent->pid);
        c=getc(); printf("%c\n",c);

        switch(c){
            case 's': do_tswitch(); break;
            case 'f': do_kfork();   break;
            case 'q': do_exit();    break;
            case 't': do_stop();    break;
            case 'c': do_continue();    break;
            case 'z': do_sleep(); break;
            case 'a': do_wakeup(); break;
            case 'p': do_chpriority(); break;
            case 'w': do_wait();    break;
            default: printf("invalid command\n"); break;
        }
    }
}





