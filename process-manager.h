/****
 BpmDj v3.8: Free Dj Tools
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
#ifndef __loaded__process_manager_h__
#define __loaded__process_manager_h__
using namespace std;
#line 1 "process-manager.h++"
#include "basic-process-manager.h"

class ProcessChanged
{
 public:
  virtual ~ProcessChanged() {};
  virtual void resetCounter() {};
  virtual void updateProcessView() {};
};

class SongSelectorLogic;
class Song;

extern Song* main_song;
extern Song* monitor_song;
extern vector<Song*> songs_to_avoid;

class ProcessManager:
  public BasicProcessManager
{
  public:
    SongSelectorLogic * selector;
    ProcessChanged* listener;
  private:
    inline SongSelectorLogic* get_selector() { return selector; };
    virtual void clearId(int id);
    void start(int id, Song * song);
  public:
    ProcessManager(SongSelectorLogic *sel);
    virtual ~ProcessManager() {};
    void switchMonitorToMain();
    void setMainSong(Song * song);
    bool startSong(Song *song);
#ifdef INCOMPLETE_FEATURES
    void startExtraSong(int id, Song *song);
#endif
};
#endif // __loaded__process_manager_h__
