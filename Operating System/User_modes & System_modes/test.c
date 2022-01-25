#include <stdio.h>


int *divide2(int *p)
{  
    int *result = *p/2;
    printf("value is == %d\n",*result);
    if(*result != 0){divide2(*result);}
   
}
int main()
{
    int i = 32768;
    divide2(&i);
    
}