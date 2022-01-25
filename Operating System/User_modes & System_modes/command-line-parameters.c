/*
First we pad command line with an extra byte to make the total length even,
if necessary. put the entire string inot the high end og ustack and lets point at the string in ustack.

Then we create a syscall interrupt satck frame for the process to goUmode

when execution begins in Umode, the stack top contains s , which points to the command string in ustack.

Accordingly we modify u.s to call main0(), whihc parses the command line into string tokens and then call main(int argc, char *argv[]) 


main0(char *s)// *s is the original command line 
{
    tokenize *s into char *argv[ ] with argc = number of token strings;

    main(agrc, argv)
}

with fork and exec we can standardize the execution of user commands byba simple ah. First we precompile main0.c as crt0.o and put it into the link library mtxlib as c startup code for all MTX Umode programs 

then w write umode programs in C as 


*/

//                      SH.C

#include "ucode.c"  //user commands and syscall interface

main(int argc, char *argv[ ])
{
    int pid, status;
    while(1){
        //display the executable commands in /bin directory
        //prompt for a command line cmdline = "cmd a1 a2 a3 a4 ......an"
        if(!strcmp(cmd,"exit")){exit(0);}

        //fork a child process to execute the cmd line

        pid = fork();
        if(pid) //the command line run only on process 0 parent sh waits for child to die
            { pid = wait(&status);}
        else                    //child exec cmdline
            { exec(cmdline);}      //exec("cmd a1 a2 a3 ....an")

    }
}


/*
then compile all Umode as binary executables in the /bin directory and run sh when the MTX starts.

This can be improved further by changing P1'x Umode image into an init.c file
*/

//                          INIT.C

main()
{
    int sh, pid, status;
    sh = fork();

    if (sh){                      //P1 runs in a while(1) loop
        while(1){
            pid = wait(&status);        //wait fro ANY child to die
            if(pid==sh){            // if sh died, fork another one
                    sh = fork();
                    continue;
            }
            printf("P1: I just buried an orphan %d\n", pid);
        }
    }
    else{
        exec("sh");         //child of P1 runs sh
    }
}
/*
P1 us the INIT proess, which xecutes the /bin/init file.

It forks a child process P2 and waits for any zombie children.
P2 exec to /bin/sh to become the sh process. 
The sh process runs in a while(1) loop in which it displays a menu and asks for a command line to execute.

All commands in the menu are user mode programs 
whem the user enters a command line of the form cmd parameter-list sh forks a child process to execute the command line and waits for the child process to terminate.

The child process uses exec to execute the cmd file , passing parameter-list to the program

When the child process terminates, it wake up the sh process, which prompts the INIT process P1 to fork another sh


*/



