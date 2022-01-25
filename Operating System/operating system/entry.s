    .gloabl pmode_entry 
    .extern p0stack 
    .org 0 

pmode_entry: .code32
              cli 
              movl P0stack, %esp 
              call init 
              
