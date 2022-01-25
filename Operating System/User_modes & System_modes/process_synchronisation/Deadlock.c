/*
To defeat processes waiting for each other in a circular manner we may rank the resiurces as R1,R2......Rn 
and enforce the folllowing rule, which can be checked at run-time:
Processes can only request in a unidirectional order.

If a process has requested Ri, it can only request Rj for j>i
but not for j<=i.

Using this scheme, it is easy to prove (by induction) that there cannnot be any llop in the requests

        DEADLOACK DETECTECTION AND RECOVERY

create a graph as follows:
    Nodes={r1,r2,....,rm,P1,P2,.....Pj}
    Add an arc(r1,Pj) iff ri is allocated to (and held by) Pj
    Add an arc (Pj,rk) iff Pj is waiting for rk


Then every closed loop is a deadlock

Recovery from a deadlock canbe done by aborting some of the processes involved in a deadlock, allowing others to proceed

Prevention in concurrent programs 

    Try to design the algorithm in such a way that every process requests the CRs in a unidirectional chain of P operations.The algorithm will be freee of deadlock since there is no waiting loop

    if cross requests cannot be avoided
            1.Combine Ri and Rj into a single CR whihc reduces concurrency 
            2.whenever we detect a potential deadlock try to release some of the acquired semaphores in order to prevent deadlock, which amounts to defeating the necessary condition C2 


As long as one process backs off there cannot be a deadlock

LIVELOCK

    a set of proesses that run in circles doing nothing useful
    This can be prevented by ensuring only one of the processes actually backs off

STARVATION
        waiting for resources may be blocked indefinitely.
        Prevent this by implementing FIFO.
*/