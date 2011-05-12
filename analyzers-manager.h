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
#ifndef ANAL_MGR_H
#define ANAL_MGR_H

#include "basic-process-manager.h"
class Song;

class AnalyzerChanged
{
 public: 
  AnalyzerChanged() {};
  virtual ~AnalyzerChanged() {};
  /**
   * This function is called by the processmanager when slot 'on_slot'
   * becomes available again. This function should be overridden.
   */
  virtual void startAnotherAnalyzer(Song* previous, int on_slot) = 0;
};

class AnalyzersManager:
  public BasicProcessManager
{
  private:
    int    * running_time;              // the current running time
    long   * total_running_time;        // the cummulated running time
    int    * songs_analyzed;            // the amount of stopped songs
    time_t * started_at;                // start_time when running
    int      limit_time;                // the timeout before considering child non responsive
    int      report_time;               // the color die out time
    Song * * songs_under_investigation; // the song under investigation
    AnalyzerChanged * listener;
    virtual void clearId(int id);    
  public:
    AnalyzersManager(int nr, AnalyzerChanged *sel, int timeout, int report);
    virtual ~AnalyzersManager() {};
    virtual void start(int id, Song* song, const char* command);
    virtual void checkSignals();
    virtual float relative_running_time(int slot);
    virtual bool slot_free(int slot);
    virtual float songs_per_second(int slot);
};

#endif
