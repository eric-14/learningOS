; section .data    
    
; section .text 

.globl _main,_prints,_NSEC
.globl _getc,_putc,_readfd,_setes,_inces,_error
    BOOTSEG = 0x9800 # booter segment 
    OSSEG = 0x1000 #      MTX Kernel segment
    SSP = 32*1024 # booter stack size 
    BSECTORS = 2 # number of sectors to load initially

start:
    mov ax,BOOTSEG
    mov es, ax 

# call BIOS INIT3 TO LOAD BSECTORS to (segment,offset) = (0x9800,0)

# in C we will use mailman algorithm
# to convert from linear to sector,cylinder and head 
# the code from the disk is uploaded to 0x9800 address


    xor dx,dx 
    # dh=head=0, dl=drive=0
    xor cx,cx 
    # ch=cyl=0 -- track, cl=sector=0
    incb cl 
    # BIOS  counts sectors from 1 
    xor bx, bx 
    #  (ES, BX) = real address = (0x9800,0)
    movb ah, 2 
    # ah=READ 
    movb al, [BSECTORS] 
    # al= number of sectors to load
    int 0x13 
    # call bios input and output function
# far jump to (0x9800, next) to continue there
    jmpi next, BOOTSEG
next:
    # next is used to set up the execution image, code, data, bss section 

    mov ax, cs      
    # set CPU segment registers to 0x9800 CS gives current address of the current position
    mov ds,ax       
    #  we know ES=CS=0x9800 , let DS=CS
    mov ss, ax      
    #  let SS  = CS 
    mov sp , #SSP   
    # SSP is higher address to give room for booter
    call _main 

    # OSSEG is just after the ROM Area

    jmpi 0, OSSEG 
    # 0 is the value of the IP and OSSEG the value of CS --START OF THE EXECUTION OF THE BOOTER

# Input anchard output functions 

_getc: 
    # char getc():return and input char
    xorb ah, ah
    int 0x16 
    # call BIOS to get a char in ax
    ret 

_putc: 
    # putc(char c ): print a char
    push bp 
    mov bp, sp
    movb al, 4[bp] 
    # al =  char
    movb ah , #14 
    # ah = 14
    int 0x10 
    # call BIOS to display the char
    pop bp
    ret 

_readfd: 
    push bp
    mov bp,sp
    movb dl, #0x00
    movb dh, 6[bp]
    movb cl, 8[bp]
    incb cl 
    movb ch, 4[bp]
    xor bx, bx
    movb ah, 0x02
    movb al,_NSEC
    int 0x13
    jb _error
    pop bp 
    ret

_setes:
    # used to set up es segment register 
    push bp
    mov bp,sp
    mov ax,4[bp]
    mov es,ax
    pop bp 
    ret 

_inces:
    mov bx,_NSEC
    shl bx, #5
    mov ax,es
    add ax,bx
    mov es, ax 
    ret

_error:
    push #msg
    call _prints 
    int 0x19           
     # reboot

msg: .asciz "ERROR"

