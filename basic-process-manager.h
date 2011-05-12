/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __loaded__basic_process_manager_h__
#define __loaded__basic_process_manager_h__
using namespace std;
#line 1 "basic-process-manager.h++"
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
    virtual void start(int id, const char* command, QString logname, bool append);
};

void setup_signals();
#endif // __loaded__basic_process_manager_h__
