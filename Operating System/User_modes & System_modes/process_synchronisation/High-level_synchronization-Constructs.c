/*
pthread_cond_wait blocks the calling thread on the condition variable which automatically and atomically unlocks the mutex m 

*/

int count;      //shared variable of threads
pthread_mutex_lock(m);      //lock mutex first

if(count is not as expected)
    pthread_cond_wait(con,m);   //wait in con and unlock mutex

pthread_mutex_unlock(m);        //unlock mutex


/*
while a thread is blocked on the condition variable, another thread may use pthread_cond_signal(con) 
to unblock a waiting thread

pthread_lock(m);
    change the shared variabe count;
    if(count reaches a certain value)
        pthread_cond_signal(con);
    
pthread_unlock(m);
*/

/*
Monitors -- are abstract data type which includes shared data objects and all procedures that operate on the shared data objects
.
*/