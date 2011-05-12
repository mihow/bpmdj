/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

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
#ifndef PROCESS_MGR_H
#define PROCESS_MGR_H

#include "basic-process-manager.h"
#include "config.h"

class ProcessManager:
  public BasicProcessManager
{
  public:
    static Song* *playing_songs;
    SongSelectorLogic * selector;
  private:
    int monitorPlayCommand;
    inline SongSelectorLogic* get_selector() { return selector; };
    virtual void clearId(int id);
  public:
    static inline Song* playingInMain() {return playing_songs[0];};
    static inline bool monitorFree() {return playing_songs[1]==NULL;};
    ProcessManager(SongSelectorLogic *sel);
    void clearPlayer(int id, bool update=true);
    void switchMonitorToMain();
    void setMainSong(Song * song);
    void startSong(Song *song);
    void startExtraSong(int id, Song *song);
};

#endif
