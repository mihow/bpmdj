/****
 Active Objects v4.3
 Copyright (C) 2006-2011 Werner Van Belle
 http://active.yellowcouch.org/

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
#ifndef __loaded__ao_scheduler_cpp__
#define __loaded__ao_scheduler_cpp__
using namespace std;
#line 1 "ao-scheduler.c++"
#include <cstdio>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <time.h>
#include <pthread.h>
#include "ao-scheduler.h"
#include "ao-pool.h"

//---------------------------------------------------------------
//                            Scheduling
//---------------------------------------------------------------
static void * startRunner(void* neglect)
{
  assert(neglect);
  Runnable &info = *(Runnable*)neglect;
#ifdef THREAD_LOGGING
  static int threadcount = 0;
  int tn=++threadcount;
  string name = info.name;
  cout << "Start [" << tn << "]: "<< name << endl;
#endif
  info.run(true);
#ifdef THREAD_LOGGING
  cout << " Stop ["<<tn<<"]: " << name << endl;
  threadcount--;
#endif
  return NULL;
}

void SpawnWhenActivated::start(Runnable * torun)
{
  pthread_t *y = new pthread_t();
  int err = pthread_create(y,NULL,startRunner,(void*)torun);
  if (err)
    {
      cerr << "Could not spawn thread , errno = " << err << endl;
      cerr.flush();
      assert(0);
    };
  pthread_detach(*y);
}
#endif // __loaded__ao_scheduler_cpp__
