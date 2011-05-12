#include <stdlib.h>
#include <sys/times.h>

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
	printf("#error CLOCK_FREQ could not be determined = %d\n",stop);
	return;
     }
   printf("#define CLOCK_FREQ %d\n",stop);
   clock_freq=stop;
   // measure nanosleeptime..
   // we measure how many full seconds we need to sleep to reach 5 seconds...
   
   start=times(NULL);
   loops=0;
/*   while(times(NULL)-start < clock_freq*5)
     {
	struct timespec req;
	struct timespec rem;
	req.tv_sec=0;
	req.tv_nsec=999999999;
	nanosleep(&req,&rem);
	loops++;
     }
 */  
   printf("// loops = %d\n",loops);
   return 0;

}
