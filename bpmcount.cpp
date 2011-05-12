/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <syscall.h>
#include "version.h"
#include "types.h"
#include "memory.h"
#include "bpm.h"
#include "files.h"
#include "signals-template.cpp"

template double normalize_abs_max<double>(double*, long);
template double find_abs_max<double>(double*, long);

int main(int argc, char *argv[])
{
  printf("bpmcount v"VERSION" (c) Werner Van Belle 2000-2006, all rights reserved\n");
  printf("http://bpmdj.sourceforge.net/\n");
  printf("_________________________________________________________________\n");

  if (argc==1)
    {
      printf("Usage: bpmcount [file]*\n");
      printf("The fileformat should be uncompressed\n");
      printf("stereo, 16bit, little endian at 44100 Hz\n");
      printf("The first run might take a long time due\n");
      printf("to calibration of the FFTW3 library\n");
    };
  int startbpm=60;
  int stopbpm=180;
  for(int i = 1; i < argc; i++)
    {
      char*name=argv[i];
      FILE * file = fopen(argv[i],"rb");
      if(!file)
	{
	  printf("NOFILE\t%s\n",name);
	  continue;
	}
      int fd = fileno(file);
      unsigned4 map_length = fsize(file);
      stereo_sample2 * audio = (stereo_sample2*)mmap(NULL,map_length,PROT_READ,MAP_SHARED,fd,0);
      assert(audio!=MAP_FAILED);
      BpmCounter bc(NULL,audio,map_length/4,44100,startbpm,stopbpm);
      tempo_type result(bc.measure());
      munmap(audio,map_length);
      fclose(file);
      printf("%g\t%s\n",result.tempo,name);
    }
}
