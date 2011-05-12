/****
 Active Objects
 Copyright (C) 2006-2010 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__ao.html

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
#ifndef __loaded__lock_cpp__
#define __loaded__lock_cpp__
using namespace std;
#line 1 "lock.c++"
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <time.h>
#include <iostream>
#include "lock.h"

bool Lock::try_lock(string w)
{
  assert(!deleted);
  if (locked) return false;
  // try to set it to what we want 
  void * before = (void*)locked;
  if (cmpxchg(&locked,locked,this)==before)
    {
      who=w;
#ifdef DEBUG_LOCKING
      cerr << "Locked " << locked << " by " << w << "\n";
#endif
      return true;
    }
#ifdef DEBUG_LOCKING
  cerr << "Tried locked of " << locked << " by " << w << 
    " could not due to " << who << "\n";
#endif
  return false;
}

void Lock::wait_lock(string w)
{
  assert(!deleted);
  time_t start_time=time(NULL);
  while (!try_lock(w)) 
    if ((time(NULL)-start_time)>60)
      {
	cerr << "Cannot obtain lock for " << w << "\n"
	     << "because " << who << " holds it excessively long\n";	
	cerr.flush();
	assert(0);
	_exit(24);
      }
}

void Lock::unlock()
{
  assert(!deleted);
  assert(locked);
  who="";
  locked=NULL;
}

#endif // __loaded__lock_cpp__
