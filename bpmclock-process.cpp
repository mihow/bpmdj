/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__bpmclock_process_cpp__
#define __loaded__bpmclock_process_cpp__
using namespace std;
#line 1 "bpmclock-process.c++"
#include "bpmclock.h"
#include <sys/mman.h>
#include <fcntl.h>
#include "info.h"

bool ping_bpmclock()
{
  bpmclock *c;
  int shm_fd;
  timeb now;
  
  ftime(&now);
  if ((shm_fd = shm_open(CLOCK_FILE, O_RDWR, 0)) > 0)
    {
      if ((c = (bpmclock*) mmap(0, sizeof(bpmclock), (PROT_READ | PROT_WRITE),
				MAP_SHARED, shm_fd, 0)) != MAP_FAILED) 
	{
	  ftime(&now);
	  signed8 dt = now.time - c->pingtime.time;
	  if ((dt >= 0) && (dt < 1))
	    {
	      close(shm_fd);
	      munmap(c, sizeof(bpmclock));	      
	      return true;
	    }
	  
	  Log("bpmclock: ", "Process died.");
	  close(shm_fd);
	  munmap(c, sizeof(bpmclock));
	}
      Log("bpmclock: ", "Removing leftover shared memory file.");
      close(shm_fd);
      shm_unlink(CLOCK_FILE);
    }
  return false;
} 
#endif // __loaded__bpmclock_process_cpp__
