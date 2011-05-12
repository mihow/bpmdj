/****
 BpmDj: Free Dj Tools
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
using namespace std;
#line 1 "analyzers-manager.c++"
#include <qstring.h>
#include "analyzers-manager.h"
#include "config.h"
#include "memory.h"

AnalyzersManager::AnalyzersManager(int count, AnalyzerChanged * sel, int timeout) :
  BasicProcessManager(count)
{
  listener = sel;
  limit_time = timeout;
};

void AnalyzersManager::start(int id,Song * song, const char* command)
{
  BasicProcessManager::start(id, command,
			     Config::analyzers[id].getLogName(),
			     true);
  Config::analyzers[id].start(song);
}

void AnalyzersManager::checkSignals()
{
  BasicProcessManager::checkSignals();
  for(int i = 0 ; i < pid_count ; i ++)
    if (Config::analyzers[i].inc_running_time()==limit_time) 
      clearId(i);
}

void AnalyzersManager::songKilled(SongProcess * song)
{
  for(int i = 0 ; i < 8 ; i ++)
    if (&Config::analyzers[i]==song)
      clearId(i);
}

bool AnalyzersManager::slot_free(int i)
{
  return (active_pids[i]==0);
}

void AnalyzersManager::clearId(int id)
{
  Song * song = Config::analyzers[id].getSong();
  Config::analyzers[id].stop();
  BasicProcessManager::clearId(id);
  listener->startAnotherAnalyzer(song,id);
}

