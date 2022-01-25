;;;Read DH sectors into ES:BX memory location from drive DL

disk_load:
        push dx ;push DX on stack so we can compare number of sectors to read
        
        mov ah, 0x02 ; int13-2 BIOS READ DISK  sectors into memory
        mov al ,dh  ;number of sectors to read
        mov ch, 0x00 ;cylinder 0
        mov dh, 0x00    ;head 0x00
        mov cl, 0x02    ;start reading at CL sector 2 

        int 0x13

        jc disk_error   ;jump if disk read error (carry flag set/=1)

        pop dx          ;restore DX from the stack

        cmp dh, al      ;if AL(# sectors actually read) != Dh (# sectors we wanted to read)
        jne disk_error     ; sectors read not equal to numer we wanted to read
        ret ;ret to caller

disk_error: 
        mov bx, DISK_ERROR_MSG
        call print_string
        hlt     ;halts cpu until the an int is called

DISK_ERROR_MSG: db 'Disk read error!!!!!',0
