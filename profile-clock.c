#include <stdlib.h>
#include <sys/times.h>

/**
 * since the clock_freq is wrong on some machines we need to test this
 * at compile time.
 */

int main(int argc, char**argv)
{
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
   return 0;
}
