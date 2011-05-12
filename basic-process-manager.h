/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#ifndef BPROCESS_MGR_H
#define BPROCESS_MGR_H

#include "config.h"
#include "growing-array.h"

class DiedProcesses;
class BasicProcessManager
{
  private:
    void processDied(int pid);
    friend class DiedProcesses;
  protected:
    int * active_pids;
    int   pid_count;
    virtual void clearId(int id);
  public:
    BasicProcessManager(int count);
    virtual ~BasicProcessManager() {};
    virtual void checkSignals();
    virtual void start(int id, const char* command) {assert(0);};
    virtual void start(int id, const char* command, QString logname);
};

#endif
