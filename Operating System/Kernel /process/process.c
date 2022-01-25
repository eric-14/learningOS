typedef struct proc{
    struct proc *next; //pointer to the next proc structure maintain PROC in dynamic data structiree such as link lists and queues
    int *ksp; //sved stack pointer of a process when it is nor executing 
    int kstack[1024]; //kstack is the execution stack process
}PROC;