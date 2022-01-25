main()
{
    int pid = getpid();
    printf("I am proc %d in Umode: running segement = %x\n",pid,getcs());
    printf("proc %d sysycall to kernel to die\n",pid);
    exit(0);
}
int getpid(){   //assume : getpid() call # = 0
    return syscall(0,0,0,0);
}
int exit(int exitValue){    //assume : exit() call = #6
    syscall(6, exitValue, 0 ,0);
}