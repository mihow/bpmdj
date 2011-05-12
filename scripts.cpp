/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__scripts_cpp__
#define __loaded__scripts_cpp__
using namespace std;
#line 1 "scripts.c++"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdarg.h>
#include "index.h"
#include "scripts.h"
#include "memory.h"
#include "bpmplay.h"
#include "overseer.h"

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
  bpmdj_deallocate(name);
  return raw;
}

FILE * openRawFileForWriting(Index* index, const char *d)
{
  FILE * raw;
  char * name = getRawFilename(d,index->get_filename());
  assert(name);
  raw = fopen(name,"r+b");
  bpmdj_deallocate(name);
  return raw;
}

void removeRaw(Index* index, char* d)
{
  char * name = getRawFilename(d,index->get_filename());
  remove(name);
  bpmdj_deallocate(name);
}

void dumpAudio(const char* fname, unsigned4 *buffer, long length)
{
  FILE * tmp = fopen(fname,"wb");
  writesamples(buffer,length,tmp);
  fclose(tmp);
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
  if (ui)
    QMessageBox::critical(NULL,"Error",toexecute,QMessageBox::Ok, 0, 0);
};

void Remote(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  Log("Remote: ", toexecute);
};

/**
 * The escaping necessary for ssh to work is ridiculous..
 * An ordinary containment within " ... " does not work, so we must
 * escape every character on its own. Especially the & is important
 * because this would otherwise spawn a background process.
 */
char * escape(const char * in)
{
  assert(in);
  char escaped[strlen(in)*2+1];
  int c;
  int i=0;
  while((c=*(in++)) && i < 2048)
    {
      if (c=='\n' || c=='&' || c=='\\' || c==' ' || c=='(' || c==')'
       || c=='\'' || c=='`')
	{
	  escaped[i++]='\\';
	}
      escaped[i++]=c;
    }
  assert(i<2048);
  escaped[i]=0;
  return strdup(escaped);
}

void start_mkdir(const char* dir)
{
  char * d = escape(dir);
  char a[2048],b[2048];
  sprintf(a,"Making directory %s",dir);
  sprintf(b,"mkdir -p -- %s",d);
  bpmdj_deallocate(d);
  exec(b,a);
}

void start_cp(const char* from, const char* to)
{
  char * a = escape(from);
  char * b = escape(to);
  char c[2048],d[2048];
  sprintf(c,"Copying %s to %s",from,to);
  sprintf(d,"cp -- %s %s",a,b);
  bpmdj_deallocate(a);
  bpmdj_deallocate(b);
  exec(d,c);
}

int start_mv(const char* from, const char* to)
{
  char * a = escape(from);
  char * b = escape(to);
  char c[2048];
  char d[2048];
  sprintf(c,"Moving %s to %s",from,to);
  sprintf(d,"mv -i -- %s %s",a,b);
  bpmdj_deallocate(a);
  bpmdj_deallocate(b);
  return execute(c,d);
}

void start_rm(const char* what)
{
  char * a = escape(what);
  vexecute("rm -- %s",a);
  bpmdj_deallocate(a);
}

void removeAllRaw(const char* d)
{
  char * a = escape(d);
  vexecute("rm -- %s/*"RAW_EXT,a);
  bpmdj_deallocate(a);
}

void removeAllLog()
{
  execute("Removing logs","rm -- /tmp/*.bpmdj.log");
}

int bpmdjraw(bool synchronous, const char* fname, const char* where)
{
  char *w=escape(where);
  char *f=escape(fname);
  int pid;
  char cmd[2048];
  sprintf(cmd,"bpmdjraw %s %s",w,f);
  bpmdj_deallocate(w);
  bpmdj_deallocate(f);
  if (synchronous)
    pid=execute("BpmDjRaw",cmd);
  else 
    pid=spawn(cmd,"BpmDjRaw");
  return pid;
}

/**
 * Two wrappers to make command line arguments easier
 */
ExitStatus vexec(const char* description, const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  return exec(toexecute,description);
}

/**
 * Two wrappers which will simply return true or false
 */
bool execute(const char* description, const char* script)
{
  ExitStatus status=exec(script,description);
  if (!status.exited())
    {
      char* err=status.error();
      assert(err);
      Error(false,"couldn't execute %s",err);
      free(err);
      return false;
    }
  return status.exit()==0;
}

bool vexecute(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsnprintf(toexecute,1024,script,ap);
  va_end(ap);
  return execute(toexecute,toexecute);
}
#endif // __loaded__scripts_cpp__
