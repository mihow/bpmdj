/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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

#include "config.h"

class ProcessManager
{
  public:
    // WVB -- TOFIX: maybe we should make this whole class static 
    int player_pids[4];
    int died_pids[4];
    static QSong* playing_songs[4];
    static double mainTempo;
  private:
    int monitorPlayCommand;
    double monitorTempo;
    void processDied(int pid);
    SongSelectorLogic* selector;
  public:
    static inline QSong* playingInMain() {return playing_songs[0];}
    ProcessManager(SongSelectorLogic *sel);
    void clearPlayer(int id, bool update=true);
    void switchMonitorToMain();
    void setMainSong(QSong * song);
    void checkSignals();
    void startSong(QSong *song);
    void startExtraSong(int id, QSong *song);
};
