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
#ifndef __loaded__process_cpp__
#define __loaded__process_cpp__
using namespace std;
#line 1 "process.c++"
#include <cstdio>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process.h"
#include "overseer.h"

bool ExitStatus::exited()
{
  return WIFEXITED(status);
}

bool ExitStatus::interrupted()
{
  return WIFSIGNALED(status) && WTERMSIG(status)==SIGINT;
}

char* ExitStatus::statusText()
{
  char result[1024];
  if (WIFEXITED(status))
    {
      int e=WEXITSTATUS(status);
      if (e)
	sprintf(result,"exit %d",e);
      else
	sprintf(result,"success");
    }
  else if (WIFSIGNALED(status))
    sprintf(result,"signal %d",WTERMSIG(status));
  else if (WCOREDUMP(status))
    sprintf(result,"core dumped");
  else if (WIFSTOPPED(status))
    sprintf(result,"stopped %d",WSTOPSIG(status));
  else if (WIFCONTINUED(status))
    sprintf(result,"continued");
  else
    assert(0);
  return strdup(result);
}

char* ExitStatus::error()
{
  if(exited()) return NULL;
  return statusText();
}
#endif // __loaded__process_cpp__
