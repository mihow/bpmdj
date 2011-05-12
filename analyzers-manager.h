/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__analyzers_manager_h__
#define __loaded__analyzers_manager_h__
using namespace std;
#line 1 "analyzers-manager.h++"
#include "basic-process-manager.h"
class Song;
class SongProcess;

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
    int      limit_time;                // the timeout before considering child non responsive
    AnalyzerChanged * listener;
    virtual void clearId(int id);
  public:
    AnalyzersManager(int nr, AnalyzerChanged *sel, int timeout);
    virtual ~AnalyzersManager() {};
    virtual void start(int id, Song* song, const char* command);
    virtual void checkSignals();
    void songKilled(SongProcess*);
    virtual bool slot_free(int slot);
};
#endif // __loaded__analyzers_manager_h__
