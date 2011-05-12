/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__basic_process_manager_cpp__
#define __loaded__basic_process_manager_cpp__
using namespace std;
#line 1 "basic-process-manager.c++"
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <assert.h>
#include <qstring.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <set>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "config.h"
#include "history.h"
#include "basic-process-manager.h"
#include "scripts.h"
#include "memory.h"
#include "vector-iterator.h"
#include "set-iterator.h"
#include "lock.h"

/**
 * The died processes class keeps track of all processes which returned
 * a status. Data can be set only by the signal handler, while data
 * can be cleared only by a process manager. This ensures no concurrency
 * problems.
 */
class DiedProcesses: public Lock
{
public:
  set<int> died_pids;
  vector<BasicProcessManager*> *listeners;
  bool sigs_installed;
  void init(BasicProcessManager * l);
  void check();
  void setup_signals();
  DiedProcesses() : sigs_installed(false) {};
  ~DiedProcesses() {};
};

static DiedProcesses dead_processes;
void DiedProcesses::check()
{
  if (!sigs_installed) return;
  set<int> died;
  {
    Synchronized(dead_processes);
    died = died_pids;
    died_pids.clear();
  }
  simpleSetIterator<int> dead(died);
ITERATE_OVER(dead)

    {
      vectorIterator<BasicProcessManager*> listener(listeners);
ITERATE_OVER(listener)

	listener.val()->processDied(dead.val());
      }
    }
  }
}

/**
 * The signaling stuff remains a mess. How many times I have to had
 * to revise this code is unbelieveable. In any case. With Qt4, it seems 
 * that the QtCore support grabs the signal and then calls us thereby 
 * completely ignoring our info parameter. As such, I throw the 
 * fucker out, and install it myself by manually initialising it 
 * after the application is allocated. What effect this will have in the
 * long run with new Qt releases is of course a question.
 */ 
void ToSwitchOrNotToSwitchSignal(int sig, siginfo_t *info, void* hu)
{
  Synchronized(dead_processes);
  if (!info) 
    printf("No info available for signal %d\n",sig);
  else
    {
      int blah;
      waitpid(info->si_pid,&blah,0);
      dead_processes.died_pids.insert(info->si_pid);
    }
}

void DiedProcesses::init(BasicProcessManager * l)
{
  if (!listeners)
    {
      listeners = new vector<BasicProcessManager*>();
      listeners->reserve(10);
    }
  listeners->push_back(l);
}

void DiedProcesses::setup_signals()
{
  assert(!sigs_installed);
  sigs_installed = true;
  // catch signals
  struct sigaction *act;
  act=bpmdj_allocate(1,struct sigaction);
  act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
  act->sa_flags=SA_SIGINFO;
  sigaction(SIGCHLD,act,NULL);
}

void setup_signals()
{
  dead_processes.setup_signals();
}

BasicProcessManager::BasicProcessManager(int count)
{
  // the process manager is a singleton
  dead_processes.init(this);
  // initialise fields
  pid_count = count;
  active_pids = bpmdj_allocate(count,int);
  for(int i = 0 ; i < count ; i ++)
    active_pids[i]=0;
};

void BasicProcessManager::processDied(int pid)
{
  for(int i = 0 ; i < pid_count ; i++)
    if (pid==active_pids[i])
      {
	clearId(i);
	return;
      }
}

void BasicProcessManager::checkSignals()
{
  dead_processes.check();
}

void BasicProcessManager::clearId(int id)
{
  active_pids[id] = 0;
}

void BasicProcessManager::start(int id,const char* command, QString logname, bool append)
{
  QString final_command(command);
  if (!logname.isEmpty())
    {
      QString fulllog = "/tmp/"+logname+".bpmdj.log";
      final_command+=QString(" >>")+fulllog+" 2>&1";
      FILE * flog = fopen(fulllog,append ? "ab" : "wb");
      assert(flog);
      fprintf(flog,"%s\n<u><b>Command: %s</b></u>\n\n",(const char*)logname,command);
      fclose(flog);
    }
  
  const char * tostart(final_command);
  // fork the command and once it exits stop immediatelly 
  if (!(active_pids[id]=bpmdj_fork()))
    { 
      execute(tostart);
      //  kill(getppid(),SIGUSR1);
      /**
       * If we use exit instead of _exit our own static data structures
       * will be destroyed !
       */
      _exit(0);
    }
}
#endif // __loaded__basic_process_manager_cpp__
