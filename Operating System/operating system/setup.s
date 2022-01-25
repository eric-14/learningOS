        .text 
        .set KCODE, 0x08        #Kernel code segment selector
        .set KDATA. 0x10        #Kernel data segment selector
        .set K_ORG , 0x10000    #MTX kernel loaded here by the booter 
        .set K_HIM , 0x100000   #MTX kernel running address 
        .set GDT_ADDR. 0x9F000  # hard coded GDT address
        .set GDT_SIZE 40        # GDT size in bytes
        .org 0

.code16  

#set segments registers cs,ds,ss = 0x9020 , stack size = 8KB
    ljmp $0x9020 , $go 

go:     movw %cs, %ax
        movw %ax, %ds 
        movw %ax, %ss
        movw $8192, %sp

#mov setup_gdt to GDT_ADDR=0x9f000
        movw $GDT_ADDR>>4, %ax 
        movw %ax, %es
        movw $gdt, %si 
        xorw %di, %di 
        movw $GDT_SIZE>>2, %cx
        rep 
        movsl 

#load GDTR with gdt_desc = [GDT_limit|GDT_ADDR]
        lgdt gdt_desc

# enter protected mode by writing 1 to CR0 

        cli 
        movl %cr0, %eax
        orl $0x1 , %eax 
        movl %eax , %cr0
#do a "ljmp" to flush instruction pipeline and set CS to KCODE

        .byte 0x66, 0xea          #prefix + ljmp-opcode
        .long 0x9020+next         #SETUP is at 0x90200
        .word KCODE               #CS selector 

    ljmpl $KCODE, $(0x9020+next)

next: # load other selectors with 0x10 for kernel data entry in GDT 
    movl $KDATA,%eax 
    movw %ax, %ds
    movw %ax, %es 
    movw %ax, %fs 
    movw %ax, %gs 
    movw %ax, %ss

#Move MTX kernel from 0x10000 to 0x100000, then jump to 0x100000

    cld 
    movl $K_ORG, %esi
    movl $K_JIM, %edi 
    movl $1024*64, %ecx
    rep 
    movsl 
    ljmp $KCODE, $K_HIM 

gdt: .quad 0x00000000000000000       #NUll descriptor
      .quad 0x00CF9A000000FFFF        #Kernel cs 
      .quad 0x00CF92000000FFFF         #KERNEL DS 
      .quad 0x0000000000000000          #task tss 
      .quad 0x0000000000000000          #task ldt 

gdt_desc: .word .=gdt=1 
           .long GDT_ADDR #hard coded 0x9F000; can be changed 
           .org 512 

