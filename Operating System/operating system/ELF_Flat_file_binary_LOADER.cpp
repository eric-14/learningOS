/*
open the image file for read(Use MTX's internal open)
Read the elf- header to check whether it's an ELF file;


*/
if(!ELF){
    //assume flat file BINARY FILE 
    //determine file size; using the MTX's internal kfdsize()

    //load file contents to process image area;
    return;
}
//      elf file 

/*
1.loacte program header(s)
for each program header do {
    get section's offset, loading address and memory size; 
    load section to virtual address until memory size;
    set section's R|w|Ex attributes in loaded pages;
}

*/
