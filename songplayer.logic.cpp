/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle

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
#include <qcheckbox.h>
#include <sys/times.h>
#include "version.h"
#include <stdlib.h>
#include "kbpm-play.h"
#include <qfiledialog.h>
#include <qpainter.h>
#include "songplayer.logic.h"
#include "bpm-analyzer.logic.h"
#include "pattern-analyzer.logic.h"
#include "spectrumanalyzer.logic.h"
#include "about.h"

static int test=0;
void SongPlayerLogic::done(int r)
{
  test++;
  assert(test<2);
  // signal any active counter to stop working
  bpmcounter->finish();
  ::stop=1;
  ::paused=0;
  while(!finished) ;
  // now finish
  SongPlayer::done(r);
  test--;
}

SongPlayerLogic::SongPlayerLogic(QWidget*parent,const char*name, bool modal,WFlags f) :
  SongPlayer(parent,name,modal,f)
{
  bpmcounter  = new BpmAnalyzerDialog(this);
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
  if (playing->valid_tar_info())
    {
      QString blah = playing->encoded_tar();
      setCaption(blah);
    }
  else if (playing->get_storedin())
    {
      QString blah = playing->get_storedin();
      blah.replace("./index/","");
      setCaption(blah);
    }
  TempoLd->display(100.0*(double)normalperiod/(double)currentperiod);
  // set colors of tempo change buttons
  normalReached(currentperiod==normalperiod);
  startStopButton->setFocus();
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
  done(0);
}

void SongPlayerLogic::timerTick()
{
  unsigned4 m=wave_max();
  // the position if kB
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
  double  T0 = mperiod2bpm(currentperiod);
  double  T1 = mperiod2bpm(normalperiod);
  if (currentperiod<-1) T0=0;
  if (normalperiod<-1) T1=0;
  CurrentTempoLCD -> display(T0);
  NormalTempoLCD -> display(T1);
  // change tempo when necesarry
  if (fade_time>0)
    targetStep();
  // if visible...
  if (bpmcounter->isVisible())
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
  if (::paused)
    ::paused=0;
}

void SongPlayerLogic::stop()
{
  if (!::paused)
    {
      if (!cue) cue_set();
      ::paused=1;
    }
  else 
    jumpto(0,0);
}

void SongPlayerLogic::retrieveZ()
{
  cue_retrieve("Z-",0);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveX()
{
  cue_retrieve("X-",1);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveC()
{
  cue_retrieve("C-",2);
  jumpto(0,0);
}

void SongPlayerLogic::retrieveV()
{
  cue_retrieve("V-",3);
  jumpto(0,0);
}

void SongPlayerLogic::storeZ()
{
  checkCueNonZero();
  cue_store("Z-",0);
  redrawCues();
}

void SongPlayerLogic::storeX()
{
  checkCueNonZero();
  cue_store("X-",1);
  redrawCues();
}

void SongPlayerLogic::storeC()
{
  checkCueNonZero();
  cue_store("Z-",2);
  redrawCues();
}

void SongPlayerLogic::storeV()
{
  checkCueNonZero();
  cue_store("V-",3);
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
  changeTempo(targetperiod);
  normalReached(false);
}

void SongPlayerLogic::normalTempo()
{
  changeTempo(normalperiod);
  normalReached(true);
}

void SongPlayerLogic::fastSwitch()
{
   fade_time=10;
   tempo_fade=0;
}

void SongPlayerLogic::mediumSwitch()
{
   fade_time=30;
   tempo_fade=0;
}

void SongPlayerLogic::slowSwitch()
{
  fade_time=60;
  tempo_fade=0;
}

void SongPlayerLogic::normalReached(bool t)
{
  setColor(PushButton22,t);	
  setColor(switcherButton,t);
  setColor(PushButton37,t);
}

void SongPlayerLogic::targetStep()
{
  tempo_fade++;
  if (tempo_fade>fade_time)
    {
      tempo_fade=0;
      fade_time=0;
      normalReached(true);
      switcherButton->setText("Normal Switch");
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
  cue_shift("Shifting cue 1/32 note backward",-WAVRATE/80);
}

void SongPlayerLogic::nudgeCueForward()
{
  cue_shift("Shifting cue 1/32 note forward",+WAVRATE/80); 
}

void SongPlayerLogic::nudgeCueBack8M()
{
   cue_shift("Shifting cue 8 measure backward",-8*normalperiod); 
}

void SongPlayerLogic::nudgeCueForward8M()
{
   cue_shift("Shifting cue 8 measure forward",8*normalperiod);
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
  map_data new_map = allocate(new_size,map_segment);
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
      deallocate(map);
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
  map_data mapcopy = allocate(map_size,map_segment);
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
  if (bpmcounter->isVisible())
    bpmcounter->hide();
   else
     bpmcounter->show();
}

void SongPlayerLogic::openInfo()
{
  playing->executeInfoDialog();
}

void SongPlayerLogic::openSpectrumAnalyzer()
{
  SpectrumDialogLogic analyzer;
  analyzer.exec();
}

void SongPlayerLogic::openPatternAnalyzer()
{
  PatternAnalyzerLogic analyzer(true);
  analyzer.exec();
}

void SongPlayerLogic::openAbout()
{
  char tmp[500];
  AboutDialog about(NULL,NULL,1);
  sprintf(tmp,"BpmDj v%s",VERSION);
  about.versionLabel->setText(tmp);
  about.exec();
}

void SongPlayerLogic::mousePressEvent(QMouseEvent * e)
{
  QPoint p = e->pos();
  QPoint xy = mappingbox->mapFromParent(p);
  int x = xy.x();
  int y = xy.y();
  if (mapin->geometry().contains(x,y))
    { 
      x = x - mapin->x();
      x *= map_size;
      x /= mapin->width();
      assert(x >= 0 && x < map_size);
      mapin_down = mapin_up = x;
      update_map_pixmaps();
    }
  else if (mapspeed->geometry().contains(x,y))
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
  else if (mapvolume->geometry().contains(x,y))
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
  else if (mapout->geometry().contains(x,y))
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
  QPoint xy = mappingbox->mapFromParent(p);
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
  QPoint xy = mappingbox->mapFromParent(p);
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
  i%=8;
  if (i == 0) return 255;
        if (i == 1) return 128;
      if (i == 2) return 170;
        if (i == 3) return 128;
    if (i == 4) return 212;
        if (i == 5) return 128;
      if (i == 6) return 170;
        if (i == 7) return 128;
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
      out.setHsv(map[i] . take_from * 240 / map_size , 255 , valuetick( map [ i ].take_from ));
      pmo.setPen(out);
      pmo.drawLine(i,0,i,h-1);
    }
  pmo.end();
  mapout->setPixmap(*mo);
}

