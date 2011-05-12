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

#include <qtimer.h>
#include <qbutton.h>
#include "songplayer.h"

class BpmCountDialog;

class SongPlayerLogic : 
  public SongPlayer
{
   Q_OBJECT
 private:
   QTimer *timer;
   int tempo_fade;
   int fade_time;
   void setColor(QButton *button, bool enabled);
   void redrawCues();
   void normalReached(bool yes);
   BpmCountDialog * bpmcounter;
 protected:
   virtual void done(int r);
 public:
   SongPlayerLogic::SongPlayerLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0);
 public slots:
   // screen update and tempo slides
   virtual void timerTick();
   // phase shifts
   virtual void nudgePlus();
   virtual void nudgeMinus();
   virtual void nudgePlus1M();
   virtual void nudgeMinus1M();
   virtual void nudgePlus4M();
   virtual void nudgeMinus4M();
   virtual void nudgePlus8M();
   virtual void nudgeMinus8M();
   virtual void nudgePlusB();
   virtual void shiftBack();
   virtual void restart();
   // cues
   virtual void setCue();
   virtual void retrieveZ();
   virtual void retrieveX();
   virtual void retrieveC();
   virtual void retrieveV();
   virtual void storeZ();
   virtual void storeX();
   virtual void storeC();
   virtual void storeV();
   // cue shifting
   virtual void nudgeCueBack();
   virtual void nudgeCueForward();
   virtual void nudgeCueBack8M();
   virtual void nudgeCueForward8M();
   // starting and stopping
   virtual void stop();
   // frequency shifts
   virtual void targetTempo();
   virtual void normalTempo();
   virtual void fastSwitch();
   virtual void mediumSwitch();
   virtual void slowSwitch();
   virtual void targetStep();
   // finish window
   virtual void accept();
   // volumes
   virtual void pcmVolume(int volume);
   virtual void mainVolume(int volume);
   // LFO functions
   virtual void fastSaw();
   virtual void slowSaw();
   virtual void fastPan();
   virtual void slowPan();
   virtual void metronome();
   virtual void normalLfo();
   virtual void breakLfo();
   virtual void slowRevSaw();
   virtual void fastRevSaw();
   virtual void openBpmCounter();
   virtual void openSpectrumAnalyzer();
   virtual void openAbout();
   // loop functionality
   virtual void toggleNoLoop();
   virtual void toggleBeatLoop();
   virtual void toggleMeasureLoop();
   virtual void toggle4MeasureLoop();
};
