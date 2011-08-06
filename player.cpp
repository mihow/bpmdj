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
#ifndef __loaded__player_cpp__
#define __loaded__player_cpp__
using namespace std;
#include <cstdio>
#include <Qt/qlcdnumber.h>
#include <stdio.h>
#include <unistd.h>
#include <Qt/qgroupbox.h>
#include <Qt/qpushbutton.h>
#include <Qt/qcolor.h>
#include <Qt/qlabel.h>
#include <Qt/qwidget.h>
#include <math.h>
#include <Qt/qslider.h>
#include <assert.h>
#include <Qt/qmessagebox.h>
#include <Qt/qcombobox.h>
#include <Qt/qradiobutton.h>
#include <Qt/qspinbox.h>
#include <Qt/qlineedit.h>
#include <Qt/qcheckbox.h>
#include <sys/times.h>
#include <unistd.h>
#include "version.h"
#include <stdlib.h>
#include "bpmplay.h"
#include <Qt/qfiledialog.h>
#include <Qt/qtabwidget.h>
#include <Qt/qpainter.h>
#include "player.h"
#include "bpm-analyzer-dialog.h"
#include "beatgraph-analyzer.h"
#include "spectrum-analyzer.h"
#include "rhythm-analyzer.h"
#include "memory.h"
#include "dsp-drivers.h"
#include "about.h"
#include "bpmplay-event.h"
#include "clock-drivers.h"
#include "info.h"
#include "dsp-jack.h"
#include "clock-drivers.h"
#include "qt-helpers.h"

/** 
 * BF: tempo fading should not be related with the UI.
 * sync operations now rely on a timer to avoid spawning a new thread,
 * but need a much lower refresh time, so I introduced a second timer.
 *
 */
#ifndef REFRESH_TIME
#define REFRESH_TIME 1000
#endif

#ifndef FINE_REFRESH_TIME
#define FINE_REFRESH_TIME 1000
#endif

#ifdef MIDI
#include "midi-bindings.h"
#endif

void fetch_config_from(PlayerConfig * target, const Player& song_player);
void store_config_into(PlayerConfig * from,   Player* song_player);

void fetch_config_from(PlayerConfig * target, const Player& player)
{
  if (player.raw_directory->text()!=target->get_core_rawpath())
    {
      QMessageBox::information(NULL,".raw file directory",
	"Changes to the temporary storage will only be in effect the next\n"
	"time you start bpmplay with this configuration\n",
	QMessageBox::Ok,QMessageBox::NoButton);
      if (player.raw_directory->text().isEmpty())
	target->set_core_rawpath("./");
      else if (!player.raw_directory->text().endsWith("/"))
	target->set_core_rawpath(player.raw_directory->text()+QString("/"));
      else
	target->set_core_rawpath(player.raw_directory->text());
    }
  
  Capacity cap = get_capacity(player.capacity);
  if (target->get_disabled_capacities()!=cap)
    {
      if (!opt_setup)
	QMessageBox::information(NULL,"Decoder option",
	 "The new decoding capacities will only be in effect the next\n"
         "time you start bpmplay with this configuration\n",
	 QMessageBox::Ok,QMessageBox::NoButton);
      target->set_disabled_capacities(cap);
    }
  
  target->set_player_dsp(0);
  if (player.alsa->isChecked())
    target->set_player_dsp(2);
  if (player.oss->isChecked())
    target->set_player_dsp(1);
  if (player.jack->isChecked())
    target->set_player_dsp(4);

#define BOOL(a) target->set_##a(player.a->isChecked());
#define NR(a) target->set_##a(player.a->value());
#define TEXT(a) target->set_##a(player.a->text());
   NR(dsp_playrate);
   BOOL(dsp_verbose); 
   NR(alsa_latency);
   TEXT(alsa_dev);
   TEXT(oss_dsp);
   BOOL(oss_init_fragments); 
   NR(oss_fragments);
   BOOL(oss_nolatencyaccounting); 
   NR(oss_latency);
   TEXT(jack_dev);
   TEXT(jack_lout);
   TEXT(jack_rout);
#undef TEXT
#undef NR
#undef BOOL
   target->set_player_rms(player.rms_toggle->isChecked());
   target->set_player_rms_target(qtof4(player.rms->text()));
}

void store_config_into(PlayerConfig * from, Player* player)
{
  player->raw_directory->setText(from->get_core_rawpath());
  // clear all selections according to numerical coding
  player->alsa->setChecked(from->get_player_dsp()==2);
  player->oss->setChecked(from->get_player_dsp()==1);
  player->jack->setChecked(from->get_player_dsp()==4);
  if (dsp)
    player->dsp_exactrate->setText(QString::number(dsp->playrate));
  else
    player->dsp_exactrate->setText("<unknown>");
  
#define BOOL(a) player->a->setChecked(from->get_##a())
#define NR(a) player->a->setValue(from->get_##a())
#define TEXT(a) player->a->setText(from->get_##a())
  NR(alsa_latency);
  NR(dsp_playrate);
  BOOL(dsp_verbose); 
  TEXT(alsa_dev);
  TEXT(oss_dsp);
  BOOL(oss_init_fragments);
  NR(oss_fragments);
  BOOL(oss_nolatencyaccounting);
  NR(oss_latency);
  TEXT(jack_dev);     
  TEXT(jack_lout);
  TEXT(jack_rout);
#undef TEXT
#undef NR
#undef BOOL
  player->rms_toggle->setChecked(from->get_player_rms());
  player->rms->setText(QString::number(from->get_player_rms_target()));
  init_capacity_widget(player->capacity,from->get_disabled_capacities());
}

void Player::done(int r)
{
  static int test=0;
  test++;
  assert(test<2);
  // signal any active counter to stop working
  assert(bpmCounter);
  bpmCounter->terminate();
  aoPool->wait_for_finish();
  core_stop();
  core_done();
  // now finish
  QDialog::done(r);
  test--;
}

void Player::init_tempo_switch_time()
{
  // we must know both tempos
  if (targetperiod_metarate <= 0 || normalperiod_metarate <= 0) return;
  // we take the percentage difference
  int percentage = targetperiod_metarate * 100 / normalperiod_metarate;
  if (percentage < 100) percentage = 100 - percentage;
  else percentage -= 100;
  // 1 percent difference is 10 seconds switch time
  int time = percentage * 10;
  // we limit this time to half of the song length
  int total_time = 0;
  if (playing) total_time = playing->get_time_in_seconds();
  total_time/=2;
  // if the song length is known of course
  if (total_time<=0) total_time=180;
  if (time > total_time) time = total_time;
  // we don't want switching times smaller than 10 seconds 
  if (time < 10) time = 10;
  // and not larger than 180s
  if (time > 180) time = 180;
  // finally we set the value
  tempoSwitchTime->setValue(time);
}

Player::Player(): QDialog()
{
  setupUi(this);
  timer = new QTimer(this);
  finetimer = new QTimer(this);
  map = NULL;
  map_size = 0;
  map_scale = 16;
  mapLengthChanged(64);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(REFRESH_TIME);
  connect(finetimer,SIGNAL(timeout()), SLOT(fineTimerTick()));
  finetimer->start(FINE_REFRESH_TIME);
  tempo_fade=-1;
  fade_time=0;
  wantedcurrentperiod=0;
  redrawCues();
  captionize_according_to_index();
  TempoLd->display(100.0*(float4)normalperiod_metarate/(float4)currentperiod_metarate);
  
  // deactivate the non compiled in sections
#ifndef COMPILE_OSS
  osstab->setDisabled(true);
#endif
#ifndef COMPILE_ALSA
  alsatab->setDisabled(true);
#endif
#ifndef COMPILE_JACK
  jacktab->setDisabled(true);
#endif  
  // set colors of tempo change buttons
  init_tempo_switch_time();
  updateTempoColors();
  startStopButton->setFocus();
  connect(tab, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabChanged() ) );
}

void Player::captionize_according_to_index()
{
  QString blah="";
  if (!playing)
    {
      char hostname[200];
      if (gethostname(hostname,200)!=-1)
	blah = QString(hostname)+": ";
      blah += "Setup of ";
      if (arg_config==NULL || arg_config[0]==0) blah+=" standard configuration";
      else blah += QString(arg_config) + " configuration";
    }
  else
    {
      blah = playing->readable_description();
    }
  setWindowTitle(blah);
}

void Player::tabChanged()
{
  if (tab->currentIndex() == TAB_BEATGRAPH)
    beatGraphAnalyzer->activate();
  else
    beatGraphAnalyzer->deactivate();
  if (tab->currentIndex() == TAB_INFO)
    songInfo->updateDataFrom(playing);
  else
    captionize_according_to_index();
  if (tab->currentIndex() == TAB_OPTIONS)
    store_config_into(config,this);
}

void Player::redrawCues()
{
  setColor(PushButton11,cues_metarate[0]>0);
  setColor(PushButton12,cues_metarate[1]>0);
  setColor(PushButton13,cues_metarate[2]>0);
  setColor(PushButton14,cues_metarate[3]>0);
  beatGraphAnalyzer->cuesChanged();
}

void Player::setColor(QWidget *button, bool enabled)
{
   QColor a;
   if (enabled)
     a=QColor(0,255,127);
   else
     a=QColor(255,0,127);
   ::setBackgroundColor(button,a);
}

typedef enum {PlayingAtNormalTempo, 
	      TempoIsChanging,
	      PlayingAtTargetTempo,
	      UnknownTempoState} tempoState;

void setTempoColors(QWidget *button, tempoState state)
{
  QColor a; 
  if (state==PlayingAtNormalTempo) a=QColor(0,255,0);
  else if (state==TempoIsChanging) a=QColor(255,255,0);
  else if (state==PlayingAtTargetTempo) a=QColor(255,0,0);  
  setBackgroundColor(button,a);
}

void Player::updateTempoColors()
{
  tempoState state;
  if (currentperiod_metarate==normalperiod_metarate)
    state=PlayingAtNormalTempo;
  else if (currentperiod_metarate==targetperiod_metarate)
    state=PlayingAtTargetTempo;
  else
    state=TempoIsChanging;
  static tempoState old=UnknownTempoState;
  if (state!=old)
    {
      old=state;
      setColor(switcherButton,state==PlayingAtNormalTempo);
      setTempoColors(beatGraphAnalyzer->currentTempoLcd,state);
    }
}

void Player::storeCue(int nr)
{
  checkCueNonZero();
  cue_store(nr);
  redrawCues();
  startStopButton->setFocus();
}

void Player::retrieveCue(int nr)
{
  cue_retrieve(nr);
  jumpto(0);
  startStopButton->setFocus();
}

void Player::retrieveNextCue()
{
  static int cue_nr=0;
  for(int i = 0 ; i < 4 ; i++)
    {
      int j = (cue_nr+i)%4;
      if (::cues_metarate[j])
	{
	  retrieveCue(j);
	  cue_nr=j+1;
	  return;
	}
    }
}

bool Player::cueKey(QKeyEvent* e, int p)
{
  int bs = e->modifiers() & Qt::KeyboardModifierMask;
  if (bs==0) 
    {
      retrieveCue(p);
      return true;
    }
  else if (bs==Qt::AltModifier)
    {
      storeCue(p);
      return true;
    }
  return false;
}

void Player::keyPressEvent(QKeyEvent * e)
{
  if (tab->currentIndex() == TAB_BEATGRAPH)
    {
#define ACCEPT e->accept(); return;
      switch (e->key())
	{
	case Qt::Key_0: mediumSwitch(); ACCEPT;
	case Qt::Key_1: nudgeMinus1M(); ACCEPT;
	case Qt::Key_2: nudgePlus1M(); ACCEPT;
	case Qt::Key_4: nudgeMinus4M(); ACCEPT;
	case Qt::Key_5: nudgePlus4M(); ACCEPT;
	case Qt::Key_7: nudgeMinus8M(); ACCEPT;	
	case Qt::Key_8: nudgePlus8M(); ACCEPT;
	case Qt::Key_Slash: setCue(); ACCEPT;
	case Qt::Key_Z: if (cueKey(e,0)) ACCEPT;
	case Qt::Key_X: if (cueKey(e,1)) ACCEPT;
	case Qt::Key_C: if (cueKey(e,2)) ACCEPT;
	case Qt::Key_V: if (cueKey(e,3)) ACCEPT;
	case Qt::Key_Minus: nudgeMinus(); ACCEPT;
	case Qt::Key_Plus: nudgePlus(); ACCEPT;
	case Qt::Key_R: slowRevSaw(); ACCEPT;
	case Qt::Key_N: normalLfo(); ACCEPT;
	case Qt::Key_9: 
	case Qt::Key_ParenLeft: nudgeCueBack(); ACCEPT;
	case Qt::Key_6:
	case Qt::Key_ParenRight: nudgeCueForward(); ACCEPT;
	case Qt::Key_Period:
	case Qt::Key_Comma:
	case Qt::Key_Space: start_stop(); ACCEPT;
	case Qt::Key_Enter: retrieveNextCue(); ACCEPT;
	}
    }
  QDialog::keyPressEvent(e);
}

void Player::shift_playpos_metarate(signed4 direction_metarate)
{
  ::metronome->shift_metarate(direction_metarate);
  startStopButton->setFocus();
}

void Player::nudgePlus()
{
  shift_playpos_metarate(-::currentperiod_metarate/(4*128));
}

void Player::nudgeMinus()
{
  shift_playpos_metarate(+::currentperiod_metarate/(4*128));
}

void Player::nudgePlusB()
{
  shift_playpos_metarate(+::currentperiod_metarate/4);
}

void Player::nudgeMinusHalfB()
{
  shift_playpos_metarate(-::currentperiod_metarate/8);
}

void Player::nudgePlus1M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(+::currentperiod_metarate);
  else
    shift_playpos_metarate(+metarate);
}

void Player::nudgeMinus1M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(-::currentperiod_metarate);
  else
    shift_playpos_metarate(-metarate);
}

void Player::nudgePlus4M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(+::currentperiod_metarate*4);
  else
    shift_playpos_metarate(+4*metarate);
}

void Player::nudgeMinus4M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(-::currentperiod_metarate*4);
  else
    shift_playpos_metarate(-4*metarate);
}

void Player::nudgePlus8M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(+::currentperiod_metarate*8);
  else
    shift_playpos_metarate(+8*metarate);
}

void Player::nudgeMinus8M()
{
  if (::currentperiod_metarate.valid())
    shift_playpos_metarate(-::currentperiod_metarate*8);
  else
    shift_playpos_metarate(-8*metarate);
}

void Player::accept()
{
  timer->stop();
  finetimer->stop();
  beatGraphAnalyzer->deactivate();
  done(Accepted);
}

static bool autostarted = false;
void Player::timerTick()
{
  static int no_raw_file_error_box = 10;
  unsigned4 m_diskrate=wave_max();
  if (no_raw_file_error_box-->0)
    {
      // see comments in PlayerConfig::load_ui_position
      if (no_raw_file_error_box==9) 
	config->load_ui_position(this);
#ifndef NO_AUTOSTART
      // should we start playing ?
      if (!autostarted && m_diskrate>(unsigned)(5*diskrate) && dsp->get_paused())
	{
	  autostarted=true;
	  retrieveZ();
	  restart();
	}
#endif
      // error on the disk stuff ?
      if (m_diskrate==0 && no_raw_file_error_box==0 && playing)
	{
	  QMessageBox::information(NULL,"No .raw file",
	     "It seems like there is no .raw file on disk. Check whether the "
	     "bpmdjraw script behaves properly (inspect the console output) "
  	     "or check whether the target directory is valid and accessible "
             "(in the options tab)",
	     QMessageBox::Ok,QMessageBox::NoButton);
	}
    }
  
  unsigned4 m2;
  if (currentperiod_metarate<0)
    m2 = m_diskrate;
  else
    m2 = y_normalise(m_diskrate);
  assert(dsp);
  unsigned4 totaltime=samples2s(m2,dsp->playrate);
  unsigned4 currenttime=samples2s(::y_playrate,dsp->playrate);
  char totalstr[20], currentstr[20];
  sprintf(totalstr,"%02d:%02d",(int)totaltime/60,(int)totaltime%60);
  sprintf(currentstr,"%02d:%02d",(int)currenttime/60,(int)currenttime%60);
  CurrentTimeLCD -> display(currentstr);
  TotalTimeLCD -> display(totalstr);
  // show current tempo
  if (playing)
    {
      float8 T0 = mperiod2bpm(currentperiod_metarate,metarate);
      float8 T1 = mperiod2bpm(normalperiod_metarate,metarate);
      if (currentperiod_metarate<-1) T0=0;
      if (normalperiod_metarate<-1) T1=0;
      CurrentTempoLCD -> display(T0);
      beatGraphAnalyzer->currentTempoLcd -> display(T0);
      NormalTempoLCD -> display(T1);
      TempoLd->display(100.0*(float4)normalperiod_metarate/(float4)currentperiod_metarate);
      updateTempoColors();
    }
  else
    {
      CurrentTempoLCD -> display(0);
      beatGraphAnalyzer->currentTempoLcd -> display(0);
      NormalTempoLCD -> display(0);
    }
}

void Player::fineTimerTick()
{
  if (fade_time>0)
    targetStep();
}

void Player::setCue()
{
  cue_set();
  beatGraphAnalyzer->cuesChanged();
}

void Player::restart()
{
  ::y_playrate=0;
  unpause_playing();
}

void Player::set_start_stop_text()
{
  if (dsp->get_paused())
    startStopButton->setText("Start");
  else
    {
      startStopButton->setText("Stop");
      autostarted=true;
    }
  startStopButton->setFocus();
}

void Player::start_stop()
{
  if (!dsp->get_paused())
    {
      bool cue_wasset = cue_metarate;
      if (!cue_metarate) 
	cue_set();
      pause_playing();
      if (!cue_wasset)
	beatGraphAnalyzer->cuesChanged();
    }
  else jumpto(0);
}

void Player::retrieveZ()
{
  retrieveCue(0);
}

void Player::retrieveX()
{
  retrieveCue(1);
}

void Player::retrieveC()
{
  retrieveCue(2);
}

void Player::retrieveV()
{
  retrieveCue(3);
}

void Player::storeZ()
{
  storeCue(0);
}

void Player::storeX()
{
  storeCue(1);
}

void Player::storeC()
{
  storeCue(2);
}

void Player::storeV()
{
  storeCue(3);;
}

void Player::checkCueNonZero()
{
  if (::cue_metarate==0)
    QMessageBox::warning(this,"Cue Zero ?",
	 "If you want to store the current playing position in a cue,\n"
	 "you must first create a cue. You can do this by selecting\n"
	 "'set Cue' (or pressing '/') at that point. Afterward you can\n"
	 "store it in any of the 4 positions");
}

#define knick 15
void Player::changeTempo(int p)
{
  wantedcurrentperiod = p;
  int pos = tempoSlider->value();
  if (pos <= 0)
    {
      if (pos<-50)
	{	                    // target = 100 - knick <-> 50
	  pos+=50;                  // pos = 0 <-> -50   
	  pos *= 100 - knick - 50;  // pos = 0 <-> (100 - knick - 50) * -50
	  pos /= 50;                // pos = 0 <-> -100 + knick + 50
	  pos += 100 - knick;       // pos = 100 - knick <-> 50
	}
      else 
	{                           // target = 100 <-> 100 - knick
	                            // pos = 0 <-> - 50
	  pos *= knick;             // pos = 0 <-> knick * -50
	  pos /= 50;                // pos = 0 <-> - knick
	  pos += 100;               // pos = 100 <-> 100 - knick
	}
    }
  else if (pos > 0)
    {
      if (pos>50)
	{                           // target = 100 + knick <-> 200
	  pos-=50;                  // pos = 0 <-> 50   
	  pos *= 100 - knick;       // pos = 0 <-> (100 - knick) * 50
	  pos /= 50;                // pos = 0 <-> 100 - knick
	  pos += 100 + knick;       // pos = 100 + knick <-> 200
	}
      else
	{                           // target = 100 <-> 100 + knick
	                            // pos = 0 <-> 50
	  pos *= knick;             // pos = 0 <-> knick * 50
	  pos /= 50;                // pos = 0 <-> knick
	  pos += 100;               // pos = 100 <-> 100 + knick
	}
    }
  p*=100;
  p/=pos;

  ::metronome->changetempo(p);
  TempoLd->display(100.0*(float4)normalperiod_metarate/(float4)currentperiod_metarate);
}

void Player::targetTempo()
{
  tempo_fade = fade_time;
  changeTempo(targetperiod_metarate);
  updateTempoColors();
}

void Player::normalTempo()
{
  tempo_fade = fade_time;
  changeTempo(normalperiod_metarate);
  updateTempoColors();
}

void Player::mediumSwitch()
{
  fade_time = tempoSwitchTime->value();
  savedtargetperiod = currentperiod_metarate;
  tempo_fade=0;
}

void Player::targetStep()
{
  tempo_fade += ((float4) FINE_REFRESH_TIME/1000.);
  if (tempo_fade>fade_time)
    {
      changeTempo(normalperiod_metarate);
      tempo_fade=0;
      fade_time=0;
      updateTempoColors();
      switcherButton->setText("Fade");
      return;
    }
  if (tempo_fade <= 1.)
    {
      updateTempoColors();
    }
  if (tempo_fade>0)
    {
      switcherButton->setText(QString::number((int)(fade_time-tempo_fade)));
      updateTempoColors();
    }
  
  /**
   * We don't change the period linearly !
   * See report 'Stepwise Tempo Changes in BpmDj'
   * at http://werner.yellowcouch.org/Papers/stepwise/index.html
   */
  float4 result = (float4)savedtargetperiod*
    pow((float4)normalperiod_metarate/(float4)savedtargetperiod,
	(float4)tempo_fade/(float4)fade_time);
  changeTempo((int)result);
}

void Player::tempoChanged()
{
  if (wantedcurrentperiod == 0 )
    wantedcurrentperiod = currentperiod_metarate;
  changeTempo(wantedcurrentperiod);
}

void Player::cueShift(signed8 dir)
{
  cue_shift_metarate(dir);
  beatGraphAnalyzer->cuesChanged();
}

void Player::nudgeCueBack()
{
  cueShift(-metarate/80);
}

void Player::nudgeCueForward()
{
  cueShift(+metarate/80); 
}

void Player::nudgeCueBack8M()
{
  cueShift(-8*normalperiod_metarate); 
}

void Player::nudgeCueForward8M()
{
  cueShift(8*normalperiod_metarate);
}

void Player::fastSaw()
{
  lfo_set("Saw",lfo_saw,16,::y_playrate-dsp->latency());
}

void Player::slowSaw()
{
  lfo_set("Saw",lfo_saw,8,::y_playrate-dsp->latency());
}

void Player::fastPan()
{
  lfo_set("Pan",lfo_pan,16,::y_playrate-dsp->latency());
}

void Player::slowPan()
{
  lfo_set("Pan",lfo_pan,8,::y_playrate-dsp->latency()); 
}

void Player::normalLfo()
{
   lfo_set("No",lfo_no,4,::y_playrate-dsp->latency()); 
}

void Player::fastRevSaw()
{
  lfo_set("Reverse saw",lfo_revsaw,16,::y_playrate-dsp->latency());
}

void Player::slowRevSaw()
{
  lfo_set("Reverse saw",lfo_revsaw,8,::y_playrate-dsp->latency()); 
}

void Player::metronome()
{
  if (lfo_get()==lfo_metronome)
    normalLfo();
  else
    lfo_set("Metronome",lfo_metronome,16,::y_playrate-dsp->latency()); 
}

void Player::breakLfo()
{
  lfo_set("Break",lfo_break,1,::y_playrate-dsp->latency());
}

void Player::mapScaleChanged(int i)
{
  if (i == 0) map_scale = 64;
  else if (i == 1) map_scale = 32;
  else if (i == 2) map_scale = 16;
  else if (i == 3) map_scale = 8;
  else if (i == 4) map_scale = 4;
  else assert(0);
  startStopButton->setFocus();
}

void Player::update_map_scale_box()
{
  if (map_scale == 64) mapping_scale->setCurrentIndex(0);
  else if (map_scale == 32) mapping_scale->setCurrentIndex(1);
  else if (map_scale == 16) mapping_scale->setCurrentIndex(2);
  else if (map_scale == 8) mapping_scale->setCurrentIndex(3);
  else if (map_scale == 4) mapping_scale->setCurrentIndex(4);
  else assert(0);
}

void Player::mapLengthChanged(int new_size)
{
  startStopButton->setFocus();
  map_data new_map = bpmdj_allocate(new_size,map_segment);
  int i = 0;
  if (map)
    {
      for(; i < new_size && i <map_size ; i ++)
	{
	  new_map[i].take_from=map[i].take_from;
	  new_map[i].speed_mult=map[i].speed_mult;
	  new_map[i].speed_div =map[i].speed_div;
	  new_map[i].volume = map[i].volume;
	}
      bpmdj_deallocate(map);
    }
  while(i<new_size)
    {
      new_map[i].take_from = i;
      new_map[i].speed_mult = 0;
      new_map[i].speed_div = 0;
      new_map[i].volume = 100;
      i++;
    }
  map_size = new_size;
  map = new_map;
  update_map_pixmaps();
}

void Player::saveMap()
{
  QString s = QFileDialog::getSaveFileName(this,"Save Pattern",
					   "Beat Patterns (*.map)");
  if (s.isNull()) return;
  const char* filename = strdup(s.toAscii().data());
  FILE *f =fopen(filename,"wb");
  int magic = 0x0002;
  // magic number
  fwrite(&magic,4,1,f);
  // the map size
  int nr = map_size;
  fwrite(&nr,4,1,f);
  // the map itself
  for(int i = 0 ; i < nr ; i ++)
    {
      fwrite(&map[i].take_from,sizeof(signed2),1,f);
      fwrite(&map[i].speed_mult,sizeof(signed2),1,f);
      fwrite(&map[i].speed_div,sizeof(signed2),1,f);
      fwrite(&map[i].volume,sizeof(signed2),1,f);
    }
  // continue flag
  nr=0;
  if (atend_continue->isChecked()) nr |= 1;
  // restart flag
  if (atend_restart->isChecked()) nr |= 2;
  // stop flag 
  if (atend_stop->isChecked()) nr |= 4;
  // loop flag
  if (atend_loop->isChecked()) nr |= 8;
  // Z flag
  if (atend_z->isChecked()) nr |= 16;
  // X flag
  if (atend_x->isChecked()) nr |= 32;
  // C flag
  if (atend_c->isChecked()) nr |= 64;
  // V flag
  if (atend_v->isChecked()) nr |= 128;
  // the fragment size
  nr+=(map_scale<<16);
  fwrite(&nr,4,1,f);
  fclose(f);
}

void Player::loadMap()
{
  QString s = QFileDialog::getOpenFileName(this,"Load pattern","",
	   "Beat Patterns (*.map)");
  if (s.isNull()) return;
  const char* filename = strdup(s.toAscii().data());
  FILE *f =fopen(filename,"rb");
  if (!f) return;
  int magic = -1;
  fread(&magic,4,1,f);
  int nr;
  fread(&nr,4,1,f);
  assert(nr>0 && nr<=256);
  mapLengthChanged(nr);
  if (magic == 0)
    for(int i = 0 ; i < nr ; i ++)
       {
	 fread(&map[i].take_from,sizeof(signed2),1,f);
       }
  else if (magic == 1)
    for(int i = 0 ; i < nr ; i ++)
      {
	fread(&map[i].take_from,sizeof(signed2),1,f);
	fread(&map[i].speed_mult,sizeof(signed2),1,f);
	fread(&map[i].speed_div,sizeof(signed2),1,f);
	fread(&map[i].volume,sizeof(signed2),1,f);
      }
  else if (magic == 2)
    {
      for(int i = 0 ; i < nr ; i ++)
	{
	  fread(&map[i].take_from,sizeof(signed2),1,f);
	  fread(&map[i].speed_mult,sizeof(signed2),1,f);
	  fread(&map[i].speed_div,sizeof(signed2),1,f);
	  fread(&map[i].volume,sizeof(signed2),1,f);
	}
      fread(&nr,4,1,f);
      atend_continue->setChecked(nr & 1);
      atend_restart->setChecked(nr & 2);
      atend_stop->setChecked(nr & 4);
      atend_loop->setChecked(nr & 8);
      atend_z->setChecked(nr & 16);
      atend_x->setChecked(nr & 32);
      atend_c->setChecked(nr & 64);
      atend_v->setChecked(nr & 128);
      map_scale = nr>>16;
      update_map_scale_box();
    }
  else assert(0);
  fclose(f);
  mapout_length->setValue(map_size);
  update_map_pixmaps();
}

void Player::mapStart()
{
  map_data mapcopy = bpmdj_allocate(map_size,map_segment);
  // copy all data and take scaling into account...
  for(int i = 0 ; i < map_size ; i ++)
    {
      // location
      mapcopy[i].take_from=map[i].take_from;
      // speed
      float4 factor = pow(2,((float4)map[i].speed_mult)/12.0);
      assert(factor!=0);
      if (factor>=1)
	{
	  mapcopy[i].speed_mult=16000;
	  mapcopy[i].speed_div=(signed2)((float4)16000/factor);
	}
      if (factor<1)
	{
	  mapcopy[i].speed_mult=(signed2)((float4)16000*factor);
	  mapcopy[i].speed_div=16000;
	}
      // volume
      mapcopy[i].volume=map[i].volume;
    }
  // decide the exit
  signed8 mexit;
  if (atend_continue->isChecked())
    mexit = map_exit_continue;
  else if (atend_restart->isChecked())
    mexit = map_exit_restart;
  else if (atend_z->isChecked())
    mexit = metarate_to_diskrate(::cues_metarate[0]);
  else if (atend_x->isChecked())
    mexit = metarate_to_diskrate(::cues_metarate[1]);
  else if (atend_c->isChecked())
    mexit = metarate_to_diskrate(::cues_metarate[2]);
  else if (atend_v->isChecked())
    mexit = metarate_to_diskrate(::cues_metarate[3]);
  else if (atend_stop->isChecked())
    mexit = map_exit_stop;
  else
    assert(0);
  map_set_diskrate(map_size,mapcopy,
		   normalperiod_metarate*diskrate*map_size/(map_scale*metarate),
		   mexit,atend_loop->isChecked());
}

void Player::mapStop()
{
  map_stop();
}

void Player::mapLoopChange()
{
  map_loop_set(atend_loop->isChecked());
}

void Player::openAbout()
{
  About box(1);
}

void Player::mousePressEvent(QMouseEvent * e)
{
  QPoint p = e->pos(); // p is expressed in window coordinates.
  QPoint xy = mapin->mapToParent(mapin->mapFrom(this,p));
  int x = xy.x();
  int y = xy.y();
  if (mapin->geometry().contains(xy))
    { 
      x = x - mapin->x();
      x *= map_size;
      x /= mapin->width();
      assert(x >= 0 && x < map_size);
      mapin_down = mapin_up = x;
      update_map_pixmaps();
    }
  else if (mapspeed->geometry().contains(xy))
    {
      x = x - mapspeed->x();
      x *= map_size;
      x /= mapspeed->width();
      
      y = y - mapspeed->y();
      y *= 24;
      y /= mapspeed->height();
      map[x].speed_mult = 12 - y;
      update_speedmap_pixmap();
    }
  else if (mapvolume->geometry().contains(xy))
    {
      x = x - mapvolume->x();
      x *= map_size;
      x /= mapvolume->width();
      
      y = y - mapvolume->y();
      y *= 100;
      y /= mapspeed->height();
      map[x].volume = 100 - y;
      update_volumemap_pixmap();
    }
  else if (mapout->geometry().contains(xy))
    {
      x = x - mapout->x();
      x *= map_size;
      x /= mapin->width();
      assert(x >= 0 && x < map_size);
      if (e->button()==Qt::LeftButton)
	{
	  int s = mapin_up - mapin_down + 1;
	  if (s > map_size - x ) 
	    s = map_size - x;
	  for(int i = 0 ; i < s ; i++)
	    map[x+i].take_from= i + mapin_down;
	}
      else if (e->button() == Qt::RightButton)
	{
	  map[x].take_from=-1;
	}
      update_map_pixmaps();
    }
  QDialog::mousePressEvent(e);
}

void Player::mouseMoveEvent(QMouseEvent * e)
{
  QPoint p = e->pos();
  QPoint xy = mapin->mapToParent(mapin->mapFrom(this,p));
  int x = xy.x();
  int y = xy.y();
  if (mapin->geometry().contains(x,y))
    {
      x = x - mapin->x();
      x *= map_size;
      x /= mapin->width();
      assert(x >= 0 && x < map_size);
      if (mapin_up != x)
	{
	  mapin_up = x;
	  update_inmap_pixmap();
	}
    }
  QDialog::mousePressEvent(e);
}

void Player::mouseReleaseEvent(QMouseEvent * e)
{
  QPoint p = e->pos();
  QPoint xy = mapin->mapToParent(mapin->mapFrom(this,p));
  int x = xy.x();
  int y = xy.y();
  if (mapin->geometry().contains(x,y))
    {
      x = x - mapin->x();
      x *= map_size;
      x /= mapin->width();
      assert(x >= 0 && x < map_size);
      mapin_up = x;
      update_map_pixmaps();
    }
  QDialog::mouseReleaseEvent(e);
}

int valuetick(int i)
{
  if (i==-1) return 0;
  i%=8;
  if (i == 0) return 255;
  if (i == 1) return 128;
  if (i == 2) return 170;
  if (i == 3) return 128;
  if (i == 4) return 212;
  if (i == 5) return 128;
  if (i == 6) return 170;
  if (i == 7) return 128;
  assert(0);
  return 0;
}

void Player::update_inmap_pixmap()
{
  int h = mapin->height();
  QPixmap mi(map_size,h);
  QPainter pmi;
  pmi.begin(&mi);
  for(int i = 0 ; i < map_size ; i ++)
    {
      QColor in;
      in.setHsv(i * 240 / map_size , 255 , valuetick( i ));
      pmi.setPen(in);
      pmi.drawLine(i,0,i,h-1);
      if (i>=mapin_down && i<=mapin_up)
	{
	  pmi.setPen(Qt::black);
	  pmi.drawPoint(i,0);
	  pmi.drawPoint(i,h-1);
	  pmi.setPen(Qt::white);
	  pmi.drawPoint(i,1);
	  pmi.drawPoint(i,h-2);
	}
    }
  pmi.end(); 
  int sx=mapin->width();
  mapin->setPixmap(mi.scaled(sx,h));
}

void Player::update_speedmap_pixmap()
{
  int h = mapspeed->height();
  QPixmap ms(map_size,h);
  QPainter pms;
  pms.begin(&ms);
  for(int i = 0 ; i < map_size ; i ++)
    {
      pms.setPen(Qt::white);
      pms.drawLine(i,0,i,h-1);
      pms.setPen(Qt::black);
      pms.drawPoint(i,(12-map[i].speed_mult)*mapspeed->height()/24);
    }
  pms.end();
  int sx=mapspeed->width();
  mapspeed->setPixmap(ms.scaled(sx,h));
}

void Player::update_volumemap_pixmap()
{
  int h = mapvolume->height();
  QPixmap mv(map_size,h);
  QPainter pmv;
  pmv.begin(&mv);
  for(int i = 0 ; i < map_size ; i ++)
    {
      pmv.setPen(Qt::white);
      pmv.drawLine(i,0,i,h-1);
      pmv.setPen(Qt::black);
      pmv.drawPoint(i,(100-map[i].volume)*mapspeed->height()/100);
    }
  pmv.end(); 
  int sx=mapvolume->width();
  mapvolume->setPixmap(mv.scaled(sx,h));
}

void Player::update_map_pixmaps()
{
  update_inmap_pixmap();
  update_speedmap_pixmap();
  update_volumemap_pixmap();
  int h = mapout->height();
  QPixmap mo(map_size,h);
  QPainter pmo;
  pmo.begin(&mo);
  for(int i = 0 ; i < map_size ; i ++)
    {
      QColor out;
      out.setHsv(abs(map[i] . take_from * 240 / map_size), 
		 255, 
		 valuetick( map [ i ].take_from ));
      pmo.setPen(out);
      pmo.drawLine(i,0,i,h-1);
    }
  pmo.end();
  int sx=mapout->width();
  mapout->setPixmap(mo.scaled(sx,h));
}

void Player::setAlsa()
{
  oss->setChecked(false);
  jack->setChecked(false);
}

void Player::setOss()
{
  alsa->setChecked(false);
  jack->setChecked(false);
}

void Player::setJack()
{
  alsa->setChecked(false);
  oss->setChecked(false);
}

static void init_dsp_and_clock()
{
  dsp_driver* old_dsp=dsp;
  dsp = dsp_driver::get_driver(config);
  delete old_dsp;
  
  clock_driver* old_metronome=metronome;
  metronome=new clock_driver();
  delete old_metronome;
}

void Player::restartCore()
{
  fetch_config_from(config,*this);
  stopCore();
  signed8 oldx_diskrate=x_diskrate;
  init_dsp_and_clock();
  startCore(0);
  y_playrate=y_normalise(oldx_diskrate);
  x_diskrate=oldx_diskrate;
  config->save();
  store_config_into(config,this);
}

void InitAndStart::run(Player * player)
{
  player->initCore();
  init_dsp_and_clock();
#ifdef MIDI
  midi_bindings* mbindings = new midi_bindings(player);
#endif
  player->startCore();
  ::metronome->init();
}

void PlayingStateChanged::run(Player * player)
{
  if (opt_check && dsp->get_paused())
    player->accept();
  player->set_start_stop_text();
}

void Player::customEvent(QEvent * e)
{
  if ((int)e->type()==(int)BpmPlayCustom)
    {
      BpmPlayEvent * E = (BpmPlayEvent*)e;
      E->run(this);
    }
}

/**
 * Here we assume that the core dsp device has been 
 * set to something useful
 */
void Player::startCore(int opening_tries)
{
#ifdef DEBUG_WAIT_STATES
  Debug("startCore try %d",opening_tries);
#endif
  int err=core_start(opt_check || opening_tries==0);
  if (opt_check)
    {	
      if (err==err_dsp)
	reject();
    }
  else
    {
      if (err==err_dsp)
	{
	  if (opening_tries==0)
	    config->set_player_dsp(0);
	  else
	    {
	      sleep(1);
	      startCore(opening_tries-1);
	    }
	}
    }
#ifdef DEBUG_WAIT_STATES
  Debug("startCore finished");
#endif
}

/**
 * here we stop the core, close the dsp device but
 * do not close the reading file
 */
void Player::stopCore()
{
  core_stop();
  delete(dsp);
  dsp = NULL;
}

void Player::initCore()
{
  store_config_into(config,this);
  /**
   * See bug #1099 to understand why this doesn't work
   */
  // tab->setCurrentIndex(TAB_PLAYER);

  /**
   * Check the raw file, we want to write the information synchronous
   */
  int err = core_object_init(opt_check);
  if (  show_error(err, err_noraw, "No raw file to be read. Probably the .mp3 "
		   "is broken.")
	|| show_error(err, err_nospawn, "Unable to spawn decoding process.\n"
		      "Please check your PATH environment variable")
	)
    if (opt_check)
      reject();
};

void msg_playing_state_changed()
{
  if (player_window)
    app->postEvent(player_window,new PlayingStateChanged());
}

class WritingFinished: public BpmPlayEvent
{
public:
  WritingFinished(): BpmPlayEvent()
  {
  }
  virtual void run(Player* sp)
  {
    if (normalperiod_metarate.valid())
      {
	if (sp->tab->currentIndex()==TAB_BEATGRAPH)
	  // it didn't change, but we want a recalculation anyway
	  sp->tabChanged(); 
	else if (sp->tab->currentIndex()==TAB_PLAYER)
	  sp->tab->setCurrentIndex(TAB_BEATGRAPH);
      }
  }
};

void msg_writing_finished()
{
  if (opt_check || opt_setup) return;
  if (player_window)
    app->postEvent(player_window,new WritingFinished());
}

void Player::loop(unsigned8 beats)
{
  loop_set_diskrate((beats*normalperiod_metarate*diskrate)/(4*metarate));
}

void Player::nudge(unsigned8 dir)
{
  static unsigned8 prev_dir = 64;
  while (dir < prev_dir) 
    {
      nudgeMinus1M();
      prev_dir --;
    }
  while (dir > prev_dir) 
    {
      nudgePlus1M();
      prev_dir ++;
    }
  prev_dir = dir;
}
#endif // __loaded__player_cpp__
