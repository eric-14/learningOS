    .globl _tinh,thandler

USS = 4; 
USP = 6;
INK = 8 ;;BYTE offset of inkmode in PROC

    MACRO INTH ; as86 macro : parameters are ?1 ?2, etc
        push ax
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        push es
        push ds
        mov ax, cs
        mov ds , ax         ; Assume one-segment kernel : set DS to kDS = 0x1000

        mov bx, _running       ;;bx points to running PROC
        inc INK[bx]             ;;enter Kmode : inc proc.inkmode by 1
        cmp INK[bx], #1         ;;if proc.inkmode = 1, interrupt was in Umode
        jg ?1                   ;; inkmode > 1 : interrupt was in Kmode

;;interrupt in Umode: save interrupted (SS,SP) into PROC

    mov bx,_running         ; ready to access running PROC
    mov USS[bx],sp         ; save SS in PROC.uss
    mov USP[bx],sp          ; save SP in PROC.usp

;;change ES,SS TO Kernel Segment
    mov ax,ds
    mov es,ax       ; CS=DS=SS=ES in Kmode
    mov ss,axmov sp, _running       ;sp -> running PROC
    mov sp, _procsize               ; sp - > running proc's kstack high end

    
?1 : call _?1                            ;call handler in c
    br _ireturn                      ; return to interrupted point
 MEND


;; install interrupt handlers

_int80h: INTH kcinth        ; install syscall INT 80 handler
_tinth: INTH thanler        ;   install timer INT 8 handler

;;interrupt handlers are installed by INTH macro calls of the form

;       _entryPoint:INTH chandler

;;Each INTH macro call generates a block of assembly code, in which the symbol ?1 is replaced with the parameter chandler.
;;In the generated assembly code, it saves all the CPU registers in the current stack. Then it switches DS ro kernel's data segment
;;to access the PROC structure if the interrupted process. It uses the proc's inkmode to determine whethre the interrupt occurred in Umode or Kmode.
;;If the interrupt occurred in Umode, the actions are the same as that of INT 80
;;In this case it uses running proc's empty kstack to call chandler().
;;if the interrupt occurred in Kmode, it continues to use the running proc's kstack to call chandler() directly.
;;When the chandler() function finishes, it returns to _ireturn, which is the same as _goUmode.
;;Depending on the proc's inkmode value, it uses the saved interrupt stack frame in either the proc's ustack or Kstack to return to the interrupted point.


_ireturn: goUmode:
        cli
        mov bx,_running ;bx points at running PROC
        dec INK[bx]     ;dec proc.inkmode by 1
        cmp _inkmode, #0     ; inkmode==0 means proc was in Umode
        jg xkmode              ; by pass return to Umode code

;return to Umode: restore uSS ,uSP from running PROC.(uss,usp)
        mov ax, USS[bx]
        mov ss,ax               ; restore SS
        mov sp,USP[bx]          ;restore SP

xkmode: pop ds
        pop es
        pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax 
        iret 
