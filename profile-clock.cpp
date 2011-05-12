/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/
#ifndef __loaded__profile_clock_cpp__
#define __loaded__profile_clock_cpp__
using namespace std;
#line 1 "profile-clock.c++"
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h> 

int main(int argc, char**argv)
{
  /**
   * since the clock_freq is wrong on some machines we need to test this
   * at compile time. 
   */
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
  printf("#define CLOCK_FREQ (%d)\n",(int)stop);
  clock_freq=stop;
  start=times(NULL);
  /**
   * The size of the types must be determined now
   */
  return 0;
}
#endif // __loaded__profile_clock_cpp__
