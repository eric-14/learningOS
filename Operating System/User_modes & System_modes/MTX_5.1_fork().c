/*
P1 to create a child procss it saves the syscall params, fork-pointer(fpc) , flags , CS , rPC(return pointer ), registers, ES,DS
 into the stack . Then it calls kfork() to create a child process P2 in a different segment.

 But does not load any Umode image file.

 Instead it copies the entire Umode image of P1 to the child segment
 this makes the child's Umode image identical to that of the parent.

PROC[2] savee uss is in 0x3000 the usp remains the same since sp is just an offset relative to the PROC'S segment

We need to change DS, ES, CS in PROC[2] since as things stand it will execute in PROC[1]'S segment

hence the processes will interfere with each other 

DS,ES, CS Should be changed to 0x3000

Because of the copied rpc and fpc values the child will return to the same place and as does the parent 

.i.e to the statement pid=fork()
as if it had called fork() before, except that the returned pid =0

to let the child return a pid=0 simply change its saved uax to 0
*/

int copyImage(u16 pseg, u16 cseg, u16 size)
{
    u16 i;
    for (i=0; i<size; i++){put_word(get_word(pseg,2*i),cseg, 2*1); }

}

int fork()
{
    int pid; u16 segment;
    PROC *p = kfork(0);
    if (p==0) return -1;        //kfork() a child, do not load image file
    segment = (p->pid+1)*0x1000;    //child segment
    copyImage(running->uss, segment, 32*1024);      //copy 32k words 
    p->uss = segment;       //child's own segment
    p->usp = running->usp;      //same as parent's usp

    /*change uDS, uES, uCS, AX in child's */

    put_word(segment, segment, p->usp);      //uDS=segment
    put_word(segment, segment, p->usp+2);       //uES = segment
    put_word(segment, segment, p->usp+2*8);     // uax = 0 
    put_word(segment, segment, p->usp+2*10);       // uCS = segment

    return p->pid;
}