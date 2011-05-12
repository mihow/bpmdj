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

#include <qlcdnumber.h>
#include <stdio.h>
#include <unistd.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qwidget.h>
#include <math.h>
#include <qslider.h>
#include <assert.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <sys/times.h>
#include <unistd.h>
#include "version.h"
#include <stdlib.h>
#include "kbpm-play.h"
#include <qfiledialog.h>
#include <qtabwidget.h>
#include <qpainter.h>
#include "songplayer.logic.h"
#include "bpm-analyzer.logic.h"
#include "beatgraph-analyzer.logic.h"
#include "spectrum-analyzer.logic.h"
#include "rythm-analyzer.logic.h"
#include "memory.h"
#include "dsp-drivers.h"
#include "aboutbox.h"
#include "bpmplay-event.h"

void fetch_config_from(PlayerConfig * target, const SongPlayerLogic& song_player);
void store_config_into(PlayerConfig * from,   SongPlayerLogic* song_player);

void fetch_config_from(PlayerConfig * target, const SongPlayerLogic& player)
{
  if (player.raw_directory->text()!=target->get_core_rawpath())
    {
      QMessageBox::information(NULL,".raw file directory",
			       "Changes to the temporary storage will only be in effect the next\n"
			       "time you start kbpm-play with this configuration\n",
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
      QMessageBox::information(NULL,"Decoder option",
			       "Specification of the new capacities will only be in effect the next\n"
			       "time you start kbpm-play with this configuration\n",
			       QMessageBox::Ok,QMessageBox::NoButton);
      target->set_disabled_capacities(cap);
    }
  
  target->set_player_dsp(0);
  if (player.alsa->isChecked())
    target->set_player_dsp(2);
  if (player.oss->isChecked())
    target->set_player_dsp(1);
  if (player.bpm->isChecked())
    target->set_player_dsp(3);

  target->set_alsa_latency(player.alsa_latency->value());
  target->set_alsa_verbose(player.alsa_verbose->isChecked());
  target->set_alsa_dev(player.alsa_dev->text());

  target->set_oss_dsp(player.oss_dsp->text());
  target->set_oss_init_fragments(player.oss_opt_fragments->isChecked());
  target->set_oss_fragments(player.oss_fragments->value());
  target->set_oss_verbose(player.oss_verbose->isChecked());
  target->set_oss_nolatencyaccounting(player.oss_nolatency->isChecked());
  target->set_oss_latency(player.oss_latency->value());

  target->set_mixed_channel(player.bpm_channel->value());

  target->set_player_rms(player.rms_toggle->isChecked());
  target->set_player_rms_target(atof(player.rms->text()));
}

void store_config_into(PlayerConfig * from, SongPlayerLogic* player)
{
  player->raw_directory->setText(from->get_core_rawpath());
  // clear all selections according to numerical coding
  player->alsa->setChecked(from->get_player_dsp()==2);
  player->oss->setChecked(from->get_player_dsp()==1);
  player->bpm->setChecked(from->get_player_dsp()==3);
  
  player->alsa_latency->setValue(from->get_alsa_latency());
  player->alsa_verbose->setChecked(from->get_alsa_verbose());
  player->alsa_dev->setText(from->get_alsa_dev());

  player->oss_dsp->setText(from->get_oss_dsp());
  player->oss_opt_fragments->setChecked(from->get_oss_init_fragments());
  player->oss_fragments->setValue(from->get_oss_fragments());
  player->oss_verbose->setChecked(from->get_oss_verbose());
  player->oss_nolatency->setChecked(from->get_oss_nolatencyaccounting());
  player->oss_latency->setValue(from->get_oss_latency());

  
  player->bpm_channel->setValue(from->get_mixed_channel());

  player->rms_toggle->setChecked(from->get_player_rms());
  player->rms->setText(QString::number(from->get_player_rms_target()));
  init_capacity_widget(player->capacity,from->get_disabled_capacities());
}

void SongPlayerLogic::done(int r)
{
  static int test=0;
  test++;
  assert(test<2);
  // signal any active counter to stop working
  if (bpmcounter) bpmcounter->finish();
  core_stop();
  core_done();
  // now finish
  SongPlayer::done(r);
  test--;
}

void SongPlayerLogic::init_tempo_switch_time()
{
  // we must know both tempos
  if (targetperiod <= 0 || normalperiod <= 0) return;
  // we take the percentage difference
  int percentage = targetperiod * 100 / normalperiod;
  if (percentage < 100) percentage = 100 - percentage;
  else percentage -= 100;
  // 1 percent difference is 10 seconds switch time
  int time = percentage * 10;
  // we limit this time to half of the songlength
  int total_time = 0;
  if (playing) total_time = playing->get_time_in_seconds();
  total_time/=2;
  // if the songlength is known of cours
  if (total_time<=0) total_time=180;
  if (time > total_time) time = total_time;
  // we don't want switching times smalelr than 10 seconds 
  if (time < 10) time = 10;
  // and not larger than 180 s
  if (time > 180) time = 180;
  // finally we set the value
  tempoSwitchTime->setValue(time);
}

SongPlayerLogic::SongPlayerLogic(QWidget*parent,const char*name, bool modal,WFlags f) :
  SongPlayer(parent,name,modal,f)
{
  if (playing) bpmcounter  = new BpmAnalyzerDialog(this);
  else bpmcounter = NULL;
  timer = new QTimer(this);
  map = NULL;
  map_size = 0;
  map_scale = 16;
  mapLengthChanged(64);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(1000);
  tempo_fade=-1;
  fade_time=0;
  wantedcurrentperiod=0;
  redrawCues();
  // set caption
  captionize_according_to_index();
  TempoLd->display(100.0*(double)normalperiod/(double)currentperiod);
  // set colors of tempo change buttons
  init_tempo_switch_time();
  normalReached(currentperiod==normalperiod);
  startStopButton->setFocus();
  connect(tab, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabChanged() ) );
}

void SongPlayerLogic::captionize_according_to_index()
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
  setCaption(blah);
}

void SongPlayerLogic::tabChanged()
{
  if (tab->currentPageIndex() == TAB_BEATGRAPH)
    beatGraphAnalyzer->activate();
  if (tab->currentPageIndex() == TAB_OPTIONS)
    store_config_into(config,this);
}

void SongPlayerLogic::redrawCues()
{
   setColor(PushButton11,cues[0]>0);
   setColor(PushButton12,cues[1]>0);
   setColor(PushButton13,cues[2]>0);
   setColor(PushButton14,cues[3]>0);
}

void SongPlayerLogic::setColor(QButton *button, bool enabled)
{
   QColor a, ad, am, aml, al;
   QPalette pal;
   QColorGroup cg;

   if (enabled)
     {
	a=QColor(0,255,127);
	ad=QColor(0,127,64);
	am=QColor(0, 170, 85);
	aml=QColor(63, 255,  159);
	al=QColor( 127,255,191) ;
     }
   else
     {
	a=QColor(255,0,127);
	ad=QColor(127,0,64);
	am=QColor( 170, 0, 85);
	aml=QColor( 255, 63, 159);
	al=QColor( 255, 127, 191) ;
     }

   cg.setColor( QColorGroup::Foreground, black );
   cg.setColor( QColorGroup::Button, a );
   cg.setColor( QColorGroup::Light, al);
   cg.setColor( QColorGroup::Midlight, aml);
   cg.setColor( QColorGroup::Dark, ad );
   cg.setColor( QColorGroup::Mid, am);
   cg.setColor( QColorGroup::Text, black );
   cg.setColor( QColorGroup::BrightText, white );
   cg.setColor( QColorGroup::ButtonText, black );
   cg.setColor( QColorGroup::Base, white );
   cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
   cg.setColor( QColorGroup::Shadow, black );
   cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
   cg.setColor( QColorGroup::HighlightedText, white );
   
   pal.setActive( cg );
   cg.setColor( QColorGroup::Foreground, black );
   cg.setColor( QColorGroup::Button, a );
   cg.setColor( QColorGroup::Light, QColor( 255, 127, 191) );
   cg.setColor( QColorGroup::Midlight, QColor( 255, 38, 147) );
   cg.setColor( QColorGroup::Dark, ad );
   cg.setColor( QColorGroup::Mid, am);
   cg.setColor( QColorGroup::Text, black );
   cg.setColor( QColorGroup::BrightText, white );
   cg.setColor( QColorGroup::ButtonText, black );
   cg.setColor( QColorGroup::Base, white );
   cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
   cg.setColor( QColorGroup::Shadow, black );
   cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
   cg.setColor( QColorGroup::HighlightedText, white );
  
   pal.setInactive( cg );
   cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
   cg.setColor( QColorGroup::Button, a );
   cg.setColor( QColorGroup::Light, QColor( 255, 127, 191) );
   cg.setColor( QColorGroup::Midlight, QColor( 255, 38, 147) );
   cg.setColor( QColorGroup::Dark, ad );
   cg.setColor( QColorGroup::Mid, am);
   cg.setColor( QColorGroup::Text, black );
   cg.setColor( QColorGroup::BrightText, white );
   cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
   cg.setColor( QColorGroup::Base, white );
   cg.setColor( QColorGroup::Background, QColor( 192, 192, 192) );
   cg.setColor( QColorGroup::Shadow, black );
   cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
   cg.setColor( QColorGroup::HighlightedText, white );
   pal.setDisabled( cg );
   
   button->setPalette( pal );
   
}

void SongPlayerLogic::nudgePlus()
{
  ::y-=::currentperiod/(4*32);
  if (::y<0)
    {
      printf("y underflow, setting to zero\n");
      ::y=0;
    }
}

void SongPlayerLogic::nudgeMinus()
{
  ::y+=currentperiod/(4*32);
}

void SongPlayerLogic::nudgePlusB()
{
  ::y+=currentperiod/4;
}

void SongPlayerLogic::nudgeMinusHalfB()
{
  ::y-=currentperiod/8;
}

void SongPlayerLogic::nudgePlus1M()
{
  ::y+=currentperiod;
}

void SongPlayerLogic::nudgeMinus1M()
{
   ::y-=currentperiod;
   if (::y<0)
     {
	printf("y underflow, setting to zero\n");
	::y=0;
     }
}

void SongPlayerLogic::nudgePlus4M()
{
   ::y+=currentperiod*4;
}

void SongPlayerLogic::nudgeMinus4M()
{
   ::y-=currentperiod*4;
   if (::y<0)
     {
	printf("y underflow, setting to zero\n");
	::y=0;
     }
}

void SongPlayerLogic::nudgePlus8M()
{
   ::y+=currentperiod*8;
}

void SongPlayerLogic::nudgeMinus8M()
{
   ::y-=currentperiod*8;
   if (::y<0)
     {
	printf("y underflow, setting to zero\n");
	::y=0;
     }
}

void SongPlayerLogic::accept()
{
  done(Accepted);
}

static bool autostarted = false;

void SongPlayerLogic::timerTick()
{
  static int no_raw_file_error_box = 10;
  unsigned4 m=wave_max();
  if (no_raw_file_error_box-->0)
    {
      // see comments in PlayerConfig::load_ui_position
      if (no_raw_file_error_box==9) 
	config->load_ui_position(this);
      // should we start playing ?
      if (!autostarted && m>5*44100 && get_paused())
	{
	  autostarted=true;
	  restart();
	}
      // error on the disk stuff ?
      if (m==0 && no_raw_file_error_box==0 && playing)
	{
	  QMessageBox::information(NULL,"No .raw file",
				   "it seems like there is no .raw file on disk\n"
				   "check whether the bpmdj-raw script behaves properly (look at the console output)\n"
				   "or check whether the target direcotry is valid and accessable (in the options tab)\n",
				   QMessageBox::Ok,QMessageBox::NoButton);
	}
    }
  
  // the position in kB
  // LcdLeft->display((double)((int)(x_normalise(::y)*1000/m))/(double)10);
  // LcdRight->display((int)(m/1024));
  // times are displayed with respect to the current tempo
  unsigned4 m2;
  if (currentperiod<0)
    m2 = m;
  else
    m2 = y_normalise(m);
  unsigned4 totaltime=samples2s(m2);
  unsigned4 currenttime=samples2s(::y);
  char totalstr[20], currentstr[20];
  sprintf(totalstr,"%02d:%02d",(int)totaltime/60,(int)totaltime%60);
  sprintf(currentstr,"%02d:%02d",(int)currenttime/60,(int)currenttime%60);
  CurrentTimeLCD -> display(currentstr);
  TotalTimeLCD -> display(totalstr);
  // show current tempo
  if (playing)
    {
      double  T0 = mperiod2bpm(currentperiod);
      double  T1 = mperiod2bpm(normalperiod);
      if (currentperiod<-1) T0=0;
      if (normalperiod<-1) T1=0;
      CurrentTempoLCD -> display(T0);
      NormalTempoLCD -> display(T1);
    }
  else
    {
      CurrentTempoLCD -> display(0);
      NormalTempoLCD -> display(0);
    }
  // change tempo when necesarry
  if (fade_time>0)
    targetStep();
  // if visible...
  if (bpmcounter && bpmcounter->isVisible())
    bpmcounter->timerTick();
}

void SongPlayerLogic::setCue()
{
  cue_set();
}

void SongPlayerLogic::shiftBack()
{
  ::y=y_normalise(cue)+dsp->latency();
}

void SongPlayerLogic::restart()
{
  ::y=0;
  unpause_playing();
}

void SongPlayerLogic::set_start_stop_text()
{
  if (get_paused())
    startStopButton->setText("Start");
  else
    {
      startStopButton->setText("Stop");
      autostarted=true;
    }
}

void SongPlayerLogic::start_stop()
{
  if (!get_paused())
    {
      if (!cue) cue_set();
      pause_playing();
    }
  else jumpto(0,0);
}

void SongPlayerLogic::retrieveZ()
{
  cue_retrieve(0);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveX()
{
  cue_retrieve(1);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveC()
{
  cue_retrieve(2);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveV()
{
  cue_retrieve(3);
  jumpto(0,0);
}

void SongPlayerLogic::storeZ()
{
  checkCueNonZero();
  cue_store(0);
  redrawCues();
}

void SongPlayerLogic::storeX()
{
  checkCueNonZero();
  cue_store(1);
  redrawCues();
}

void SongPlayerLogic::storeC()
{
  checkCueNonZero();
  cue_store(2);
  redrawCues();
}

void SongPlayerLogic::storeV()
{
  checkCueNonZero();
  cue_store(3);
  redrawCues();
}

void SongPlayerLogic::checkCueNonZero()
{
  if (::cue==0)
    QMessageBox::warning(this,"Cue Zero ?",
			 "If you want to store the current playing position in a cue,\n"
			 "you must first create a cue. You can do this by selecting\n"
			 "'set Cue' (or pressing '/') at that point. Afterwards you can\n"
			 "store it in any of the 4 positions");
}

#define knick 15
void SongPlayerLogic::changeTempo(int p)
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
  changetempo(p);
  TempoLd->display(100.0*(double)normalperiod/(double)currentperiod);
}

void SongPlayerLogic::targetTempo()
{
  tempo_fade = fade_time;
  changeTempo(targetperiod);
  normalReached(false);
}

void SongPlayerLogic::normalTempo()
{
  tempo_fade = fade_time;
  changeTempo(normalperiod);
  normalReached(true);
}

void SongPlayerLogic::mediumSwitch()
{
  fade_time = tempoSwitchTime->value();
  tempo_fade=0;
}

void SongPlayerLogic::normalReached(bool t)
{
  setColor(switcherButton,t);
}

void SongPlayerLogic::targetStep()
{
  tempo_fade++;
  if (tempo_fade>fade_time)
    {
      tempo_fade=0;
      fade_time=0;
      normalReached(true);
      switcherButton->setText("Fade");
      return;
    }
  if (tempo_fade==1)
    {
      normalReached(false);
    }
  if (tempo_fade>0)
    {
      switcherButton->setText(QString::number(fade_time-tempo_fade));
    }
  
  /**
   * We dont change the period lineary !
   */
  double result;
  result = (double)targetperiod*pow((double)normalperiod/(double)targetperiod,(double)tempo_fade/(double)fade_time);
  // result = targetperiod+((double)normalperiod-(double)targetperiod)*(double)tempo_fade/(double)fade_time;
  changeTempo((int)result);
}

void SongPlayerLogic::tempoChanged()
{
  if (wantedcurrentperiod == 0 )
    wantedcurrentperiod = currentperiod;
  changeTempo(wantedcurrentperiod);
}

void SongPlayerLogic::nudgeCueBack()
{
  cue_shift(-WAVRATE/80);
}

void SongPlayerLogic::nudgeCueForward()
{
  cue_shift(+WAVRATE/80); 
}

void SongPlayerLogic::nudgeCueBack8M()
{
   // Shifting cue 8 measure backward
   cue_shift(-8*normalperiod); 
}

void SongPlayerLogic::nudgeCueForward8M()
{
   cue_shift(8*normalperiod);
}

void SongPlayerLogic::fastSaw()
{
   lfo_set("Saw",lfo_saw,16,::y-dsp->latency());
}

void SongPlayerLogic::slowSaw()
{
   lfo_set("Saw",lfo_saw,8,::y-dsp->latency());
}

void SongPlayerLogic::fastPan()
{
   lfo_set("Pan",lfo_pan,16,::y-dsp->latency());
}

void SongPlayerLogic::slowPan()
{
   lfo_set("Pan",lfo_pan,8,::y-dsp->latency()); 
}

void SongPlayerLogic::normalLfo()
{
   lfo_set("No",lfo_no,4,::y-dsp->latency()); 
}

void SongPlayerLogic::fastRevSaw()
{
  lfo_set("Reverse saw",lfo_revsaw,16,::y-dsp->latency());
}

void SongPlayerLogic::slowRevSaw()
{
  lfo_set("Reverse saw",lfo_revsaw,8,::y-dsp->latency()); 
}

void SongPlayerLogic::metronome()
{
  if (lfo_get()==lfo_metronome)
    normalLfo();
  else
    lfo_set("Metronome",lfo_metronome,16,::y-dsp->latency()); 
}

void SongPlayerLogic::breakLfo()
{
  lfo_set("Break",lfo_break,1,::y-dsp->latency());
}

void SongPlayerLogic::mapScaleChanged(int i)
{
  if (i == 0) map_scale = 64;
  else if (i == 1) map_scale = 32;
  else if (i == 2) map_scale = 16;
  else if (i == 3) map_scale = 8;
  else if (i == 4) map_scale = 4;
  else assert(0);
  startStopButton->setFocus();
}

void SongPlayerLogic::update_map_scale_box()
{
  if (map_scale == 64) mapping_scale->setCurrentItem(0);
  else if (map_scale == 32) mapping_scale->setCurrentItem(1);
  else if (map_scale == 16) mapping_scale->setCurrentItem(2);
  else if (map_scale == 8) mapping_scale->setCurrentItem(3);
  else if (map_scale == 4) mapping_scale->setCurrentItem(4);
  else assert(0);
}

void SongPlayerLogic::mapLengthChanged(int new_size)
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

void SongPlayerLogic::saveMap()
{
  QString s = QFileDialog::getSaveFileName("sequences","Beat Patterns (*.map)",this,"Save pattern","Choose a filename" );
  if (s.isNull()) return;
  const char* filename = s;
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
  if (atend_loop->isOn()) nr |= 8;
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

void SongPlayerLogic::loadMap()
{
  QString s = QFileDialog::getOpenFileName("sequences","Beat Patterns (*.map)",this,"Load Pattern","Choose a file");
  if (s.isNull()) return;
  const char* filename = s;
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

void SongPlayerLogic::mapStart()
{
  map_data mapcopy = bpmdj_allocate(map_size,map_segment);
  // copy all data and take scaling into account...
  for(int i = 0 ; i < map_size ; i ++)
    {
      // location
      mapcopy[i].take_from=map[i].take_from;
      // speed
      float factor = pow(2,((float)map[i].speed_mult)/12.0);
      assert(factor!=0);
      if (factor>=1)
	{
	  mapcopy[i].speed_mult=16000;
	  mapcopy[i].speed_div=(signed2)((float)16000/factor);
	}
      if (factor<1)
	{
	  mapcopy[i].speed_mult=(signed2)((float)16000*factor);
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
    mexit = ::cues[0];
  else if (atend_x->isChecked())
    mexit = ::cues[1];
  else if (atend_c->isChecked())
    mexit = ::cues[2];
  else if (atend_v->isChecked())
    mexit = ::cues[3];
  else if (atend_stop->isChecked())
    mexit = map_exit_stop;
  else
    assert(0);
  map_set(map_size,mapcopy,normalperiod*map_size/map_scale,mexit,atend_loop->isOn());
}

void SongPlayerLogic::mapStop()
{
  map_stop();
}

void SongPlayerLogic::mapLoopChange()
{
  map_loop_set(atend_loop->isOn());
}


void SongPlayerLogic::openBpmCounter()
{
  if (bpmcounter)
    {
      if (bpmcounter->isVisible())
	bpmcounter->hide();
      else
	bpmcounter->show();
    }
  else
    {
      QMessageBox::information(NULL,"Setting up kbpm-play",
			       "You are setting up kbpm-play.\n"
			       "There is no song playing at the moment\n",
			       QMessageBox::Ok,QMessageBox::NoButton);
    }
}

void SongPlayerLogic::openRythmAnalyzer()
{
  if (!playing)
    {
      QMessageBox::information(NULL,"Setting up kbpm-play",
			       "You are setting up kbpm-play.\n"
			       "There is no song playing at the moment\n",
			       QMessageBox::Ok,QMessageBox::NoButton);
      return;
    }
  RythmDialogLogic analyzer;
  analyzer.exec();
}

void SongPlayerLogic::openInfo()
{
  if (!playing)
    {
      QMessageBox::information(NULL,"Setting up kbpm-play",
			       "You are setting up kbpm-play.\n"
			       "There is no song playing at the moment\n",
			       QMessageBox::Ok,QMessageBox::NoButton);
    }
  else
    {
      playing->executeInfoDialog();
      captionize_according_to_index();
    }
}

void SongPlayerLogic::openSpectrumAnalyzer()
{
  if (!playing)
    {
      QMessageBox::information(NULL,"Setting up kbpm-play",
			       "You are setting up kbpm-play.\n"
			       "There is no song playing at the moment\n",
			       QMessageBox::Ok,QMessageBox::NoButton);
      return;
    }
  SpectrumDialogLogic analyzer;
  analyzer.exec();
}

void SongPlayerLogic::openAbout()
{
  doAbout(1);
}

void SongPlayerLogic::mousePressEvent(QMouseEvent * e)
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
      if (e->button()==LeftButton)
	{
	  int s = mapin_up - mapin_down + 1;
	  if (s > map_size - x ) 
	    s = map_size - x;
	  for(int i = 0 ; i < s ; i++)
	    map[x+i].take_from= i + mapin_down;
	}
      else if (e->button() == RightButton)
	{
	  map[x].take_from=-1;
	}
      update_map_pixmaps();
    }
  SongPlayer::mousePressEvent(e);
}

void SongPlayerLogic::mouseMoveEvent(QMouseEvent * e)
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
  SongPlayer::mousePressEvent(e);
}

void SongPlayerLogic::mouseReleaseEvent(QMouseEvent * e)
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
  SongPlayer::mouseReleaseEvent(e);
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

void SongPlayerLogic::update_inmap_pixmap()
{
  int h = mapin->height();
  QPixmap *mi = new QPixmap(map_size,h);
  QPainter pmi;
  pmi.begin(mi);
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
  mapin->setPixmap(*mi);
}

void SongPlayerLogic::update_speedmap_pixmap()
{
  int h = mapspeed->height();
  QPixmap *ms = new QPixmap(map_size,h);
  QPainter pms;
  pms.begin(ms);
  for(int i = 0 ; i < map_size ; i ++)
    {
      pms.setPen(Qt::white);
      pms.drawLine(i,0,i,h-1);
      pms.setPen(Qt::black);
      pms.drawPoint(i,(12-map[i].speed_mult)*mapspeed->height()/24);
    }
  pms.end(); 
  mapspeed->setPixmap(*ms);
}

void SongPlayerLogic::update_volumemap_pixmap()
{
  int h = mapvolume->height();
  QPixmap *mv = new QPixmap(map_size,h);
  QPainter pmv;
  pmv.begin(mv);
  for(int i = 0 ; i < map_size ; i ++)
    {
      pmv.setPen(Qt::white);
      pmv.drawLine(i,0,i,h-1);
      pmv.setPen(Qt::black);
      pmv.drawPoint(i,(100-map[i].volume)*mapspeed->height()/100);
    }
  pmv.end(); 
  mapvolume->setPixmap(*mv);
}

void SongPlayerLogic::update_map_pixmaps()
{
  update_inmap_pixmap();
  update_speedmap_pixmap();
  update_volumemap_pixmap();
  int h = mapout->height();
  QPixmap *mo = new QPixmap(map_size,h);
  QPainter pmo;
  pmo.begin(mo);
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
  mapout->setPixmap(*mo);
}


void SongPlayerLogic::setAlsa()
{
  oss->setChecked(false);
  bpm->setChecked(false);
}

void SongPlayerLogic::setOss()
{
  alsa->setChecked(false);
  bpm->setChecked(false);
}

void SongPlayerLogic::setBpmMixingDesk()
{
  alsa->setChecked(false);
  oss->setChecked(false);
}

void SongPlayerLogic::restartCore()
{
  fetch_config_from(config,*this);
  // printf("Stopping core\n");
  stopCore();
  // printf("reassigning dsp device\n");
  dsp = dsp_driver::get_driver(config);
  //  printf("Starting core\n");
  startCore();
  // printf("Finished applying options\n");
  config->save();
  store_config_into(config,this);
}

void InitAndStart::run(SongPlayerLogic * player)
{
  player->initCore();
  player->startCore();
}

void PlayingStateChanged::run(SongPlayerLogic * player)
{
  if (opt_check) 
    player->accept();
  player->set_start_stop_text();
}

void SongPlayerLogic::customEvent(QCustomEvent * e)
{
  if ((int)e->type()==(int)BpmPlayCustom)
    {
      BpmPlayEvent * E = (BpmPlayEvent*)e;
      E->run(this);
    }
}

/**
 * here we assume that the core dsp device has been set to something usefull
 */
void SongPlayerLogic::startCore()
{
  int err = core_start();
  if (opt_check)
    {
      if (show_error(err, err_dsp, "Unable to open dsp device"))
	reject();
    }
  else
    {
      if (show_error(err, err_dsp, "Unable to open dsp device\n"
		     "Swithcing back to zero DSP driver\n"))
	{
	  config->set_player_dsp(0);
	  dsp = dsp_driver::get_driver(config);
	  err = core_start();
	  show_error(err, err_dsp, "Still unable to get anywhere\n");
	};
    }
}

/**
 * here we stop the core, close the dsp device but
 * do not close the reading file
 */
void SongPlayerLogic::stopCore()
{
  core_stop();
  delete(dsp);
  dsp = NULL;
}

void SongPlayerLogic::initCore()
{
  // if we check the raw file, we want to write the information synchronous
  int err = core_object_init(opt_check);
  if (show_error(err, err_noraw, "No raw file to be read. Probably the .mp3 is broken.\n")
      || show_error(err, err_nospawn, "Unable to spawn decoding process.\nPlease check your PATH environment variable\n"))
    if (opt_check) 
      reject();
};

void msg_playing_state_changed()
{
  app->postEvent(player_window,new PlayingStateChanged());
}

void msg_writing_finished()
{
  /*  if (player_window)
    app->postEvent(player_window,new QCustomEvent(WritingFinished));
  */
}


