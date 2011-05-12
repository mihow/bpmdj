/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include <unistd.h>
#include <dirent.h>
#include <qmessagebox.h>
#include <ctype.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include "preferences.h"
#include "about.h"
#include "songselector.logic.h"
#include "history.h"
#include "version.h"
#include "kbpm-dj.h"
#include "basic-process-manager.h"
#include "qsong.h"
#include "config.h"
#include "scripts.h"
#include "memory.h"

BasicProcessManager* processManager = NULL;

void ToSwitchOrNotToSwitchSignal(int sig, siginfo_t *info, void* hu)
{
  int blah;
  waitpid(info->si_pid,&blah,0);
  for(int i = 0 ; i < 4 ; i ++)
    if (processManager->died_pids[i]==-1)
      {
	processManager->died_pids[i] = info->si_pid;
	return;
      }
  assert(0);
}

BasicProcessManager::BasicProcessManager(int count, ProcessChanged* lis)
{
 // the process manager is a singleton
  assert(processManager==NULL);
  assert(lis);
  processManager = this;
  listener = lis;
  // initialise fields
  pid_count = count;
  active_pids = allocate(count,int);
  died_pids = allocate(count,int);
  for(int i = 0 ; i < pid_count ; i ++)
    {
      died_pids[i]=-1;
      active_pids[i]=0;
    }
  // catch signals
  struct sigaction *act;
  act=allocate(1,struct sigaction);
  act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
  act->sa_flags=SA_SIGINFO;
  sigaction(SIGUSR1,act,NULL);
  // ignore sigchlds
  // signal(SIGCHLD,SIG_IGN);
};

void BasicProcessManager::processDied(int pid)
{
  for(int i = 0 ; i < pid_count ; i++)
    if (pid==active_pids[i])
      {
	clearId(i);
	return;
      }
  assert(0);
}

void BasicProcessManager::checkSignals()
{
  int tmp;
  for(int i = 0 ; i < pid_count ; i ++)
    {
      tmp=died_pids[i];
      // printf("%d: Died %d,  Active %d\n",i,tmp,active_pids[i]);
      if (tmp!=-1)
	{
	  died_pids[i]=-1;
	  processDied(tmp);
	}
    }
}

void BasicProcessManager::clearId(int id)
{
  active_pids[id] = 0;
}

void BasicProcessManager::start(int id,const char* command)
{
  // fork the command and once it exists stop immediatelly
  if (!(active_pids[id]=fork()))
    { 
      execute(command);
      kill(getppid(),SIGUSR1);
      exit(0);
    }
}
