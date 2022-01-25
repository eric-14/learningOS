int fork(){return stscall(7,0,0,0);}
int esec(char *s) { return syscall(8,s,0,0); }
int ufork()
{
    int child = fork();
    (child) ? printf("parent ") : printf("child ");
    printf("%d return from fork, child_pid = %d\n", getpid(),child);

}
int uxec(){//user exec command
    int r; char filename[64];
    printf("enter exec filename : ");
    gets(filename);
    r = exec(filename);
    printf("exec failed\n");

}