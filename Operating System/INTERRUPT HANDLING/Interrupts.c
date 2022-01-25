/*
NMI - Non -maskable Interupt
INTR - Interrupt request

A masked put interrupt is kept pending until th PU flag, I_bit is changed to 1. The instruction CLI clears the flag,I_bit and STL set the flag .I_bit


The interrupts hardware consists of two interconnected 8529 Programmable Interupt Controllers(PICs)
The slave PIC's INT request line is fed to IRQ2 of the master PIC. the IRQs of the slave PIC 

IRQ8 = real-time clock
IRQ9 = redirected IRQ2
IRQ10 - 12 = general use
IRQ13 = FPU,
IRQ14 = IDE channel 1 (HD)
IRQ15  = IDE channe 2 (CDROM)


EACH 8259 PIC has an interrupt control(INT_CNTL) register ans an interrupt mask 
(INT_MASK) register

        INT_CNTL control register at 0x20 (slave : 0xA0)
        INT_MASK mask register at 0x21 (slave:0xA1)

The INT_MASK register is used to enable/disable individual IRQ lines.
An IRQn line is enabled if bit n of INT_MASK register = 0 ; bit n =1 disables IRQn.

The INT_CNTL register is used to signal End-of-Interrupt(EOI) processing.
After sending an IRQn interrupt to the CPU, the 8259 PIC automatically blocks all IRQs of the same or lower priority.

The software interrupt handler must explicitly write EOI to the INT_CNTL
register at the end of interrupt processing.
Otherwise, no further interrupts of the same or lower priority can occur again

The CPU does not accept the request until ts flag.I_bit is 1. while the flag.I_bit = 0 , INTR will remain asserted which keep the interruot request pending.

when an interupt occurs and the CPU's fag.I_bit is 1, the CPU accepts the interrupt and starts interrupt processing
at the end of 
current instruction or at an interruptible point in a long instruction.

        master PIC : IRQ0- 7 : 0x80 - 0x0F
        Slave PIC : IRQ8-15 : 0x70 - 0x70

        SEQUENCE OF EVENTS DURING INTERRUPT PROCESSING 

1.CPU issues an INT_ACK signal to the 8259 PIC, asking for the interrupt vector
2.The 8259 PIC spits out a byte value, known as the interrupt vector, to the data bus, allowing
the CPU to read the vector. When the PC starts, the interrupt vectors of the PICs are initialized by BIOS to the default values

The interupt vectors can be changed by programming the 8259 interrupt controllers.
Since it is unneccessary to remap IRQ vectors in 16-bit real mode.

3. For PCs in real mode, the lowesr 1KB memory contains 256 interrupt vectors.
    Each vector area contains a pair of [PC, CS], which point to the entry point of 
    an interrupt handler. In order to handle an interrupt IRQn,
    an interrupt handler must be installed for the interrupt vector before the interupt occurs
4.After acquiring the vector n, the CPU's actions are identical to that of executing an INT n instruction.
    It first saves[flag,CS,PC] into the current stack, changes flag register's T and I bits 0, load[PC,CS],
    from the vetor location [4*n,4*n+2] and continues execution from the new [PC,CS], 
    which is the entry point of the IRQn interrupt handler.

TIMER AND TIMER SERVICE

The channel-0 timer is connected to the master PIC at IRQ0.
It can be programmed to generate square waves, which are used to trigger periodic timer interrupts

The PC's channel-0 timer has a base frequenecy of 1193182 Hz . it can be programmed to generate 60 interrupt second.
The time interval between timer interrupts is called a tick

Timer initialization: 








*/

#define TIMER_COUNT 0x00    //cc00xxxx, cc=channel, x=any
#define SQUARE_WAVE 0x36    //ccaammmb aa=access, mmm=mode,b=BCD
#define TIMER_FREQ 1193182L //timer frequency
#define TIMER_COUNT TIMER FREQ/60       //intial value of counter
#define TIME_PORT 0x40                  //channel 0 timer port address
#define TIMER_MODE 0x43
#define TIMER_IRQ  0            //IRQ0
#define     INT_CNTL 0x20       //master PIC mask reg:bit i=0=enable
#define     INT_MASK 0x21       //master PIC mask reg:bit i=0=enable

int tick = 0;           //tick count

int timer_init()        //Initialize channel 0 of the 8253A timer to 60Hz
{
    out_byte(TIMER_MODE, SQUARE_WAVE);      //set tmer continuous MODE
    out_byte(TIMER_PORT,TIMER_COUNT);       //write timer low byte
    out_byte(TIMER_PORT, TIMER_COUNT >> 8);     //write timer high byte
}

/*
Install a timer handler to vector 8 and enable timer interrupt: Assume that _tinth is the 
entry point of the timer interruot handler in assembly code

int tinth();        //declare tinth() as a function
set_vectot(8, tinth);   //install address of tinth() to vector 8
q

which writes [_tinth,0x1000] to [4*8,4*8+2], similar to installing int80h for syscalls

Then enable timer interrupts by 
  
*/


//Assue IRQ 0-7 ONLY.claer bit 0 in PIC mask register 
int enable_irq(u16 ir)
{
    out_byte(INT_MASK,in_byte(INT_MASK)& ~(1 << irq));

}
enable_irq(TIMER_IRQ);
/*
Once initialized and enabled the timer will interrupt at IRQ0 once evry 1/60s.
If the system is not yet ready to handle interrupts, we mask out CPU interrupts until it is ready to accept interrupts.

4.Interrupt Processing Stack : Unlike system calls, which can only occur in Umode, interrupts may occur in either Umode or Kmode.
    Interrupt processing must be done in Kmode. When a timer interrupt occurs, the CPU save [flag,CS,PC] of that interrupted point 
    in the current stack, which may be either the useer mode or kernel mode stack .
    Then it follows the vector 8 contents to continue execution from [_tinth,0x1000] in the MTX kernel

    At this moment, only CS = 0x1000, all other CPU registers are still those of the interrupted point. Then a decision must be made in order to set up the correct execution environment

    if the CPU was in Umode before the interrupt we must change DS, SS, ES to the kernel segment 0x1000 save the interrupted Umpde (SS, SP) and switch to the running process's stack 
    Then call a handler function to continue processing the interrupt.

    In this case the situation is exactly the same as that of syscall when the process enters Kmode via INIT 80.
    We save the CPU registers into the running proc's ustack , save (SS, SP) into ots PROC(uss, usp) and use the proc's empty
    kstack .i.e let sp point to the high end of the runnning proc's kstack


    If the CPU was in Kmode before the interrupt, then there is no need to change DS,SS,ES since they alreadt point to the kernel segment 0x1000.
    In this case, we must continue to use the running proc's kstack since it not only 
    contains the saved context of the interrupted point but also the execution history 
    of the interrupted process in Kmode.

    Some CPU's status register has a current/previous mode field, which allows an interrupt handler to determine in which mode the interrupt occurred.

    in other CPU's e.g. x86 in protected mode, the CPU switches to kernel mode stack automatically folloeing an interrupt, so there is no need to decide which stack to use.

    Since the x86 CPU in real mode does not have this capability, we have to use some other means to determine whether an interrupt occurred in Umode or Kmode.
    In either case, we must ensure that interrupt processing is performed in Kmode, so that CS=Kernel code segment,
    DS=SS=Kernel data + stack segment and stack is the running proc's kstack

    5.After establishing an appropriate kernel mode stack, we may call thandler() in C, which implements the actual timer interrupt 
        processing. In thandler() , we may update the current time, display a wall clock, update running proc's CPU usage time,
        adjust process priorities and start time dependent work,etc. Then issue EOI to signal end of interrupt processing.
    
    6.When thandler() finishes, it returns to the calling in _tinth, which returns to the interrupted point by

            _ireturn: ! same as _goUmode
                    cli ! mask out interrupts
                    if(process was in Umode){
                        restore(SS, SP) from running PROC.(uss, usp);
                    }

                    pop saved registers in reverse order 

                    iret
    
    7. When control enters an interrupt handler, the CPU's interrupts are masked out.
            After saving the interrupted context, the interrupt handler typically unmasks interrupts 
            to allow the CPU to accept other interrupts of higher priority.
            If so , the CPU may handle another interrupt before the current interrupt handler finishes.
            Thus, interrupt processing may be nested.

            for nested interrupts the first interrupt may occir in Umode.
            Any subsequent interrupts must occur in Kmode.

            In the latter case, we continue to use the proc's kstack to save and return to the last interrupted point.
            therefore, our scheme can handle nested interrupts. The maximal depth of nested interrupts is limited by the number of different IEQs which is 15 in the case of Intel x86

            This alos implies that every ksatck must be big enough to contain up to 15 layerrs if saved interrupted contexts and their stack frames.

            Instead if requiring every process to have such a big kernel stack, some CPUs use a single hardware interrupt stack for interrupt processing.
            in that case, each proc's kstack only needs enough space to maintain its execution context in Kmode.






*/






