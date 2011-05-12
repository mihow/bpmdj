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

#ifndef __BORGIV_AO_SCHEDULER_H__
#define __BORGIV_AO_SCHEDULER_H__
using namespace std;
#line 1 "ao-scheduler.h++"
#include "lock.h"

/**
 * @brief has a virtual run method.
 * 
 * A runnable class has a name and a run method. Is is used in the scheduler
 * logic to actually activate objects when necessary. It is further used
 * to detect whether the runnable can actually still run.
 */
class Runnable
{
 public:
  /**
   * name of the runnable object
   */
  string name;
  /**
   * @see ActiveObject::run(bool)
   */
  virtual bool run(bool continuous) = 0;
  /**
   * should return true when the runnable has been terminated.
   */
  virtual bool terminated() = 0;
  Runnable(string name) : name(name)
  {
  }
  virtual ~Runnable()
  {
  };
};

/**
 * @brief has a virtual start method
 *
 * A scheduler is an abstract class that can 
 * start a runnable object.
 */
class Scheduler
{
public:
  /**
   * This method should start a (virtual) thread
   * and return as soon as possible. It is normally called from
   * within the activate method.
   */
  virtual void start(Runnable * who) = 0;
  virtual ~Scheduler()
  {
  };
};

/**
 * @brief starts a thread for each start request.
 * 
 * This scheduler will start a thread for every time the
 * active object is activated.
 */
class SpawnWhenActivated: public Scheduler
{
 public:
  virtual void start(Runnable * torun);
};
#endif
