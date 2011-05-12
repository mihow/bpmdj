/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__bpmclock_cpp__
#define __loaded__bpmclock_cpp__
using namespace std;
#line 1 "bpmclock.c++"
#include "bpmclock.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "version.h"
#include "info.h"

int main(int argc, char *argv[])
{
  bpmclock *c;
  int shm_fd;
  
  printf(
  "BpmDj Clock Process v"VERSION"\n"
  "Copyright (c) 2009-2010 Bernard Fortz / Werner Van Belle\n"
  "This software is distributed under the GPL2 license. See copyright.txt\n\n");
  
  if (ping_bpmclock()) 
    {
      Log("bpmclock: ", "another process running.");
      exit(0);
    }
  
  shm_fd = shm_open(CLOCK_FILE, (O_CREAT | O_EXCL | O_RDWR),
		    (S_IREAD | S_IWRITE));
	
  /* Set the size of the memory segment and connect the pointer */
  ftruncate(shm_fd, sizeof(bpmclock));
  
  c = (bpmclock*) mmap(0, sizeof(bpmclock), (PROT_READ | PROT_WRITE),
		       MAP_SHARED, shm_fd, 0);
  
  c->currentperiod = 0;
  c->currentframe = c->last_access = 0;
  c->master = false;
  sem_init(&c->sem, 0, 1);
  
  
  /* we want realtime scheduling */
  int priority = sched_get_priority_max(SCHED_FIFO);
  sched_param sp;
  sp.sched_priority = priority;
  sched_setscheduler(getpid(), SCHED_FIFO, &sp);
  ftime(&c->pingtime);
  c->start = (double) 44.1*(c->pingtime.time*1000+c->pingtime.millitm);
  mlockall(MCL_FUTURE | MCL_CURRENT);

  while (1) 
    {
     int ret;
     usleep(TICK_TIME);
     ret = sem_trywait(&c->sem);
     if (ret == 0) 
       {
	 ftime(&c->pingtime);
	 c->currentframe = (double) 44.1*(c->pingtime.time*1000
					  +c->pingtime.millitm)-c->start;
	 
	 if (c->currentframe > c->last_access + 44100*10) 
	   {
	     Log("bpmclock: ", "No client left, exiting.");
	     close(shm_fd);
	     munmap(c, sizeof(bpmclock));
	     shm_unlink(CLOCK_FILE);
	     exit(0);
	   }
       }
     sem_post(&c->sem);
    }
}
#endif // __loaded__bpmclock_cpp__
