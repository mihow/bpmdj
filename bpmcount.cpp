/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__bpmcount_cpp__
#define __loaded__bpmcount_cpp__
using namespace std;
#line 1 "bpmcount.c++"
#include <stdlib.h>
#include <cstdio>
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
#include "memory.h"
#include "bpm.h"
#include "files.h"
#include "signals.h"

template float8 normalize_abs_max<float8>(float8*, long);
template float8 find_abs_max<float8>(float8*, long);

int main(int argc, char *argv[])
{
  printf("bpmcount v"VERSION" (c) Werner Van Belle 2000-2010,"
	 " all rights reserved\n");
  printf("http://bpmdj.yellowcouch.org/\n");
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
      stereo_sample2 * audio = (stereo_sample2*)mmap(NULL,map_length,
						     PROT_READ,MAP_SHARED,fd,0);
      assert(audio!=MAP_FAILED);
      BpmCounter bc(NULL,audio,map_length/4,44100,startbpm,stopbpm);
      tempo_type result(bc.measure());
      munmap(audio,map_length);
      fclose(file);
      printf("%g\t%s\n",result.tempo,name);
    }
}
#endif // __loaded__bpmcount_cpp__
