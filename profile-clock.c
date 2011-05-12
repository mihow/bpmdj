#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h> 

/**
 * since the clock_freq is wrong on some machines we need to test this
 * at compile time.
 */

int main(int argc, char**argv)
{
   int loops;
   int clock_freq;
   clock_t start,stop;
   start=times(NULL);
   sleep(2);
   stop=times(NULL);
   stop = (stop-start)/2;
   if (stop>900 && stop <1100) 
     stop = 1000;
   else if (stop>90 && stop <110)
     stop = 100;
   else
     {
       printf("#error CLOCK_FREQ could not be determined = %d\n",(int)stop);
       return 1;
     }
   printf("#define CLOCK_FREQ %d\n",(int)stop);
   clock_freq=stop;
   // measure nanosleeptime..
   // we measure how many full seconds we need to sleep to reach 5 seconds...
   
   start=times(NULL);
   loops=0;
   printf("// loops = %d\n",loops);
   return 0;
}
