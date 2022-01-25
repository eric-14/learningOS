// MTX HD data structure
struct request{
    struct request *next;       //next request pointer
    int opcode;                 //R|W
    u32 sector;                 //start sector or block
    u16 nsectors;               // number of sectors to R/W
    SEMAPHORE io_done;          // initial value =0
};  
//  I/O_queue = a (FIFO) queue of I/O requests 

hd_read(){
    /*
    1. construct an "I/O request" with SEMPAHORE request.io_done=0
    2. hd_rw(request)
    3. P(request.io_done)              //"wait" for READ completion
    4. read data from request.buf;
    */
}
hd_write()
{
    /*
    1. construct an "I/O request"
    2. write data to request.buf
    3. hd_rw(request);                  // do not "wait" for completion
    */
}
hd_rw(I/O_request)
{
    /*
    1. enter request into(FiFO) I/O_QUEUE
    2. if(first in I/O QUEUE)
                start_io(request);              //issue actual I/O to HD
    
    */
}

//                  MTX HD Driver lower-half

interruptHandler()
{
    /*
    request =  first request in i/o_queue
    1.while(request.nsectors)
    {
        transfer data;
        request.nsectors--;
        return;
    }
    2. request = dequeue(I/O_qeue);     //1st request from I/O queue
    if(requet.opcode ==READ)            //READ is synchronous
        V(request.io_done);             //unblock waiting processs
    else
        release the request buffer;     //delay write; releae buffer
    
    3.if(!empty(I/O_queue))             //if I/O_queue not-empty
        start_io(first request in I/O_queue);   //start next I/O
    */
}