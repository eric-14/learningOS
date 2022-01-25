/*
Each command invokes a  user command function,
 which calls a syscall interface function 
 to issue a syscall to the MTX 5.0 kernel.

Each syscall is routed to a corresponding kernel function by
 the syscall handler kcinth()

*/
#define AX 8
#define PA 13 
int kcinth()
{
    u16 segment, offset ;
    int a, b, c, d, r;

    /* get syscall parameters from ustack*/

    segment = running->uss;
    offset = running->usp;

    a= get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*(PA+1));
    c = et_word(segment, offset + 2*(PA+2));
    d = et_word(segment, offset + 2*(PA+3));

    // route syscall call to kernel dunctions by call# a

    switch(a){
        case 0 : r = getpid(); break;
        case 1 : r = kps(); break;
        case 2 : r = kchname(b); break;
        case 3 : r = kfork(); break;
        case 4 : r = kswitch(); break;
        case 5 : r = kwait(b); break;
        case 6 : kexit(b); break;
        default: printf("invalid syscall %d\n",a);
    }

    put_word(r, segment, offset + 2*AX); //return value of AX
}
/*
when the number of syscalls is large, it is better to use a branch table containing
kernel function pointers

1. Define kernel function prototypes 

        int kgetpid() , kps(), kchname(), kfork(), kswitch(), kwait(), kexit()

2. Set up a table of function pointer, each index corresponds to a syscall number

                    0          1    2       3       4        5        6
    int(*f[ ])() = {kgetpid, kps, kchname, kfork, kswitch, kwait, kexit};

3 . call the kernel function by the call#a , passing parameters to the function as needed

        int r = (*f[a]) (b,c,d);


*/