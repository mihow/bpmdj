/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__overseer_cpp__
#define __loaded__overseer_cpp__
using namespace std;
#include <cstdio>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <stdarg.h>
#include <map>
#include <set>
#include "Active/lock.h"
#include "overseer.h"

/**
 * @page Processes
 *
 * The process management for the selector is based on an 'overseer' process.
 * This process is necessary to address the following problems:
 * - no data space corruption in the X resources that are allocated 
 *   in the main program after forking and exiting
 * - faster startup, since we don't need to duplicate that much file handles 
 *   and other resources when a fork occurs
 * - the ALSA problem is solved. This problem occurs essentially because 
 *   the alsalib does not provide a function to unlink a resource after it
 *   has been forked. As such, when an analysis process is running, which 
 *   during the fork took a copy of the DSP handle that was used during 
 *   fragment playing.. then the selector itself can no longer access the 
 *   DSP driver, or could not close/open it any longer. 
 * - signals no longer get lost, which was still a problem in the old code and 
 *   a potential deadlock situation in the ToSwitchOrNotToSwitch signal 
 *   has also been removed
 *
 * Aside from these advantages. The centralized nature of this overseer process
 * makes it possible for us to directly test a number of conditions which are
 * otherwise difficult to check. See bos.cpp for a number of these tests
 * - multiple threads can concurrently ask to spawn processes.
 * - multiple threads can concurrently retrieve the process state.
 *
 * The overseer itself forks of the main program and remains listening 
 * to requests to spawn a program. In return it will send back the exit code
 * when the child dies. A side effect is as well that the process id's are 
 * assigned by the main client and automatically mapped to the real ids.
 *
 * %To debug applications that are using this overseer, one should resort
 * to starting the application outside the debugger and then connecting
 * to the second BpmDj instance (the real program, not the overseer) with
 *
 *   gdb ./bpmdj <pid>
 */

//#define OVERSEER_LOG
//#define DEBUG_OVERSEER
//#define PROCESS_LOG_START
#define PROCESS_LOG_STOP

//------------------------------------------------------------------------
//     Globals declarations & forward class globals
//------------------------------------------------------------------------
static int from_main;
static int to_main;
static int to_overseer;
static int from_overseer;
static int main_pid;
static unsigned int next_child_id=1;
#ifdef LID2PID
extern map<int,int> mainpid2realpid;
#endif
extern map<int,int> realpid2mainpid;
static vector<ExitStatus>* realpidsdied;
extern PollDelay overseer_delay;
extern struct sigaction child_died_signal;
extern struct sigaction pipe_signal;
extern map<int,Process*> processes;
extern Lock pulling;
class ForkQuick;
extern ForkQuick order_matters;

vector<ExitStatus>* take_death_pids()
{
  vector<ExitStatus>* result;
  do
    {
      result=realpidsdied;
    }
  while (cmpxchg(&realpidsdied,realpidsdied,0)!=result);
  return result;
}

/**
 * This is tricky since there can only be one signal handler 
 * processing the data, meaning that if we return it, the old
 * value should always have been NULL
 */
void return_death_pids(vector<ExitStatus>* r)
{
  vector<ExitStatus>* res=cmpxchg(&realpidsdied,NULL,(unsigned long)(void*)r);
  assert(res==NULL);
}

void process_death_pids()
{
  vector<ExitStatus>* dp=take_death_pids();
  if (!dp) return;
  for(vector<ExitStatus>::iterator it=dp->begin(); it!=dp->end(); it++)
    {
      ExitStatus es=*it;
      int rpid=es.pid;
      int status=es.status;
      // printf("process %d died\n",rpid);
      if (rpid==main_pid)
	{
	  /**
	   * If the main process receives a SIGINT it indicates that it 
	   * was interrupted in a debugger, I believe
	   */
	  if (es.interrupted())
	    {
	      printf("Overseer: main process was interrupted.\n");
	      exit(127);
	    }
	  else if (es.exited())
	    {
#ifdef DEBUG_OVERSEER
	      printf("Overseer: main process (%d) died, "
		     "quitting as well with %d\n",rpid,es.exit());
#endif
	      exit(es.exit());
	    }
#ifdef DEBUG_OVERSEER
	  else
	    {
	      printf("Overseer: main process (%d) died, "
		     "quitting as well with %s",rpid,es.statusText());
	      fflush(stdout);
	      exit(127);
	    }
#endif
	}
      else if (realpid2mainpid.find(rpid)==realpid2mainpid.end())
	cerr<< "Overseer: unknown process died\n";
      else
	{
	  int mpid=realpid2mainpid[rpid];
	  realpid2mainpid[rpid]=0;
#ifdef LID2PID
	  mainpid2realpid[mpid]=0;
#endif
	  assert(mpid);
	  char tmp[4096];
	  sprintf(tmp,"%d %d",mpid,status);
#ifdef OVERSEER_LOG
	  printf("Overseer stopped %d [%s]\n",mpid,es.statusText());
#endif
	  int tw=strlen(tmp)+1;
	  int w=write(to_main,tmp,tw);
	  if (w==-1)
	    {
	      if (errno==SIGPIPE)
		printf("Overseer: main process no longer exists "
		       "to write data too\n");
	      else
		assert(0);
	    }
	  else
	    assert(w==tw);
	}
    }
  delete dp;
}

void spawn_request(char* str)
{
  // first obtain the number 
  int i=0;
  while(str[i])
    {
      if (str[i]==' ')
	str[i]=0;
      else
	i++;
    }
  int main_calls_this=atol(str);
  str+=i+1;
  
  int child;
  child=fork();
  assert(child!=-1);
#ifdef LID2PID
  mainpid2realpid[main_calls_this]=child;
#endif
  realpid2mainpid[child]=main_calls_this;
  
  if (child==0)
    {
      /*int status = */
      execl("/bin/sh","/bin/sh","-c",str,(char*)NULL);
      assert(0);
    }
#ifdef OVERSEER_LOG
  printf("Overseer started %d: %s\n",main_calls_this,str);
#endif
}

void process_main_request(char* str)
{
  assert(strlen(str)>6);
  if (strncmp(str,"Start ",6)==0)
    spawn_request(str+6);
  else
    {
      cerr << "Overseer: ignoring malformed request: " << str << "\n";
    }
}

void check_death_pids()
{
  process_death_pids();
  overseer_delay.wait();
}

void listen_to_main_requests()
{
  char buf[4096];
  int i=0;
  do
    {
      int r = read(from_main,buf+i,1);
      if (r==-1)
	{
	  if (errno==EAGAIN || errno==EINTR)
	    {
	      check_death_pids();
	      continue; 
	    }
	  printf("Overseer: error during pipe read was %d: %s",
		 errno, strerror(errno));
	  assert(r!=-1);
	}
      if (r==1)
	{
	  assert(i<4096);
	  overseer_delay.reset();
	  if (buf[i]==0)
	    {
	      process_main_request(buf);
	      i=0;
	    }
	  else
	    i++;
	}
      else
	check_death_pids();
    }
  while(true);
}

void overseer_child_died(int sig, siginfo_t *info, void* hu)
{
  ExitStatus es;
  while(true)
    {
      es.pid=waitpid(-1,&es.status,WNOHANG);
      if (es.pid==-1)
	{
	  if (errno==ECHILD) return;
	  printf("Overseer: error %d during wait: %s\n",errno,strerror(errno));
	}
      else if (es.pid==0)
	return;
#ifdef DEBUG_OVERSEER
      printf("Overseer: real child %d died with signal %d\n",es.pid,sig);
      fflush(stdout);
#endif
      vector<ExitStatus>* dp=take_death_pids();
      if (dp==NULL) dp=new vector<ExitStatus>();
      dp->push_back(es);
      return_death_pids(dp);
    }
}

//------------------------------------------------------------------------
//     Main side
//------------------------------------------------------------------------
void send_overseer_request(char* str)
{
  write(to_overseer,str,strlen(str)+1);
}

bool poll_death()
{
  // get all pids that are reported to be death.
  vector<ExitStatus> exits;
  bool res=false;
  {
    Synchronized(pulling);
    static char buf[4096];
    static int i=0;
    fcntl(from_overseer,F_SETFL,O_NONBLOCK);
    do
      {
	int r = read(from_overseer,buf+i,1);
	if (r==-1)
	  {
	    if (errno==EINTR) break;
	    if (errno==EAGAIN) break;
	    printf("Main: error during pipe read from overseer was %d: %s", 
		   errno, strerror(errno));
	    assert(r!=-1);
	  }
	if (r==1)
	  {
	    assert(i<4096);
	    if (buf[i]==0)
	      {
		ExitStatus es;
		sscanf(buf,"%d %d",&es.pid,&es.status);
		exits.push_back(es);
		i=0;
		res=true;
	      }
	    else
	      i++;
	  }
	else break;
      }
    while(true);
  }
  // now post the signals through
  for(unsigned i = 0 ; i < exits.size(); i++)
    {
      ExitStatus es=exits[i];
      if (processes.find(es.pid)!=processes.end())
	{
	  processes[es.pid]->died(es);
	  processes[es.pid]=NULL;
	}
    }
  return res;
}

int request_spawn(const char* str, Process* target)
{
  char tmp[4096];
  assert(strlen(str)<4000);
  int pid=next_child_id++;
  if (target) processes[pid]=target;
  sprintf(tmp,"Start %d %s",pid,str);
  send_overseer_request(tmp);
  return pid;
}

int spawn(const char* str, const char* description, Process* target)
{
  int p= request_spawn(str,target);
#ifdef PROCESS_LOG_START
  if (target)
    printf("Process-Start %d: %s\n",p,description); 
  else
    printf("Process-Start %d: %s [without exit receiver]\n",p,description); 
#endif
  return p;
}

int spawn(const char* str, const char* descr)
{
  return spawn(str,descr,NULL);
}

void Process::command(const char* script, const char* desc)
{
  if (cmd) free(cmd);
  cmd=strdup(script);
  if (description) free(description);
  description=strdup(desc);
}

void Process::spawn()
{
  assert(cmd);
  assert(!exitstatus.pid);
  exitstatus.pid=::spawn(cmd,description,this);
}

/**
 * Performs a synchronous execution of the exec process
 */
ExitStatus exec(const char* script, const char*description)
{
  Process *p=new Process();
  p->exec(script,description);
  ExitStatus t=p->exitstatus;
  delete p;
  return t;
}

ExitStatus Process::exec()
{
  assert(cmd);
  assert(!exitstatus.pid);
  exitstatus.pid=::spawn(cmd,description,this);
  PollDelay exec_waiter(100,5000,1000000);
  while(!_died)
    {
      poll_death();
      exec_waiter.wait();
    }
  return exitstatus;
}

ExitStatus Process::exec(const char* e, const char* d)
{
  command(e,d);
  return exec();
}

void Process::spawn(const char* str, const char* desc)
{
  command(str,desc);
  spawn();
}

void Process::vcommand(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  command(toexecute,toexecute);
}

void Process::died(ExitStatus s)
{
  _died=true;
  exitstatus=s;
#ifdef PROCESS_LOG_STOP
  char* stat=s.statusText();
#ifdef PROCESS_LOG_START
  printf("Process-Stop %d: %s [%s]\n",s.pid,description,stat); 
#else
  printf("Process: %s [%s]\n",description,stat); 
#endif
#endif
}

//--------------------------------------------------------------------------
//     Initialization
//--------------------------------------------------------------------------
void init_process_overseer()
{
  int overseer2main[2];
  int main2overseer[2];
  int err;
  err=pipe(overseer2main);
  to_main=overseer2main[1];
  from_overseer=overseer2main[0];
  assert(err!=-1);
  err=pipe(main2overseer);
  to_overseer=main2overseer[1];
  from_main=main2overseer[0];
  assert(err!=-1);
  /**
   * We need to be able to catch the death signal of the main 
   * before we fork, afterward we might be too late
   */
  child_died_signal.sa_sigaction=overseer_child_died;
  child_died_signal.sa_flags=SA_SIGINFO | SA_NOCLDSTOP;
  pipe_signal.sa_handler=SIG_IGN;
  sigaction(SIGCHLD,&child_died_signal,NULL);
  sigaction(SIGPIPE,&pipe_signal,NULL);
  main_pid=fork();
  assert(main_pid!=-1);
  if (main_pid==0)
    {
      /**
       * We are the child and should simply continue with 
       * the rest of the program. 
       */
      close(to_main);
      close(from_main);
    }
  else
    {
      /**
       * This is the overseer process
       */
      close(to_overseer);
      close(from_overseer);
      fcntl(from_main,F_SETFL,O_NONBLOCK);
      listen_to_main_requests();
    }
}

class ForkQuick
{
public:
  ForkQuick()
  {
    init_process_overseer();
  }
};

//------------------------------------------------------------------------
//     Globals Objects
//------------------------------------------------------------------------
#ifdef LID2PID
map<int,int> __attribute__ ((init_priority(201)))      mainpid2realpid;
#endif
map<int,int> __attribute__ ((init_priority(201)))      realpid2mainpid;
PollDelay __attribute__ ((init_priority(201)))         overseer_delay(0,5000,250000);
struct sigaction __attribute__ ((init_priority(201)))  child_died_signal;
struct sigaction __attribute__ ((init_priority(201)))  pipe_signal;
map<int,Process*> __attribute__ ((init_priority(201))) processes;
Lock __attribute__ ((init_priority(201)))              pulling;
ForkQuick __attribute__ ((init_priority(202)))         order_matters;
#endif // __loaded__overseer_cpp__
