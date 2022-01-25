/*
Timer Request Queue: Timer service provides each procss with a virtula or logical timer by a single physical timer
                    This is a achieved by maintaining a timer queue to keep track of process timer requests.



When TQE time reads 0 the interrupt handler deetes the TQE form the timer queue and invokes the action function of the TQE.


*/

//timer queue element

typedef struct tq{
    struct tq *next;        //next element pointer
    int time;               //requested time
    struct PROC *proc;      //pointer to PROC
    int (*action)();        //0|1| handler function pointer

}TQE;

TQE *tq, tqe[NPROC];        //tq =  timer queue pointer

/*              Algorithm of itimer

int itimer(t)
{
    1.fill in TQR[pid] info
    2.lock()
    3.tranverse timer queue to compute the position to insert TQE 
    4. insert the TQE and update the time of next TQE 
    5.unlock()
}
*/
/*
when IRQn occurs all other IRQs of lower or same priority cannot be done
To allow further interrupts to occur again the, an interrupt handler must explicitly write EOI 
(0x20) to port 0x20 to signal End-Of_Interrupt procesing
*/


/*
I/O bound processes
Compute bound processes

Responsive time - how fast a system can respond to an event 
 vs throughput - is the numbet of processes completed per unit time

Pre-emption - CPU cn be taken away from the running process at any give time
non-premption - means a process runs until it gives up CPU ny itself 

Round-robin - processes take trun to run 
Dynamic Priority - scheduling each process has a priority which changes dynamically over time

Real time - system responds to external events such as interrupts within a minimum response time
            processing of such events within a given time interval

Time sharing    -   process runs with a guaranteed time slice so that all processes receive their fair share of CPU time

Process scheduling policy is set of rules by which a system tries to achieve good system perfomance by striving for balance among the conflicting goals

it is worth noting that not a single piece of code or module that can be identified as the scheduler. The functions of a scheduler are implemented in may places
e.g. in wakeup(), exit()








*/