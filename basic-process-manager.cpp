/****
 BpmDj: Free Dj Tools
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

/**
 * The died processes class keeps track of all processes which returned
 * a status. Data can be set only by the signal handler, while data
 * can be cleared only by a process manager. This ensures no concurrency
 * problems.
 */
#define MAX_PROCESSES (50)
class DiedProcesses
{
public:
  int died_pids[MAX_PROCESSES];
  vector<BasicProcessManager*> *listeners;
  bool sigs_installed;
  void init(BasicProcessManager * l);
  void check();
  DiedProcesses() : sigs_installed(false) {};
  ~DiedProcesses() {};
};

void DiedProcesses::check()
{
  if (!sigs_installed) 
    return;
  int tmp;
  for(int i = 0 ; i < MAX_PROCESSES ; i ++)
    {
      tmp=died_pids[i];
      if (tmp!=-1)
	{
	  died_pids[i]=-1;
	  vectorIterator<BasicProcessManager*> listener(listeners); ITERATE_OVER(listener)
	    listener.val()->processDied(tmp);
	  }
	}
    }
}

static DiedProcesses dead_processes;

void ToSwitchOrNotToSwitchSignal(int sig, siginfo_t *info, void* hu)
{
  int blah;
  waitpid(info->si_pid,&blah,0);
  // grmpf ! error here !!!
  for(int i = 0 ; i < MAX_PROCESSES ; i ++)
    if (dead_processes.died_pids[i] == -1)
      {
	dead_processes.died_pids[i] = info->si_pid;
	return;
      }
  printf("Warning: not enough place to store died pids\n");
}

void DiedProcesses::init(BasicProcessManager * l)
{
  if (!sigs_installed)
    {
      sigs_installed = true;
      listeners = new vector<BasicProcessManager*>();
      listeners->reserve(10);
      // clear the dead array
      for(int i = 0 ; i < MAX_PROCESSES ; i ++)
	died_pids[i]=-1;
      // catch signals
      struct sigaction *act;
      act=bpmdj_allocate(1,struct sigaction);
      act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
      act->sa_flags=SA_SIGINFO;
      // sigaction(SIGUSR1,act,NULL);
      sigaction(SIGCHLD,act,NULL);
      // ignore sigchlds
      // signal(SIGCHLD,SIG_IGN);
    };
  listeners->push_back(l);
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
  //  printf("Warning: a process died which I didn't know\n");
  // assert(0);
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
      QString fulllog = "/tmp/"+logname+".kbpmdj.log";
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
