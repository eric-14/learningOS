/*
                    ALGORITHM OF INSERT_DIR_ENTRY

1. need_len = 4*((8+name_len+3)/4);     //new entry need length
2. for each existing data block do {
    if(block has only one entry with inode number ==0)
            enter new wntry as first entry in block
    
    else{
        go to last entry in block;
        ideal_len  = 4*((8+last_entry's name_len+3)/4);
        remain =  last entry's rec_len -  ideal_len;
        if(remain >=  need_len)
        {
            trim last entry's rec_len to ideal_len;
            enter new entry as last entry with rec_len = remain;
        }else{
            allocate a new data block;
            enter new entry as first entyr in the data block
            increase DIR'S size by BLKSIZE
        }
    }
    write block to disk;
}
mark DIR's minode modified for write block

        ALGORITHM OF DELETE DIR_ENTRY

1. search DIR'S data block(s) for entry by name;
2. if(entry is the only entry in block)
        clear entry's inode number to 0;
    else{
        if(entry is last entry in block)
            add entry's rec_len to predecessor entry's rec_len;
        else{
            //entry in middle of block
            add entry's rec_len to last entry's rec_len;
            move all trailing entries left to overlay deleted entry;
        }
    }

    write block back to disk




*/

//     ALGORITHM OF MKDIR 
/*
int mkdir(char *pathname)
{
    1.if (pathname  is absolute) dev= rooot->dev;
        else                dev = PROC'S cwd->dev
    
    2. divide pathname into dirname and basename;
    3. //dirname must exist and is a DIR:

        pino = getino(dev, pino);
        pmip = iget(dev, pino);
        check pmip -> INODE is a DIR

    4. //basename nust not exist in parent DIR
            search(pmip, basename) to create a DIR
    
    5. call kmkdir(pimp, basename) to create a DIR;
        kmkdir() consists of 4 major steps:
        -->allocate an INODE and a disk block:
            ino = ialloc(dev); blk= balloc(dev);
            mip = iget(dev, ino);           //load INODE into a minode
        -->initialize mip->INODE as a DIR INODE;
            mip->INODE.i_block[0] = blk;
            other i_block[ ] are 0;
            mark minode modified(dirty)
            iput(mip);              //write INODE back to disk

        -->make data block 0 of INODE to contain . and .. entries;
                write to disk block blk
        -->enter_child(pmip, ino , basename); which enters
        (ino, basename) as a DIR entry to the parent INODE; 

    6.increment parent INODE'S links_count by 1 and maek pmip dirty;
        iput(pmip);    


}


        ALGORITHM TO CREATE

create(char *pathname)
{
    1. this INODE.i_mode field is set to REG file type, permission
            bits set to 0644 for rw-r--r-- and
    2. no data block is allocated for it, so the file size is 0
    3.Do not increment parent INODE'S links count
}


            ALGORITHM OF MKNOD

mknod represents a special file whihc represents either a char or block device with a device number 
=(major, minor)


mknode(char *name, int type, int device_number)
{
    1.the default parent directory is /dev
    2. INODE.i_mode is set to CHAR or BLK file type
    3. INODE.I_block[0] contains device_number=(major, minor)
}


            ALGORITHM OF CHDIR

int chdir(char *pathname)
{
    1. get INODE of pathname into a minode
    2. verify it is a DIR 
    3. change running process CWD to minode of pathname
    4. iput(old CWD); return 0 for OK
}

GETCWD() - get the parent INODE into memory. Search the parent INDOE's data block for the name of the current
            directory and save the name string 
            Repeat the operation for the parent INODE until the root directory is reached.
            Construct the absolute pathname of CWD on return.


            ALGORITHM OF STAT

stat(pathname, STAT *st) returns the information of a file in a STAT structure

int stat(char *pathname, STAT *st)
{
    1. get INODE of pathname into a minode;
    2.copy(dev, ino) of minode to (st_dev, st_ino) of the STAT structure in user space;

    3.copy other fields of INODE to STAT structure in user space;
    4.iput(minode); return 0 for OK;
}





*/