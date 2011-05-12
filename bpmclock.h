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
#ifndef __loaded__bpmclock_h__
#define __loaded__bpmclock_h__
using namespace std;
#line 1 "bpmclock.h++"
#include "period-type.h"
#include <semaphore.h>
#include <sys/timeb.h>

#define CLOCK_FILE "/bpmclock"
#define TICK_TIME 100

typedef struct 
{
  quad_period_type currentperiod;
  unsigned8 start;
  unsigned8 currentframe;
  unsigned8 last_access;
  timeb pingtime;
  sem_t sem;
  bool master;
} bpmclock;

bool ping_bpmclock();
#endif // __loaded__bpmclock_h__
