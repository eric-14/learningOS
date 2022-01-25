#define NPROC 9
#define SSIZE 1024
#define FREE 0
#define READY 1
#define RUNNING 2
#define STOPPED 3
#define SLEEP 4
#define ZOMBIE 5

typedef struct proc{
    struct proc *next;
    int *ksp;
    int pid;
    int status;     //FREE| READY | RUNNING | SLEEP | ZOMBIE
    int ppid;
    struct proc *parent;    //pointer to parent PROC
    int priority; 
    int event;
    int exitCode;           //exitCode
    int kstack[SSIZE];
}PROC;

