/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>
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
  if (!raw)
    {
      printf("Error: Unable to open %s\n",name);
      exit(3);
    }
  deallocate(name);
  return raw;
}

FILE * openRawFileForWriting(Index* index, const char *d)
{
  FILE * raw;
  char * name = getRawFilename(d,index->get_filename());
  assert(name);
  raw = fopen(name,"r+b");
  if (!raw)
    {
      printf("Error: Unable to open %s\n",name);
      exit(3);
    }
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
      exit(100);
    }
}

int execute(const char* script)
{
  if (system(script)<=256) 
    {
      // printf("Information: started %s\n",script);
      return 1;
    }
  printf("Error: couldn't execute %s\n",script);
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
