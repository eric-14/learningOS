    .globl _tswitch         ;export to c
    .globl _main,_running,_scheduler,_proc0,_procSize
    mov sp, #_proc          ;set up point to proc[0]
    add sp,_procSize        ; let sp point to high end of proc[0] 

start:
    mov ax, cs      ;set DS=CS=SS 0x1000
    mov ds, ax 
    mov ss, ax
    mov sp ,#_proc0        ;set sp point to proc0 in c code
    add sp,_procSize        ;sp point to high end of proc0
    call _main              ; if main() returns, just halt
    hlt

_tswitch:                   ;tswictch( function
SAVE:
    push ax
    push bx 
    push cx 
    push dx 
    push bp 
    push si 
    push di 
    pushf
    mov bx,_running         ;bx ->proc 
    mov 2[bx],sp            ;save sp to proc.ksp

FIND:
    call _scheduler         ;call scheduler() in c
RESUME:  mov bx,_running       ;bx -> running proc 
        mov sp , 2[bx]
        popf 
        pop di 
        pop si 
        pop bp 
        pop dx 
        pop cx
        pop bx 
        pop ax 