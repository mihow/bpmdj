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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>
#include <qmessagebox.h>
#include <unistd.h>
#include "index.h"
#include "scripts.h"
#include "stdarg.h"
#include "memory.h"

char *getRawFilename(const char * rawpath, const char * n)
{
  char d[2048];
  sprintf(d,"%s/%s.raw",rawpath,basename((char*)n));
  return (char*)strdup(d);
}

FILE * openRawFile(Index* index, const char * rawpath)
{
  FILE * raw;
  char * name = getRawFilename(rawpath, index->get_filename());
  assert(name);
  raw = fopen(name,"rb");
  deallocate(name);
  return raw;
}

FILE * openRawFileForWriting(Index* index, const char *d)
{
  FILE * raw;
  char * name = getRawFilename(d,index->get_filename());
  assert(name);
  raw = fopen(name,"r+b");
  deallocate(name);
  return raw;
}

void removeRaw(Index* index, char* d)
{
  char * name = getRawFilename(d,index->get_filename());
  remove(name);
  deallocate(name);
}

void removeAllRaw(const char* d)
{
  vexecute(RM"%s/*"RAW_EXT,d);
}

void dumpAudio(const char* fname, unsigned4 *buffer, long length)
{
  FILE * tmp = fopen(fname,"wb");
  writesamples(buffer,length,tmp);
  fclose(tmp);
}


void spawn(const char* script)
{
  if (!fork())
    {
      execute(script);
      /**
       * If we use exit instead of _exit our own static data structures
       * will be destroyed !
       */
      _exit(100);
    }
}

int execute(const char* script)
{
  if (system(script)<=256) 
    {
      Debug("started %s",script);
      return 1;
    }
  // we cannot simply make this true because then we might end up using the
  // wrong thrzead to access graphics, with disaster as a result.
  Error(false,"couldn't execute %s",script);
  return 0;
}

int vexecute(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  return execute(toexecute);
}

FILE* openScriptFile(const char* name)
{
  FILE * script = fopen(name,"wb");
  assert(script);
  fprintf(script,SHELL_HEADER);
  return script;
}

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
  vsprintf(toexecute,script,ap);
  va_end(ap);
  Log("Information: ", toexecute);
};

void Debug(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  Log("Debug: ", toexecute);
};

void Error(bool ui, const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  Log("Error: ", toexecute);
  if (ui)
    QMessageBox::critical(NULL,"Error",toexecute,QMessageBox::Ok, QMessageBox::NoButton);
};

void Remote(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  Log("Remote: ", toexecute);
};
