//syscall interface functions

int getpid() { return syscall(0,0,0,0);}
int ps() {return syscall(1,0,0,0); }
int chname(char *s) {return syscall(2,s,0,0); }

int kfork() {return syscall(3,0,0,0); }
int kswitch() {return syscall(4,0,0,0); }
int wait(int *status) {return syscall(5,status,0,0); }

int exit(int exitValue) { syscall(6,exitValue,0,0); }

/* Common code of Umode programs*/

char *cmd[] ={"getpid","ps","chname","kfork","switch","wait","exit",0};

int show_menu()
{
    printf("-------------Menu-------------\n");
    printf("- ps chname kfork switch wait exit -\n");
    printf("-------------------------------------\n");
}
int find_cmd(char *name)//convert cmd to an index
{
    int i =0; char *p = cmd[0];
    while(p){
        if(!strcmp(p,name)){return i;}
        i++;
        p = cmd[i];
    }
    return -1;
}











