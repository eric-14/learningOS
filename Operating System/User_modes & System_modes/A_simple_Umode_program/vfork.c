/*
Wen the child runs , it changes Umode image to a new fukr. In this cse copying image in fork() would be a waste
since the child process abandons the copied image immediately.

Vfork is similar to fork but does not copy the parents's Image

A child is created to share the same Image with the parent

we use memory protection schemes to avoid modification of shared memoey

            Algorithm

1. kfork(0) a child process ready to run in Kmode, return -1 if fails;
2.copy a selection of parent's ustack from parents.usp all the way back to where it call pid= vfork()
3.Let child PROC.USS = PARENT PROC.uss, so that they share the same segment, ste child PROC.usp = parent PROC.usp-1024, change ax in child'd goUmode frame to 0 for it to return 0 to pid = vfork()

4. return child pid


When the child returns to Umode it runs in the same image as the parent. Then it issues an exec syscall to change image.
when it enters kexec(), the Umode segment is still that if the parent. 
The child can fetch the cimmanf line and then exec to its own segment.

To support vfork(), kexec() only need to change the caller's current segment, it loads the new image to the caller's default segment(by pid), thereby detaching it form the parent image

*/
int vfork()
{
    int pid, i , w; u16 segment;
    PROC *p = kfork(0); //kfork() child do not load image file

    if(p==0){return -1;} //kfork() failed
    p->vforked =1 ;// set vforked flag, use in kexec()

    //copy a section of parent ustack for child to return to Umode

    for(i=0;i<24:i++){
        w = get_word(running->uss, running->usp+i*2);
        put_word(w, running->uss, running->usp-1024+i*24);
    }
    p->uss = running->uss;
    p->usp = running->usp - 1024;
    put_word(0, running->uss, p->usp+8*2); //set child's return value to 0
    p->kstack[SSIZE-1] = goUmode;       //CHILD GOUMODE DIRECTLY

    return p->pid;
}

/*
MTX5.3 demonstrates vfork in MTX. To do this, we add a uvfork syscall interface and a user mode vfork command program.


*/

int uvfork(){return syscall(9,0,0,0); }//vfork() system call

int vfork(int argc, char *argv[ ])
{
    int pid, status;
    pid =  uvfork();        //call uvfork() to vfork a child

    if(pid){
        printf("vfork parent %d waits \n",getpid());
        pid = wait(&status);
        printf("parent %d waited, dead child= %d\n",getpid(),pid)
    }else
    {
        printf("vforked child %d in segement %x\n", getpid(), pid);
        printf("This is Goldilocks playing in papa bear's bed\n");
        printf("EXEC NOW! before he wakes up\n");
        exec("u2 Bye Bye! Papa Bear");
        printf("exec failed! Goldilocks in depp trouble\n");
        exit(1);            //better die with dignity than mauled by a big bear
        
    }
    
}