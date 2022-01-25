BOOSEG = 0X9800
SSP = 32*1024 !32KB bss + stack; may be adjusted
.globl _main,prints,_dap,_dp,_bsector,_vm_gdt !IMPORT

.globl _diskr,_getc,_putc,_getds,_setds !EXPORT
.globl _cp2himem , _jmp_setup

!MBR loaded at 0x07c0 . load entire booter to 0x9800

start:
    mov ax, #BOOSEG
    mov es, ax
    xor bx, bx      !clear BX=0
    mov dx, #0x0080 !head 0, HD 
    xor cx,cx
    incb cl         !cyl 0, sector 1
    incb cl         !start reading at sector 2 right after our boot sector
    mov ax, #0x0220 !READ 32 sectors, booter size up to 16KB
    int 0x13

    !far jump to (0x9800, next ) to continue execution there
    
    jmpi next , BOOSEG  !CS=BOOTSEG IP =next

    next:
        mov ax,cs   !set CPU segment registers
        mov ds,ax
        mov ss, ax 
        mov es, ax  !CS=SS=DS=ES=0X9800
        mov sp , #SSP !32KB STACK
        call _main
        test ax,ax     ! check return value from main()
        je error        ! mian() return 0 if error
        jmpi 0x7000, 0x0000 ! otherwise as chain booter
    

    _diskr:
        mov dx, #0x0080  !drive= 0x80 for HD
        mov ax, #0x4200  
        mov si, #_dap 
        int 0x13        !call BIOS INIT13-42 read the block
        jb error        ! to error if CarryBit is on 
        return
    
    error:
        mov bx, #bad
        push bx
        call _prints
        int 0x19        ! reboot

    bad: .asciz "\n\rERROR!\n\r"

    _jmp_setup:
        mov ax, 0x9000      !for SETUP in 2.6 kernel:
        mov ds, ax          !DS must point at 0x9000
        jmpi 0, 0x9020      !jmpi to execute SETUP at 0x9020



    ! for aech batch of k<=16 blocks, load to RM=0x10000 ( at most 64KB)
    !then call cp2himem to copy to it VM=0x100000 + k*4096
    _cp2himem:
        push bp 
        mov bp,sp 
        mov cx, 4[bp]   !words to copy (32*1024 or less)
        mov si,#_vm_gdt
        mov ax,#0x8700
        int 0x15
        jc error
        pop bp 
        ret 
