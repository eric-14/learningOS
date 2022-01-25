/*
The user process typically executes the command interpretr sh , which gets coomads from the user and executes the commands

To redirect inputs from a file, the process can close the file descriptor then open 
 the infile for READ. 

 the file descriptor of the newly opened file would br 0.
 this is beacuses the kernelss open() functions usess the lowest numbered file descriptoe for a newly opened file
 After these, a.out would receive inputs from infile rather than form the keyboard.

 Similarlt, a.out>outfile, the process can close its file descriptor 1 and then ope outfile for WRITE with CREAT if necessary 

 In addition to the close-open paradigm, file descriptors can also be mainpulated directly by dup and dup2 system calss.

 dup() - creates a copy of fd and returns the lowest numbered file descriptor as newfd
 dup2() - copies oldfd to newfd, closing newfd first if necessary . 

after either dup or dup2 the two file descriptos can be used interchangeably since they point to the same open file instance (OFT)


                    PIPE OPERATION

*/

int pid, pd[2];

pipe(pd);           //creates a pipe

pid = fork();       //fork a child to share the pipe

if(pid)             // parent as pipe READER 
{
    close(pd[1]); 
    dup2(pd[0], 0);
    close(pd[0]);
    exec(cmd1);
}else{              //child as pipe writer
    close(pd[0]);
    dup2(pd[1],1);
    close(pd[1]);
    exec(cmd2);

}

//              ALGORITHM OF SH 

while(1)
{
    get a coomand line 
    get cmd token from command line 

    if(cmd ==cd || layout || su ){
        //built in commands
         do cmd directly;
         continue;
    }
    // ofr binary executable commad 
    pid = fork();                   //fork a child sh process
    if(pid){                        // parent sh
        if(no & symbole)            //assume at most one & for main sh
                pid = wait(&status);
        continue;

    }else{
        do_pipe(cmd_line, 0);
    }

    int do_pipe(char *cmdLine, int *pd)
    {
        if(pd){
            // if has a pipe passed in, as WRITER on pipe pd
            close(pd[0]);
            dup2(pd[1], 1);
            close(pd[1]);
        }
        // divide cmdLine into head tail by rightmost pipe symbol
        hasPipe =  scan(cmdLine, head , tail);
        if(hasPipe){
            create a pipe 1pd;
            
        }
    }
}