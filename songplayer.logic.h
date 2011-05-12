/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include "player-core.h"

class BpmAnalyzerDialog;

#define TAB_BEATGRAPH 2
#define TAB_OPTIONS   3 

class SongPlayerLogic : 
  public SongPlayer
{
  Q_OBJECT
 public:
  friend class PlayingStateChanged;
 private:
   QTimer *timer;
   map_data map;
   int mapin_up;
   int mapin_down;
   int map_size;
   int map_scale;
   int tempo_fade;
   int fade_time;
   signed8 wantedcurrentperiod;
   void setColor(QButton *button, bool enabled);
   void redrawCues();
   void normalReached(bool yes);
   BpmAnalyzerDialog * bpmcounter;
   void init_tempo_switch_time();
 protected:
   virtual void done(int r);
   void checkCueNonZero();
   void update_map_pixmaps();
   void update_inmap_pixmap();
   void update_speedmap_pixmap();
   void update_volumemap_pixmap();
   void update_map_scale_box();
   void set_start_stop_text();
   void captionize_according_to_index();
 public:
   SongPlayerLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0);
 public slots:
   virtual void mousePressEvent(QMouseEvent * e);
   virtual void mouseReleaseEvent(QMouseEvent * e);
   virtual void mouseMoveEvent(QMouseEvent * e);
   // screen update and tempo slides
   virtual void timerTick();
   // phase shifts
   virtual void nudgePlus();
   virtual void nudgeMinus();
   virtual void nudgeMinusHalfB();
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
   virtual void start_stop();
   // tempo shifts
   virtual void targetTempo();
   virtual void normalTempo();
   virtual void mediumSwitch();
   virtual void targetStep();
   virtual void tempoChanged();
   void changeTempo(int p);
   // finish window
   virtual void accept();
   // Map functions
   virtual void mapStart();
   virtual void mapStop();
   virtual void mapLoopChange();
   virtual void mapLengthChanged(int i);
   virtual void mapScaleChanged(int i);
   virtual void loadMap();
   virtual void saveMap();
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
   virtual void openRythmAnalyzer();
   virtual void openInfo();
   virtual void openAbout();
   virtual void tabChanged();
   // functions to have an exclusive sound card selection
   virtual void setAlsa();
   virtual void setOss();
   virtual void setBpmMixingDesk();
   virtual void restartCore();
   virtual void customEvent(QCustomEvent * e);
   virtual void startCore();
   virtual void stopCore();
   virtual void initCore();
};
