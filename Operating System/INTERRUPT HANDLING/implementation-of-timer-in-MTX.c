/*
1. Mask out all interrupts by lock(), which issues CLI
2.Intialize MTX kernel as before. Create P0 and let P0 kfork P1 with a Umode image. In kfork(), 
    set the saved flag register to 0x0200(I_bit=1) so that P1 will run with interrupts enabled when it starts.

3. Set syscall and timer interruot vectors by set_vectors(80, int80h); set_vectir(8, tinth)

4. Initialize and enable timer by timer_init(). timer interrupts will occur immediate;y but they are masked out for now.
5.tswitch() to P1, which loads CPU's flag register with I_bit=1, allowing CPU to accept interrupts

With interruts the entire tswitch must be exectes with interrupts masked out.
Uses macro, INTH , to generate approproate entry and exit code for both syscall and timer interrupts.

Since the same INTH macro will also be used later for other interrupts, we shall show the entire INTH macro code

first we need an inkmode field to eery PROC structure at the byte offset 8.
When a process is created in kernel, we initalize its inkmode to 1. When a process goes to Umode, we decrement its inkmode by 1. When a process enters or re-enters Kmode
We increment ist inkmode by 1. Thus, we may use the running proc's inkmode value to determine in which mode an interruot occured.

If inkmode =1, it occurred in Umode. If inkmode > 1, it occurred in Kmode.
Alternatively we may also use the save segment register to determine the CPU mode.

for example, if the saved CS is not the kernel segment (0x1000), the interrupt must have occurred in the Umode.

*/

//Modified PROC structure and the   INTH macro.
struct proc{
    struct proc *next;
    int *ksp;
    int uss, usp;
    int kmode;  //ADD: process in Kmode flag; initial =1
}PROC;

