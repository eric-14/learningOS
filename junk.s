Sum:
    push bp 
    # save the bp of the previous instruction
    push ax
    push bx
    # Save the registers of the current execution image
    mov bp, sp 
    # this is where the new stack of the current sum function should start. 
    # It should start at the top of the previous stack
    mov ax,4[ bp]. # Get the 1st parameter passed to this function
    # Assuming a 32 bit system the first parameter is at 4 bytes bp + 4
    mov bx, 8[bp] 
    # get the second parameter at bp + 8
    pop bp 
    ret