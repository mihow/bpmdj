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
#ifndef __loaded__overseer_h__
#define __loaded__overseer_h__
using namespace std;
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>
#include "process.h"

/**
 * This must be called at the beginning of the program. It will
 * fork of a new process. The child will continue with the main
 * operation while the parent will listen to any potential
 * process start requests. 
 */
void init_process_overseer();

/**
 * This function will check whether there is any process that ended its
 * life so far. If so, it will call that process with died()
 * So the thread will jump from the poll_Death function to any of
 * the died calls in a process. This is important for Qt since a random 
 * thread indirectly started through an exec might end up in the
 * user interface thread. As such it is very important to check the
 * process end status independently before proceeding in Qt.
 */
bool poll_death();

class PollDelay
{
  int min;
  int step;
  int max;
  int delay;
public:
  PollDelay(int _min=0, int _step=5000, int _max=1000000): 
    min(_min), step(_step), max(_max), delay(_min)
  {
  }    
  void reset()
  {
    delay=min;
  }
  void wait()
  {
    if (delay) 
      usleep(delay);
    delay+=step;
    if (delay>max) delay=max;
  }
};
#endif // __loaded__overseer_h__
