/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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


void spawn(const char* script)
{
  if (!bpmdj_fork())
    {
      execute(script);
      /**
       * If we use exit instead of _exit our own static data structures
       * will be destroyed !
       */
      _exit(100);
    }
}

bool execute(const char* script)
{
  Debug("Started %s",script);
  int status = system(script);
  if  (status == -1)
    { 
      Error(false,"couldn't execute [fork failed] %s",script);
      return false;
    }
  if (WIFEXITED(status))
    {
      status = WEXITSTATUS(status);
      return status==0;
    }
  if (WIFSIGNALED(status))
    {
      Error(false,"couldn't execute [signal] %s",script);
      return false;
    }
  if (WCOREDUMP(status))
    {
      Error(false,"couldn't execute [core dumped] %s",script);
      return false;
    }
  Fatal("Unknown exit status %d: %s",status,script);
  return false;
}

bool vexecute(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  return execute(toexecute);
}

int bpmdj_fork()
{
  int fork_thread;
  while((fork_thread=fork())==-1)
    {
      Debug("Could not fork process. Trying again in 10 seconds...");
      fflush(stdout);
      sleep(10);
    }
  return fork_thread;
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

void Fatal(const char* script, ...)
{
  char toexecute[1024];
  va_list ap;
  va_start(ap,script);
  vsprintf(toexecute,script,ap);
  va_end(ap);
  Log("Fatal: ", toexecute);
  _exit(100);
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

// The escaping necessary for ssh to work is ridicoulous..
// An ordinary containement within " ... " does not work, so we must
// escape every character on its own. Especially the & is important
// because this would otherwise spawn a background procses.
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

int start_bpmdj_raw(const char* where, const char* file)
{
  char * w = escape(where);
  char * f = escape(file);
  int  err = vexecute("bpmdjraw %s %s",w,f);
  bpmdj_deallocate(w);
  bpmdj_deallocate(f);
  return err;
}

void start_mkdir(const char* dir)
{
  char * d = escape(dir);
  vexecute("mkdir -p -- %s",d);
  free(d);
}

void start_cp(const char* from, const char* to)
{
  char * a = escape(from);
  char * b = escape(to);
  vexecute("cp -- %s %s",a,b);
  bpmdj_deallocate(a);
  bpmdj_deallocate(b);
}

int start_mv(const char* from, const char* to)
{
  char * a = escape(from);
  char * b = escape(to);
  int err = vexecute("mv -i -- %s %s",a,b);
  bpmdj_deallocate(a);
  bpmdj_deallocate(b);
  return err;
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
  execute("rm -- /tmp/*.bpmdj.log");
}

int start_umount_cdrom(bool eject)
{
  int err = execute("umount "CDROM);
  if (eject) execute("eject "CDROM);
  return err;
}

int start_mount_cdrom()
{
  int err = execute("mount "CDROM);
  return err;
}
#endif // __loaded__scripts_cpp__
