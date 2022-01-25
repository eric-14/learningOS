#define SSIZE 1024   //2KB stack size per process
typedef struct proc{
    struct proc *next;
    int *ksp;       //saved sp when PROC is not running 
    int kstack[SSIZE];      //process kernel mode stack
}PROC;

int procSize = sizeof(PROC);
PROC proc0 , *running;        //proc0 structure and running pointer
int scheduler(){running = &proc0;}
main(){
    running = &proc0;
    printf("call tswitch ()\n");
            tswitch();
    printf("back to main ()\n");

}
/*
------->it lets running point to proc0,,,the system is now running the process proc0

------->it executes tswitch() which saves the return address rPC in stack
------->it executes the SAVE part of tswitch(), which saves CPU register inot stack and saves the stack pointer
        sp inyo proc0.ksp
------->it executes the save part of tswitch() which saves CPU register inot stack and saves the stack pointer sp into proc0.ksp

-------> it calls scheduler(), which sets running point at proc0 again. For now, this is redundant since running already points at proc0
        Then it executes the RESUME part of tswitch()
--------> it sets sp to proc0.ksp which is again redundant since they are already the same.Then it pops the stack which restores the saved CPU register 
-------->calls ret of RESUME which return to the calling place of tswitch()

*/
