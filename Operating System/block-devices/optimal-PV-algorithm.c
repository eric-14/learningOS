/*
every proc structure has the added fields dev, blk 
and a buffer pointer.

V operation on a semaphore with waiters 
returns a pointer to the unblocked process PROC

*/


BUFFER *getblk(dev, blk)
{
    1. set running PROC.(dev,blk) = (dev, blk) and PROC.bp =0;
    2. search dev_list for bp=(dev,blk);
    3 . if(found bp){
        if(bp is FREE){
            P(free);            //dec free.valuw by 1; no wait
            take bp out of freelist
            P(bp);
            return bp;

        }
        //bp busy
        P(bp);              //wait in bp.queue
        return PROC.bp;     //return buffer pointer in PROC
    }
    //buffer not in cache ,try to create the needed buffer 
    while(1)
    {
        P(free);
        if(PROC.bp)             //if waited in free.queue
        {return PROC.bp;}       //return buffer pointer in PROC

        BP = FIRST BUFFER REMOVED FROM FREELIST;
        P(bp);                      //LOCK BP

        if(bp dirty ){
            awrite(bp);             //asynchronous write, no wait
            continue;
        }
        reasssign bp to (dev, blk);
        return bp;
    }
}
brelse(BUFFER *bp)
{
    if(bp queue has waiter){
        PROC *p=V(bp);
        p->bp=bp;               // give up to first waiter
        return;
    }
    /*  bp queue has no waiter */
    if(free queue has waiter){
        if(bp is dirty){
            awrite(bp); return;
        }
        //bp has a  free queue[has no waiters] has waiter && bp not dirty
        swapQueue(free, bp);        //bp.queue=0 but swap anyway
        goto(10);
    }
    // no waiter in both bp and free queue
    enter bp into tail of freelist; V(bp); V(free);
}

swapQueue()
{
    let P1  = first waiter in free.queue
    m = number of PROCs in free.queue
    n= number of PROCs in free.queue waiting for the same bp as P1
    k =  number of PROCs in bp.queue

    reassign bp to P1.(dev,blk);
    swap bp.queue with PROCs waiting for the same bp in free.queue
    Adjust sempahore values;
    free.value = -(m+k - n );
    bp.value = -n;
}