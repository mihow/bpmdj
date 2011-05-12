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

#ifndef __BORGIV_AO_POOL_H__
#define __BORGIV_AO_POOL_H__
using namespace std;
#line 1 "ao-pool.h++"
#include "ao-scheduler.h"
using namespace std;

class DawnTilDusk
{
public:
  /**
   * Called by the constructor of each active object to obtain 
   * a scheduler for itself
   */
  virtual Scheduler * sunrise(string who) = 0;
  /**
   * Called by the scheduler whenever an active object
   * dies
   */
  virtual void sunset(string who) = 0;
  virtual ~DawnTilDusk()
  {
  };
};

/**
 * The point where each active object will acquire its scheduler and
 * where each active object will be buried
 */
extern DawnTilDusk *aoPool;
#endif
