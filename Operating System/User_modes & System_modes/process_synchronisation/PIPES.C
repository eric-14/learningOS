/*
these are FIFO  inter-communication channel for processes
Can be synchronous and blocking or asynchronous and non-blocking

when a reader reads from a pipe, if the pipe has data the reader reads from a pipe, if the pipe has data

if the pipe has no data and the writer is writing 
the reader awaits for the writer

A process can only be a reader or a writer to a process but not both 
so a to prevent itself from locking itself out

After creating a pipe, the process forks a child process to shre the pipe

During fork the child inherits all the opened file descriptors of the parent.

the user must designate on of the processes as a writer and the other one as a reader of the pipe
Each porcess must close its unwanted file descriptor .i.e writer must close pd[0]

The writer may terminate first and the reader may continue to read date from the pipe so long a the pipe has data
However, if the reader terminates first the writer should be able to see a broken pipe and issue a broken pipe and also terminate 



*/

/*      UNIX PIPE       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pd[2], n, i;

char line[256], *s="data from pipe";

main()
{
    pipe(pd);           //create a pipe
    if(fork()){         //fork a child as READER, parent as WRITER
        printf("parent %d close pd[0]\n",getpid());
        close(pd[0]);
        while(i++ < 10){        //parent writes to pipe t0 times
                printf("parent %d writing pipe : %s\n",getpid(),s);
                write(pd[1],s, strlen(s));
        }
        printf("parent %d exit\n", getpid());
        exit(0);

    }else{  //child as pipe READER
        printf("child %d close pd[1]\n",getpid());
        close(pd[1]);
        while(1){
            printf("child %d reading from pipe\n",getpid());
            if(n = read(pd[0],line , 256) == 0){exit(0);}

            line[n] = 0 ; printf("%s n = %d\n",line, n);

        }
    }
}
