/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__process_h__
#define __loaded__process_h__
using namespace std;
#line 1 "process.h++"
#include<stdlib.h>
#include<assert.h>

struct ExitStatus
{
public:
  int pid;
  int status;
  ExitStatus(): pid(0), status(-1)
  {
  }
  bool valid()
  {
    return pid;
  }
  /**
   * Returns true if the process exited in a normal manner.
   */
  bool exited();
  /**
   * returns an error string if a strange condition 
   * resulted in termination
   */
  bool interrupted();
  char* error();
  char* statusText();
  int exit()
  {
    assert(exited());
    return WEXITSTATUS(status);
  }
};

class Process
{
protected:
  char* cmd;
  char* description;
  bool _died;
public: 
  ExitStatus exitstatus;
  Process(): cmd(NULL), description(NULL), _died(false)
  {
  }
  virtual ~Process()
  {
    if (cmd)
      {
	free(cmd);
	cmd=NULL;
      }
    if (description)
      {
	free(description);
	description=NULL;
      }
  }
  /**
   * sets the command to execute
   */
  void command(const char *script, const char* description);
  void vcommand(const char *script,...);
  void spawn();
  void spawn(const char* script, const char* description);
  void vspawn(const char* script,...);
  ExitStatus exec();
  ExitStatus exec(const char* script, const char* description);
  /**
   * The died virtual will be called from a random thread
   * to notify the termination of the command.
   * BEWARE: the thread is not necessarily the same as the
   * one used to initiate poll_death.
   */
  virtual void died(ExitStatus s);
  bool has_died() const
  {
    return _died;
  }
};

/**
 * Will branch of a new process to execute the provided script. 
 * The return value is a local process id of the child.
 */
int spawn(const char*, const char * descr);
int spawn(const char* str, const char* descr, Process* target);

/**
 * executes the given command and returns the execution status
 */
ExitStatus exec(const char* script, const char* description);
#endif // __loaded__process_h__
