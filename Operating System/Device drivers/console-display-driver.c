/*
The VDC register can be accessed through the
display adaptor's index register at the I/O port address 0x3D4.

To change a VDC register, first select the intended register.

Then write a (2-byte) value to the data register at 0x3D5 to change the selected register.

When programming the VDC the most important VDC Register pairs are 
        start_address=0x0C-0x0D

        and cursor_position = 0x0E - 0x0F

            DISPLAY DRIVER ALGORITHM
1. call vid_init() to initialize the cursor position and , VID_ORG of the VDC hardware.
        initialize the driver variables row and columns to 0. Clear the screen and position the cursor at the top left.

2. A screen = 25 rows by 80 columns = 2000 words= [attribute byte, char byte] in the video RAM, beginning at start_address=0xB8000.

3.TO DISPLAY A SCREEN: write 4000 bytes(2000 words) to the video ram at the cirrent VID_ORG location. 
        The VDC will display the 2000 chars from start_address=VID_ORG in the video RAM to the screen

4. The driver must keep track of the Cursor position(row,col).
    After displaying a char, the driver must advance the Cursor by one word, which may change(row,col).
    When col >=80 , reset col to 0 and increment row by 1. When row>=25, scroll up one row.
    Handle special chars such as \n and \b to produce the right visual effect

5. Scroll up or down 

    (I) To scroll up one row: increment VID_ORG by one row. Write a row or blanks 
            to the last row on the screen. If the last exceeds video RAM size, copy current screen to the video RAM 
            beginning and reset VID_ORG to 0.
    (II) To scroll down one row : decrement VID_ORG by one row.
*/
#define VDC_INDEX 0x3D4
#define VDC_DATA   0x3D5
#define CUR_SIZE     10         //cursor size register 
#define VID_ORG     12          //start address register
#define CURSOR      14          // cursor postion register 
#define LINE_WIDTH  80          // # characters on a line
#define SCR_LINES   25            // # lines on screen 
#define SCR_BYTES    4000          // bytes of one screen= 25*80
#define CURSOR_SHAPE 15             //block cursor for EGA/VGA

//attribute byte: 0x0HRGB, H=highlight; RGB determine color

u16 base = 0xB800;          // VRAM base address
u16 vid_mask = 0x3FFF;      // mask = Video RAM size - 1
u16 offset;                 // offset from VRAM segment base
int color;                  // attribute byte
int org;                    // current display origin, r.e. VRAM base 
int row, column;            //logical row, col position

int set_VDC(u16 reg, u16 val) //set VDC register reg to val
{
    lock();
    out_byte(VDC_INDEX, reg);       //set index register
    out_byte(VDC_DATA, (val >> 8) & 0xFF);  //output high byte
    out_byte(VDC_INDEX, reg+1);         // next index register
    out_byte(VDC_DATA, val & 0xFF);       // output low byte
    unlock();
}

int vid_init()      //initialize org=0 (row,column)
{
    int i,w ;
    org = row = column =0 ;              // initialize globals
    color = 0x0A;                       //high Yellow
    set_VDC(CUR_SIZE, CURSOR_SHAPE);    //set cursor size
    set_VDC(VID_ORG,0);                 //display  origin to 0
    set_VDC(CURSOR,0);                  // set cursor postion to 0
    w = 0x07000;                        //White, blank char

    for(i=0;i<25*80;i++)               //clear screen
    {put_word(w, base , 0+2*i);}      // write 25*80 blanks to VRAM
}

int scroll()        // scroll up one line
{
    u16 i, w, bytes;
    // test offset =  org +ONE screen + ONE more line
    offset =  org + SCR_BYTES + 2 * LINE_WIDTH;

    if(offset <= vid_mask)  //offset still within VRAM area
    {org += 2 * LINE_WIDTH; }     //Just advance org by ONE line
    else{
        //offset exceeds VRAM area ==> reset to VRAM beginning by
        //copy current rows 1-24 to BASE, then reset org to 0
        //copying the next screen to the start of VRAM
        for(i = 0;i < 24*80;i++ )
        {
            w = get_word(base, org+160+2*i); // We start at 160 since we wrote a series of blanks in the last line
            put_word(w, base , 0+2*i);
        }
        org = 0;
        //org has been set up properly 
        offset = org + 2*24*80;     //offset = beginning of row 24
        // copy a line of BLANKS to row 24
        w = 0x0C00;     //HRGB=1100 ==> Highloght RED, Null char

        for(i=0;i<80;i++)
        {put_word(w, base,offset + 2*i);}

        set_VDC(VID_ORG,org >> 1);      // set VID_ORG to org
    }
}
    int move_cursor()       //move cursor to current position
    {
        int pos = 2*(row*80 + column);
        offset = (org + pos) & vid_mask;
        set_VDC(CURSOR,offset >> 1);
    }
    // display a char, handle special chars '\n','\r','\b'

    int putc(char c)
    {
        u16 w, pos;
        if(c=='\n'){
            row++;
            if(row>=25){
                row = 24;
                scroll();
            }
            move_cursor();
            return;
        }
        if(c=='\r'){
            column=0;
            move_cursor();
            return;
        }
        if(c=='\b'){
            if(column > 0){
                column--;
                move_cursor();
                put_word(0x0700,base , offset);
            }
            return;
        }
        // c is an ordinary char
        pos = 2*(row*80 + column);
        offset = (org + pos) & vid_mask;

        w = (color << 8) + c;
        put_word(w, base, offset);
        column++;
        if(column >= 80){
            column = 0;
            row++;
            if(row >= 25){
                row = 24;
                scroll();
            }
        }
        move_cursor();

    }
 /*
 In Umode change putc() to a syscall, which calls putc() in the display driver.

 Add a Umode command, color, which issues a syscall(9, color) to change the 
 display color by chnaging the color bits in the attribute byte
 
 */