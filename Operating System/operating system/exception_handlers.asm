    .MACRO ECODE n   ;; for exceptions with error code
      .global trap\n 
trap\n:  pushl $\n      ;;push vector number 
        jmp trap_all 
    .ENDM 

    .MACRO NOECODE n    ;;for exceptions without error code
        .global trap\n
trap\n: pushl $0
            push $\n 
            jmp trap_all 
        .ENDM

;;call macro  ECODE or NOECODE, depending on whether the exception generates an error code or not.


trap_all: 
        pushal
        pushl %ds
        pushl %es
        pushl %fs
        pushl %gs 
        pushl %ss
        #change to kernel data segments
        movw $KDATA, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs


        movl running, %ebx   #running->inkmode++
        incl 8(%ebx)         #inkmode at byte offset 8 in PROC
        movl 52(%esp),%ecx   #get nr at byte offset 4*13 = 52

#Use nr to call the corresponding handler(*trapHandler[nr])()

        call *trapHandler(, ecx , 4)    #use 0 + ecx*4 as index 

ereturn:
        movl running, %ebx
        decl 8(%ebx)        # test whether was in Umode or Kmode
        jg   kreturn        # to return to Kmode if it was in Kmode 
        call kpsig          #return to Umode handle signal first

kreturn: #return t point of excpetion; kmode traps never return 
        addl $4, %esp  #for %ss
        popl %gs
        popl %fs
        popl % es
        popl %ds
        popal 
        addl $8, %esp       #pop vector _nr and err_code
        iret  


