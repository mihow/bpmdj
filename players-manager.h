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
#ifndef __loaded__players_manager_h__
#define __loaded__players_manager_h__
using namespace std;
#include "song-process.h"
class Song;
class SongSelectorLogic;

extern Song* main_song;
extern Song* monitor_song;
extern vector<Song*> songs_to_avoid;

class ProcessManager: public DeathProcessListener
{
  private:
    virtual void process_died(SongSlot*);
    void start(SongSlot*, Song * song);
  public:
    virtual ~ProcessManager() {};
    void switchMonitorToMain();
    void setMainSong(Song * song);
    bool startSong(Song *song);
};
#endif // __loaded__players_manager_h__
