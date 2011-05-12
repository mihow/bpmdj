#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

#define SIZE (1024*1024)

unsigned char memory[SIZE];

double measure_time(int block_size)
{
  // laat ons zeggen dat we er 150 keer doormoeten in het totaal. In het eerste
  // geval nemen we eerst 100 keer dezelfde blok, in het latere geval, nemen 
  clock_t start = times(NULL);
  int s;
  for(int i = 0 ; i < 10 ; i++)
    for(int y = 0 ; y < SIZE/block_size ; y++)
      for(int x = 0 ; x < block_size ; x ++)
	s+=memory[x];
  clock_t stop = times(NULL);
  return stop-start;
}

int main(int argc, char* argv[])
{
  for(int i = 1 ; i < 1024 ; i++)
    {
      double t = measure_time(i*1024);
      printf("%d %g\n",i,t);
    }    
}
