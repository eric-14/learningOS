/*
kexit(int exitValue){
  1. erase process user-mode context, e.g. close file descriptors, release resources,
  deallocate usermode image memory, etc. 
  2. dispose of children processes, if any.
  3. record exitValue in PROC.exitcode for parent to get.
  4. become a Zombie (but do not free the PROC)
  5. wakeup parent and, if needed, also the INIT process p1  
}
*/

//In Unix system it does implent the process family tree as a process termination management scheme
//The process tree allows to have orphans and Parent-child relation to be loosely linked (in their execution environment) such that the ac hild can exist without a parent

/*
In the dying of a process the process becomaes a ZOMBIE but does not free up PROC

then the process calls kwakeup(event) to wakes up its parent, where the event must be the same unique value used by both the parent and child processes

The final act of dying process is to call tswitch() for the last time.
After this the process is essentially dead but has a dead body in the form of ZOMBIE PROC,which will be buried by the parent process through the wait operation


In PROC there is an exitCode ---> the lower bits of the exit code show the termination value while the higher bits show the interrupt that caused the termination
*/

/*
At any time, a process may call the kernel function kwait()

to wait a ZOMBIE  child process, If successful, the returned pid is the ZOMBIE
child's pid and staus contains the exitCode of the ZOMBIE child. In addition, kwait() also releases the ZOMBIE PROC back to the freelist. 
*/

int kwait(int *status)
{
    if(caller has no child){return -1 for error;}

    while(1)//caller has children
    {
        //search for a (any) ZOMBIE child;
        if(found a ZOMBIE child){
            get ZOMBIE child pid;
            copy ZOMBIE child exitcode to *status;
            bury the ZOMBIE child(put its PROC back to freeList)
            return ZOMBIE child pid;
        }
        /*  has children but none dead yet    */
        ksleep(running); //sleep on it PROC address
    }
}

//when a procss terminates it must issue a kwakeup(parent)
//to wake up the parent, it will find a dead child

//kwait() --> only handles one ZONBIE at a time

/*
In Unix the INT process P1 wears may hats:  
        ancestor of all processes except P0. In particular it is the grand daddy of all user processes since all login processes are children of P1

        it is the head of the orphanage

        it is the undertaker -- keeps looking for ZOMBIES to bury their dead bodies

        so in unix if the P1 dies or gets stuck the systen would stop functioning because no user can login agin the system and will soon be full of rotten corpses

*/