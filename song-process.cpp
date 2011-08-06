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
#ifndef __loaded__song_process_cpp__
#define __loaded__song_process_cpp__
using namespace std;
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <assert.h>
#include <qstring.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <set>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "config.h"
#include "history.h"
#include "song-process.h"
#include "scripts.h"
#include "memory.h"
#include "vector-iterator.h"
#include "set-iterator.h"
#include "Active/lock.h"
#include "overseer.h"

static set<SongProcess*> actives;

void SongProcess::checkSignals()
{
  set<SongProcess*> toremove;
  for(set<SongProcess*>::iterator it=actives.begin(); it!=actives.end(); it++)
    {
      if ((*it)->has_died())
	toremove.insert(*it);
    }
  for(set<SongProcess*>::iterator it=toremove.begin(); it!=toremove.end(); it++)
    {
      actives.erase(*it);
      (*it)->process_died();
    }
}

void SongProcess::start(QString command, QString description, bool append)
{
  QString logname=slot->getLogName();
  QString final_command(command);
  if (!logname.isEmpty())
    {
      QString fulllog = "/tmp/"+logname+".bpmdj.log";
      final_command+=QString(" >>")+fulllog+" 2>&1";
      FILE * flog = fopen(fulllog.toAscii().data(),append ? "ab" : "wb");
      assert(flog);
      fprintf(flog,"%s\n<u><b>Command: %s</b></u>\n\n",
	      logname.toAscii().data(),
	      command.toAscii().data());
      fclose(flog);
    }
  
  Process::command(final_command.toAscii().data(),
		   description.toAscii().data());
  actives.insert(this);
  spawn();
}
#endif // __loaded__song_process_cpp__
