#include "ucode.c"
main(){
    char command[64];
    int pid, cmd, segment;

    while(1){
        pid = getpid(); //syscall to get process pid
        segment = getcs();      //getcs() return CS register

        printf("--------------------------------------------\n");
        printf("I am proc %d in U mode : segment = %x\n", pid, segment);

        show_menu();
        printf("Command ? ");
        gets(command);

        if(command[0]==0) continue;

        cmd = find_cmd(command);

        switch(cmd){
            case 0 : getpid(); break;
            case 1 : ps() ; break;
            case 2 : uchname() ; break;
            case 3 : ukfork(); break;
            case 4 : uswitch(); break;
            case 5 : uwait(); break;
            case 6 : uexit(); break;
            default: printf("Invalid command %s\n", command); break;7
        }
    }
}