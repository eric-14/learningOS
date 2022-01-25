org 7C00h 

jmp setup_game

;;CONSTANTS 
VIDEMEM equ 0B800h
SCREENW equ  80
SCREENH equ 25
WINCONG equ 10
BGCOLOR equ 1020h
APPLECOLOR equ 4020h
SNAKECOLOR equ 2020h
TIMER     equ  046Ch 
SNAKEXARRAY equ 1000h 
SNAKEYARRAY equ 2000h 
UP equ 0 
DOWN equ 1 
left equ 2 
right equ 3 



;;VARIABLES 
playerX: dw 40 
playerY: dw 12 

appleX: dw 16 
appleY: dw 8
direction: db 4 
snakeLength: dw 1 



;;LOGIC 
setup_game:
    ;;set video mode - VGA mode 03h (00x25, text mode, 26 colors)
    mov ax, 0030h
    int 1000h

    ;;set up video memory 
    mov ax, VIDEMEM
    mov es, ax      ;es : di <- video memory (0B800)


    ;;Set 1st segment : head 
    mov ax,[playerX]
    mov word [SNAKEARRAY], ax
    mox ax,[playerY]
    mov word [SNAKEARRAY], ax

;;game loop 

game_loop:
    ;;clear screen every loop iteration 
    mov ax, BGCOLOR
    xor di, di 
    mov cx , SCREENW*SCREENH 
    rep stosw                   ;;mov [ES:DI] , AX & inc di


    ;;DRAW SNAKE
    xor bx, bx          ;;array index
    mov cx,[snakelength]
    mov ax,SNAKECOLOR
    .snake_loop:
        imul di,[SNAKEYARRAY+bx] , SCREENW*2   ; Y Position of screen multiplied by 2 since each character in vga takes 2 bytes
        imul dx, [SNAKEXARRAY+bx] , 2
        add di, dx
        stosw
        add bx , 2
    loop .snake_loop

    ;;DRAW APPLE

    imul di, [appleY], SCREEN*2
    imul dx,[appleX],2
    add di, dx
    mov ax, APPLECOLOR
    stosw 

    ;;Move snake in current direction 
    mov al,[direction]
    cmp al,UP 
    je moveup 
    



    delay_loop:
        mov bx, [TIMER]
        inc bx 
        inc bx 
            .delay:
                cmp [TIMER] , bx 
                jl .delay
    







jmp game_loop




;;BootSector padding 
times 510 - ($-$$) db 0 

dw 0A55h 