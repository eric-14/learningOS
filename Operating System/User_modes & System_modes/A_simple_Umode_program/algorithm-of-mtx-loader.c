struct header {
    u32 ID_space; // 0x4100301: combined I&D | 0x4200301: separate I&D
    u32 magic_number;   // code section size in bytes
    u32 tsize;  // code section size in bytes
    u32 dsize;  //initialized data section size in bytes
    u32 bsize;  // bss section size in bytes
    u32 zero;   // 0
    u32 total_size;     // total memory size, including heap
    u32 symbolTable_size;   //only if symbol table is present
}

// a loader is a program which loads a binary executable file into memory for execution 

int load(char *filename, u16 segement){
    /*
        1.find the inode of filename; return 0 if it fails
        2.read file header to get tsize, dsize and bsize;
        3.load [code | data] sections of filename to memory segment
        4.clear bss section of loaded Image to 0;
        5. return 1 for success
    */
}

/*
    we modify the kfork() to kfork(char *filename) which creates a new process
    and loads filename as its Umode image 
*/



                                ///CREATE PROCESS WITH USER MODE IMAGE
PROC *kfork(char *filename)
{
   // 1. create a child PROC readyer  to run from body() in Kmode
   // 2. segment = (child pid + 1 ) * 0x1000;; child mode segment
   // if (filename){
       //load flename to child segement as its Umode image;
       //set up child's ustack for it to return to umode to execute the loaded image
   //}
   //5. return child PROC pointer;
}