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
#ifndef __loaded__scripts_cpp__
#define __loaded__scripts_cpp__
using namespace std;
#line 1 "scripts.c++"
#include <cstdio>
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
#include "info.h"

char *getRawFilename(const char * rawpath, const char * n)
{
  char d[2048];
  sprintf(d,"%s/%s.raw",rawpath,basename((char*)n));
  return (char*)strdup(d);
}

FILE * openRawFile(Index* index, const char * rawpath)
{
  //  Debug("Opening %s\n",rawpath);
  char * name = getRawFilename(rawpath,
			       index->get_filename().toAscii().data());
  assert(name);
  //  Debug("which became %s\n",name);
  FILE * raw = fopen(name,"rb");
  bpmdj_deallocate(name);
  return raw;
}

FILE * openRawFileForWriting(Index* index, const char *d)
{
  FILE * raw;
  char * name = getRawFilename(d,index->get_filename().toAscii().data());
  assert(name);
  raw = fopen(name,"r+b");
  bpmdj_deallocate(name);
  return raw;
}

void removeRaw(Index* index, char* d)
{
  char * name = getRawFilename(d,index->get_filename().toAscii().data());
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
	  || c=='\'' || c=='`' || c==';')
	{
	  escaped[i++]='\\';
	}
      escaped[i++]=c;
    }
  assert(i<2048);
  escaped[i]=0;
  return strdup(escaped);
}

QString escape(const QString& in)
{
  char* res=escape(in.toAscii().data());
  QString answer(res);
  bpmdj_deallocate(res);
  return answer;
}

void start_mkdir(const QString& dir)
{
  QString d = escape(dir);
  char a[2048],b[2048];
  sprintf(a,"Making directory %s",dir.toAscii().data());
  sprintf(b,"mkdir -p -- %s",d.toAscii().data());
  exec(b,a);
}

void start_cp(const QString& from, const QString& to)
{
  QString a = escape(from);
  QString b = escape(to);
  char c[2048],d[2048];
  sprintf(c,"Copying %s to %s",from.toAscii().data(),to.toAscii().data());
  sprintf(d,"cp -- %s %s",a.toAscii().data(),b.toAscii().data());
  exec(d,c);
}

int start_mv(const QString& from, const QString& to)
{
  QString a = escape(from);
  QString b = escape(to);
  char c[2048];
  char d[2048];
  sprintf(c,"Moving %s to %s",from.toAscii().data(),to.toAscii().data());
  sprintf(d,"mv -i -- %s %s",a.toAscii().data(),b.toAscii().data());
  return execute(c,d);
}

void start_rm(const QString& what)
{
  QString a = escape(what);
  vexecute("rm -- %s",a.toAscii().data());
}

void remove(QString a)
{
  ::remove(a.toAscii().data());
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

int bpmdjraw(bool synchronous, const char* fname, QString where)
{
  return ::bpmdjraw(synchronous,fname,where.toAscii().data());
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
bool execute(QString description, QString script)
{
  ExitStatus status=exec(script.toAscii().data(),
			 description.toAscii().data());
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
