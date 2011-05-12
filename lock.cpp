/* Automatically generated file, please edit lock.c++ */
/****
 Borg IV
 Copyright (C) 2006-2007 Werner Van Belle

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

using namespace std;
#line 1 "lock.c++"
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <time.h>
#include "lock.h"

bool Lock::try_lock()
{
  if (locked) return false;
  // try to set it to what we want 
  void * before = (void*)locked;
  if (cmpxchg(&locked,locked,this)==before)
    return true;
  return false;
}

void Lock::wait_lock()
{
  time_t start_time=time(NULL);
  while (!try_lock()) 
    if ((time(NULL)-start_time)>60)
      assert(0);
}

void Lock::unlock()
{
  assert(locked);
  locked=NULL;
}

