/*
when a process forks a new process or exec to a new image, it calls kmalloc(size).
which allocates a piece of free memory of the needed size. If Successful, Kmallox() return s a pointer to the allocated 
memory segment.

It return 0 if no free memory can be allocated 



segment *kmalloc(u16 request_size)
{
    search free memory list for a HOLE with size >= request_size:
    if find such a HOLE at , sat [ai,si]:

        if(si ==  request_size){ //exactly fit 
            delete the hole form free memory list
            return ai;
            }
            //hole size > request_size
            allocate request_size from HIGH end of the hole by
            changing [ai,si] to [ai,si-request_size];

            return ai + si - request_size;      // where the address starts
            }
            return 0;       //no hole big enough

}

kmalloc() tries to allocate memory from the first hole that is big enough to satisfy 
the requested size. It is therefore known as First-Fit Algorithm.
There are variations to this scheme, such a Best-fit and Worst-Fit algorithms


*/