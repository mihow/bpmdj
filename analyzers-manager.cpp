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

#include "analyzers-manager.h"
#include "memory.h"

AnalyzersManager::AnalyzersManager(int count, AnalyzerChanged * sel, int timeout, int r) :
  BasicProcessManager(count)
{
  listener = sel;
  running_time = allocate(count,int);
  total_running_time = allocate(count,long);
  started_at = allocate(count,time_t);
  songs_analyzed = allocate(count,int);
  songs_under_investigation = allocate(count, Song*);
  for(int i = 0 ; i < count ; i++)
    {
      running_time[i]=timeout;
      total_running_time[i]=0;
      started_at[i]=0;
      songs_analyzed[i]=0;
      songs_under_investigation[i]=NULL;
    }
  limit_time = timeout;
  report_time = r;
};

void AnalyzersManager::start(int id,Song * song, const char* command)
{
  BasicProcessManager::start(id, command, QString("analyzer#")+QString::number(id+1));
  running_time[id]=0;
  started_at[id]=time(NULL);
  songs_under_investigation[id]=song;
}

float AnalyzersManager::relative_running_time(int slot)
{
  float r = running_time[slot];
  r/=(float)report_time;
  if (r>1.0) r = 1.0;
  return r;
}

float AnalyzersManager::songs_per_second(int slot)
{
  float t = total_running_time[slot];
  if (t==0) return 0;
  float s = songs_analyzed[slot];
  return s/t;
}

void AnalyzersManager::checkSignals()
{
  BasicProcessManager::checkSignals();
  for(int i = 0 ; i < pid_count ; i ++)
    {
      running_time[i]++;
      if (running_time[i]==limit_time)
	clearId(i);
    }
}

bool AnalyzersManager::slot_free(int i)
{
  return (active_pids[i]==0);
}

void AnalyzersManager::clearId(int id)
{
  songs_analyzed[id]++;
  running_time[id]=limit_time;
  total_running_time[id]+=time(NULL)-started_at[id];
  BasicProcessManager::clearId(id);
  Song * song = songs_under_investigation[id];
  songs_under_investigation[id]=NULL;
  listener->startAnotherAnalyzer(song,id);
}
