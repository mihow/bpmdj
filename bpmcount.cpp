/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#include <qapplication.h>
#include <qimage.h>
#include "version.h"
#include "memory.h"
#include "bpm-analyzer-2001.h"
#include "bpm-analyzer-2010.h"
#include "bpm-analyzer-wec.h"
#include "bpm-analyzer-autocor.h"
#include "bpm-analyzer-env.h"
#include "analyzer-progress-qt.h"
#include "files.h"
#include "signals.h"

template float8 normalize_abs_max<float8>(float8*, long);
template float8 find_abs_max<float8>(float8*, long);

//#define WITH_PLOT
int main(int argc, char *argv[])
{
  QApplication app(argc,argv);
  printf("bpmcount v"VERSION" (c) Werner Van Belle 2000-2011\n");
  printf("http://werner.yellowcouch.org/Papers/bpm10/\n");
  printf("-----------------------------------------------------------\n");
  
  if (argc==1)
    {
      printf("Usage: bpmcount [file]*\n\n");
      printf("The fileformat should be raw uncompressed, stereo, 16bit, little endian at 44100 Hz\n");
      printf("The results are written to stderr\n");
    };
  int startbpm=90;
  int stopbpm=180;
  TextualAnalyzerProgress printer;
#ifdef WITH_PLOT
  QAnalyzerProgress plot;
#endif
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
      BpmAnalyzer2010* bpm=new BpmAnalyzer2010(audio,map_length/4,44100,startbpm,stopbpm);
      // BpmAnalyzerWec* bpm=new BpmAnalyzerWec(audio,map_length/4,44100,startbpm,stopbpm);
      // BpmAnalyzer2001* bpm=new BpmAnalyzer2001(audio,map_length/4,startbpm,stopbpm);
      // BpmAnalyzerAutocor* bpm=new BpmAnalyzerAutocor(audio,map_length/4,startbpm,stopbpm);
      // BpmAnalyzerEnv* bpm=new BpmAnalyzerEnv(audio,map_length/4,startbpm,stopbpm);
      
      bpm->attach_progress(&printer);
#ifdef WITH_PLOT
      bpm->attach_plot(&plot);
#endif
      while(bpm->step()) ;
      munmap(audio,map_length);
      fclose(file);
      fprintf(stderr,"%g\t%s\n",bpm->tempo,name);
#ifdef WITH_PLOT
      QImage image(1024,768,QImage::Format_RGB32);
      plot.paint(&image);
      image.save("plot.png");
#endif
    }
}
#endif // __loaded__bpmcount_cpp__
