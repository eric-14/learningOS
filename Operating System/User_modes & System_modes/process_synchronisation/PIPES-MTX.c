/*
A process creates a pipe by the syscall pipe(pd[2]). The pipe()
syscall allocates a pipe object
*/

typedef struct pipe{
    char buf[PSIZE];        //circular dat buffer
    int head, tail;            //circular buf index
    int data, room;             //number of data & room in pipe
    int nreader, nwriter;       //number of reader, writers on pipe
}PIPE;

//A PIPE has a read end and a write end. Each end is represented by an 
//Open File Descriptor (OFD) object;

typedef struct oft{
    int mode;       //READ_PIPE | WRITE_PIPE
    int refCount;       //number of PROC's using this pipe end
    struct pipe *pipe_ptr;  //pointer to the pipe object
}OFT;

/*
in an OFT , mode= READ_PIPE or WRITE_PIPE, whihc identifies the pipe end, 
refCount records the number of processes at the pipe end
pipe_ptr points to the pipe obj

From a process point of view it can access the PIPE object if it has the PIPE's OFTs

Thus we add a field OFT *fd[NFD] to every PROC structure
 
The fd[] array contains pointers to OFTs which are opened file instances

each index of fd[] array is called a file descriptor

When a process opens a fle it uses a free entry in the fd[] array to point at the OFT of the opened file.

When a process creates a pipe it uses two fd entries each points to an OFT on the pipe object 
*/

/*
                ALGORITHM OF PIPE CREATION : PIPE(INT PD[2])

1.Add a file descriptor array fd[NFD], to every PROC structure

2. Allocate a PIPE object. Initialize the pipe object with head=tail=0; data=0; room=PSIZE;
    nreaders=nwriters=1;
3. Alloacte 2 OFTS. initialize the OFTs as readOFT.mode = READ_PIPE; writeOFT.mode = WRIRE_Pipe;
4. Allocate 2 free entries in the PROC.fd[] array, e.g fd[i] and fd[j] let fd[i] point to the readOFT and fd[j] point to the writeOFT
5.write index i to pd[0] and index j to pd[1] point to writeOFT.
6.return 0 for OK


After creating a pipe the process must fork() child to share the pipe.
Modify fork() to let the child process inherit all the opened filed descriptors of the parent

int fork()
{
    // same as before but Add copy opened file descriptors
    //asume PROC *p points at the child PROC

    for(int i=0; i<NFD; i++)
    {
        if(running->fd[i]){
            p->fd[i] = running->fd[i];
            p->fd[i]->refCount++;   //inc OFT.refCOunt by 1

            if(p->fd[i]->mode == READ_PIPE){
                p->fd[i]->pipe_ptr->nreader++;
            }
            if(p->fd[i]->mode == WRITE_PIPE)
            {
                p->fd[i]->pipe_ptr->nwriter++;      //pipe.nwriter++
            }
        }
    }
}

After fork, both processes have 2 file descriptors
Since each process must be either a reader or writer each process must close an unwanted pipe end.

        CLOSE ALGORITHM

1.validate fd to make sure it's a valid opened file descriptor
2.Follow PROC.fd[fd] to its OFT. decrement OF.refcount by 1;
3. if(WRITE_PIPE){
    decrement nwriter by 1;     //last writer on pipe
    if(nwriter == 0){ deallocate PIPE}
    deallocate writeOFT;
}
wakeup(data);       //wakeup ALL blocked readers
else{
    //read pipe
    decrement reader by 1
    if(nreader==0)//last reader on pipe
    {
        if(nwriter==0){deallocate PIPE;}
        deallocate readOFt;
    }
    wakeup(room);           //wakeup ALL blocked writers
}

5. Clear caller's fd[fd] to 0; return OK;
6.Show how the reader and the writer work

                ALGORITHM OF READ PIPE

int read_pipe(int fd, char *buf, int n)
{
    int r =0 ;
    if(n<=0){return 0;}

    validate fd; from fd, 
    get OFT and PIPE pointer p

    while(n){
        while(data){
            read a byte from pipe buf;
            n--;r++;data--; room++;
            if(n==0) break;
        }
        if(r){              //has read some date
            wakeup(room);
            return r;
        }
        //pipe has no data
        if(nwriter){    //pipe still has a writer
            wakeup(room);   //wakeup All writers, if any
            sleep(data);        //sleep for data
            continue;

        }
        //pipe has no writer and no data
        return 0;
    }
}

            ALGORITHM FOR WRITER

int write_pipe(int fd, char *buf, int n)
{
    int r =0;
    if(n<=0){return 0;}
    validate fd;
    from fd get OFT and pipe pointer o;

    while(n){
        if(!nreader)        //no more readers
        {kexit(BROKEN_PIPE);}           //BROKEN_PIPE error
    while(room)
    {
        write a byte from buf to pipe;
        r++;data++; room--; n--;
        if(n==0)break;
    }
    wakeup(data);               //wakeup ALL readers, if any.
    if(n==0) return r;
    //still has data to write but pipe has no room

    sleep(room);            //sleep room   
    }
}
*/