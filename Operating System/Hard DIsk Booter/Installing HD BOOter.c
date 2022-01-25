/*
1. dumps the first 432 bytes of the booter to the MBR without distrubingthe partition table, which begins at byte 444

2. dumps the entire booter to sectors 1 and beyond.
During booting, BIOS loads the MBR to 0c7C00 and executes the beginning part of the hd-booter

the hd-booter reloads the entire booter beginning from sector 1, to 0x9800 and continues execution in the new segment 
*/

// Anything that can go wrong will go wrong 

// Before writing to the MBR of HD write the partition table down incase you mess up 