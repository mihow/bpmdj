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

#define PLAYCOMMAND1  "kbpm-play -d /dev/dsp2 -x /dev/mixer1 -p 0 0   -m \"%s\" \"%s\""
#define PLAYCOMMAND2  "kbpm-play -d /dev/dsp1 -x /dev/mixer  -p 0 400 -m \"%s\" \"%s\""

class ProcessManager
{
 public:
  // WVB -- TOFIX: maybe we should make this whole class static 
  int monitorpid;
  static double mainTempo;
  Song* playingInMain;
  Song* playingInMonitor;
 private:
  int monitorPlayCommand;
  char* playCommand1;
  char* playCommand2;
  double monitorTempo;
  SongSelectorLogic* selector;
 public:
  ProcessManager(SongSelectorLogic *sel);
  void clearMonitor();
  void switchMonitorToMain();
  void startSong(Song *song);
  char* getPlayCommand(int i);
  void setPlayCommand(int i, char* cmd);
};
