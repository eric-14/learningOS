#include "ucode.c"
int in , out , err;
main(int argc , char * argv[1]){
    1. close file descriptors 0,1 inherited from INIT 
    2. open argv[1] 3  times as in(0), out(1), err(2)
    3.setty(argv[1]);   // set tty name string in PROC.tty
    4. open /etc/ passwd file for READ \

            while(1){
                printf("login:");
                get username 
                 printf("pasword:"); 
                 get user password
                 for each lin in passwd file do{
                     tokenize account line;

                     if(user has a valid account){
                         change uid, gid to users uid, gid; //chuid()
                         change cwd to users home DIR       //CHDIR()
                        
                        change opened /etc/passwd file      //close
                        exec to program in usr acount       //exec()

                     }
                 }
                 printf("login failed, try again \n");
            }
}