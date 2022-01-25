_int80h:
    push ax
    push bx
    push cx
    push dx
    push bp
    push si
    push di
    push es
    push ds 


    ;change DS to KDS in order to access sata in Kernal space
    mov ax,cs   ; assume one segment kernel
    mov ds, ax  ; let DS=CS= 0x1000 = kernel DS

USS = 4 ; offsets of uss, usp in PROC
USP = 6

;Save running proc's Umode (uss, uSP) into its PROC.(uss, usp)

    mov bx, _running
    mov USS(bx),ss  ;save uss in proc.uss
    mov USS(bx) , sp ; save in proc.usp

;change ES, SS to kernel segment 0x1000
    mov ax, ds      ;CPU musy mov segements this way
    mov ex,ax
    mov ss, ax

;;switching running proc's stack from u space to K spce

    mov sp, bx      ;sp points at running proc
    add sp, procSize    ;sp -> HIGH END of running's kstack

; we are now completely in L space, stck = running proc's EMPTY kstack

    call _kcinth ; actually handles the syscall

    //RETURN TO Umode

    _getUmode:
        cli         ;mask out interrupts
        mov bx,_running
        mov ax, USS(bx)
        mov ss, ax
        mov sp, USP(bx)
        pop ds
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax  ;return value must be in AX in ustack
        iret
