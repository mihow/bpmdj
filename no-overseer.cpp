/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__no_overseer_cpp__
#define __loaded__no_overseer_cpp__
using namespace std;
#line 1 "no-overseer.c++"
#include <stdlib.h>
#include <stdarg.h>
#include <qapplication.h>
#include "scripts.h"
#include "process.h"
#include "info.h"

/**
 * Performs a synchronous execution of the exec process
 */
ExitStatus exec(const char* script, const char* description)
{
  ExitStatus res;
  Debug("Started %s",script);
  int status = system(script);
  if  (status == -1)
    { 
      Fatal("couldn't execute [fork failed] %s",script);
      return ExitStatus();
    }
  res.pid=0;
  res.status=status;
  return res;
}

int spawn(const char* script, const char* description)
{
  int cid;
  QApplication::flush();
  /**
   * A vfork should work slightly faster
   * We don't care about a new task structure anyway
   */
  while((cid=vfork())==-1)
    {
      Debug("Could not fork process. Trying again in 10 seconds...");
      sleep(10);
    }
  
  if (cid==0)
    {
      // system(script);
      int status = execl("/bin/sh","/bin/sh","-c",script,(char*)NULL);
      Error(false,"Couldn't spawn [execl returned %d] %s",status,script);
      /**
       * If we use exit instead of _exit our own static data structures
       * will be destroyed ! Of course this routine is only necessary
       * when there was an error becoming something else.
       */
      _exit(100);
    }
  Debug("Spawned %s",script);
  return cid;
}
#endif // __loaded__no_overseer_cpp__
