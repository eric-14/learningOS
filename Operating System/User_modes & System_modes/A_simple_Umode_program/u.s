    .globl _main, _syscall,_exit,_getcs

start: call _main
        ;if main return, sysycall exit(0) to terminate in MTX kernek
        push #0
        call _exit

;;int syscall(a,b,c,d) from C code 

_syscall: int 80
        ret 

_getcs: mov ax, cs  ;getcs() return CS segement register
        ret