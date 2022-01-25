int ukfork() { kfork(); }
int uswitch() { kswitch(); }

int uchname()
{
    char s[32];
    printf("Input new name : ");
    chname(gets(s));//assume gets() return pointer to string 

}

int uwait()
{
    int child, status;
    child = wait(&status);

    printf("proc %d, dead child = %d \n",getpid(), child);

    if(child >= 0){     //only if has child
        printf("status = %d\n",status);
    }
}

int uexit()
{
    char s[16]; int exitValue;
    printf("enter exitValue : ");
    exitValue = atoi(gets(s));

    exit(exitValue);
}