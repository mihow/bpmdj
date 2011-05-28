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
#ifndef __loaded__info_cpp__
#define __loaded__info_cpp__
using namespace std;
#line 1 "info.c++"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <stdarg.h>
#include "info.h"

void Log(const char* prefix, const char* text)
{
  char * copy = strdup(text);
  char * current = copy;
  while(current)
    {
      
      char * nextstart = strchr(current,'\n');
      if (nextstart)
	*nextstart=0;
      printf("%s%s\n",prefix,current);
      if (nextstart)
	current=nextstart+1;
      else 
	current=NULL;
    }
  // in order to guarantee descent output buffering and flushing
  // we overwrite the stderr (2) with the standard output (1)
  fflush(stdout);
  fflush(stderr);
  free(copy);
}

void Info(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Information: ", toexecute);
};

void Warning(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Warning: ", toexecute);
};

void Debug(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Debug: ", toexecute);
};

void Fatal(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Fatal: ", toexecute);
  _exit(100);
};

void Error(bool ui, const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Error: ", toexecute);
  /**
   * TODO: We might need to check which thread we are before 
   * blindly jumping into this messagebox
   */
  if (ui)
    QMessageBox::critical(NULL,"Error",toexecute,QMessageBox::Ok, 0, 0);
};
#endif // __loaded__info_cpp__
