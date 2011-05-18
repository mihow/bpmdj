/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__player_h__
#define __loaded__player_h__
using namespace std;
#line 1 "player.h++"
#include <Qt/qnamespace.h>
#include <Qt/qtimer.h>
#include <QtGui/qpushbutton.h>
#include "ui-player.h"
#include "player-core.h"

class BpmAnalyzerDialog;

#define TAB_PLAYER     0
#define TAB_BEATGRAPH  2
#define TAB_BPMCOUNTER 3
#define TAB_INFO       6
#define TAB_OPTIONS    7

class Player: 
  public QDialog,
  public Ui_SongPlayer
{
  Q_OBJECT
public:
  friend class PlayingStateChanged;
private:
  QTimer *timer;
  QTimer *finetimer;
  map_data map;
  int mapin_up;
  int mapin_down;
  int map_size;
  int map_scale;
  float4 tempo_fade;
  float4 fade_time;
  signed8 wantedcurrentperiod;
  signed8 savedtargetperiod;
  void setColor(QWidget *button, bool enabled);
  /**
   * Will update the colors of the beatgraph LCD. Red when the song is playing 
   * at the target tempo. Yellow when the tempo is changing and green when 
   * the song is playing at its normal tempo
   */
  void updateTempoColors();
  void redrawCues();
  void normalReached(bool yes);
  void init_tempo_switch_time();
  bool cueKey(QKeyEvent*,int);
  void storeCue(int nr);
  void retrieveCue(int nr);
  void retrieveNextCue();
  void cueShift(signed8 dir);
  void shift_playpos(signed4 direction);
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
  virtual void keyPressEvent ( QKeyEvent * e );
public:
  Player();
public slots:
  virtual void mousePressEvent(QMouseEvent * e);
  virtual void mouseReleaseEvent(QMouseEvent * e);
  virtual void mouseMoveEvent(QMouseEvent * e);
  // screen update and tempo slides
  virtual void timerTick();
  // fade and sync with clock
  virtual void fineTimerTick();
  // change the playing position
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
  // virtual void shiftBack();
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
  virtual void openAbout();
  virtual void tabChanged();
  // functions to have an exclusive sound card selection
  virtual void setAlsa();
  virtual void setOss();
  virtual void setJack();
  virtual void restartCore();
  virtual void customEvent(QEvent * e);
  virtual void startCore(int tries=5);
  virtual void stopCore();
  virtual void initCore();
  // Midi helpers
  virtual void loop(unsigned8 beats);
  virtual void nudge(unsigned8 dir);
};
#endif // __loaded__player_h__
