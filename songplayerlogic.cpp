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

#include <qlcdnumber.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qlabel.h>
#include <math.h>
#include <qslider.h>
#include <sys/times.h>
#include "songplayerlogic.h"
#include "kbpm-play.h"
#include "version.h"
#include "kbpm-counter.h"

extern "C" 
{
#include "cbpm-index.h"
}

SongPlayerLogic::SongPlayerLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0) :
  SongPlayer(parent,name,modal,f)
{
   bpmcounter  = new BpmCountDialog(this);
   visiblebpmcounter = false;
   timer = new QTimer(this);
   connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
   timer->start(1000);
   tempo_fade=-1;
   fade_time=0;
   redrawCues();
   // remove last 8 charactesr...
   int l=strlen(wave_name);
   char* blah=(char*)malloc(sizeof(l));
   blah=strdup(wave_name);
   if (l>8) blah[l-8]=0;
   PlayingText->setText(blah);
   
   TempoLd->display(100.0*(double)normalperiod/(double)currentperiod);
   // set colors of tempo change buttons
   normalReached(currentperiod==normalperiod);
   /* 
    setColor(PushButton22,false);
    setColor(PushButton36_2,false);
    setColor(PushButton37,false);
    */
   
   // set volume sliders
   Slider3->setValue(100-mixer_get_main());
   Slider4->setValue(100-mixer_get_pcm());
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
   // signal any active counter to stop working
   bpmcounter->finish();
   // if the song has been stopped, simply start and stop again.
   ::stop=1;
   ::paused=0;
   // wait until finished
   while(!finished) ;
   // now finish
   SongPlayer::accept();
}

void SongPlayerLogic::timerTick()
{
   unsigned4 m=wave_max();
   // the position if kB
   LcdLeft->display((double)((int)(x_normalise(::y)*1000/m))/(double)10);
   LcdRight->display((int)(m/1024));
   // times are displayed with respect to the current tempo
   unsigned4 totaltime=y_normalise(m)/WAVRATE;
   unsigned4 currenttime=::y/WAVRATE;
   char totalstr[20], currentstr[20];
   sprintf(totalstr,"%02d:%02d",totaltime/60,totaltime%60);
   sprintf(currentstr,"%02d:%02d",currenttime/60,currenttime%60);
   CurrentTimeLCD -> display(currentstr);
   TotalTimeLCD -> display(totalstr);
   // show current tempo
   double  T0=4.0*(double)WAVRATE*60.0/(double)currentperiod;
   double  T1=4.0*(double)WAVRATE*60.0/(double)normalperiod;
   CurrentTempoLCD -> display(T0);
   NormalTempoLCD -> display(T1);
   // change tempo when necesarry
   if (fade_time>0)
     targetStep();
   // if visible...
   if (visiblebpmcounter)
     bpmcounter->timerTick();

}

void SongPlayerLogic::setCue()
{
   cue_set();
}

void SongPlayerLogic::shiftBack()
{
   ::y=y_normalise(cue)+dsp_latency();
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
   cue_store("Z-",0);
   redrawCues();
}

void SongPlayerLogic::storeX()
{
   cue_store("X-",1);
   redrawCues();
}

void SongPlayerLogic::storeC()
{
   cue_store("Z-",2);
   redrawCues();
}

void SongPlayerLogic::storeV()
{
   cue_store("V-",3);
   redrawCues();
}

void SongPlayerLogic::targetTempo()
{
   changetempo(targetperiod);
   normalReached(false);
}

void SongPlayerLogic::normalTempo()
{
   changetempo(normalperiod);
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
     setColor(PushButton36_2,t);
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
	return;
     }
   if (tempo_fade==1)
     {
	normalReached(false);
     }
   
   /**
    * Another enhancement is that we dont change the period lineary !
    */
   double result;
   result = (double)targetperiod*pow((double)normalperiod/(double)targetperiod,(double)tempo_fade/(double)fade_time);
   changetempo((int)result);
   TempoLd->display(100.0*(double)normalperiod/(double)currentperiod);
}

void SongPlayerLogic::nudgeCueBack()
{
   cue_shift("Shifting cue 1/32 note backward",-normalperiod/128);
}

void SongPlayerLogic::nudgeCueForward()
{
   cue_shift("Shifting cue 1/32 note forward",+normalperiod/128); 
}

void SongPlayerLogic::nudgeCueBack8M()
{
   cue_shift("Shifting cue 8 measure backward",-8*normalperiod); 
}

void SongPlayerLogic::nudgeCueForward8M()
{
   cue_shift("Shifting cue 8 measure forward",8*normalperiod);
}

void SongPlayerLogic::mainVolume(int v)
{
   mixer_set_main(100-v);
}

void SongPlayerLogic::pcmVolume(int v)
{
   mixer_set_pcm(100-v);
}

void SongPlayerLogic::fastSaw()
{
   lfo_set("Saw",lfo_saw,16,::y-dsp_latency());
}

void SongPlayerLogic::slowSaw()
{
   lfo_set("Saw",lfo_saw,8,::y-dsp_latency());
}

void SongPlayerLogic::fastPan()
{
   lfo_set("Pan",lfo_pan,16,::y-dsp_latency());
}

void SongPlayerLogic::slowPan()
{
   lfo_set("Pan",lfo_pan,8,::y-dsp_latency()); 
}

void SongPlayerLogic::normalLfo()
{
   lfo_set("No",lfo_no,4,::y-dsp_latency()); 
}

void SongPlayerLogic::fastRevSaw()
{
 lfo_set("Reverse saw",lfo_revsaw,16,::y-dsp_latency());
}

void SongPlayerLogic::slowRevSaw()
{
   lfo_set("Reverse saw",lfo_revsaw,8,::y-dsp_latency()); 
}

void SongPlayerLogic::metronome()
{
   lfo_set("Metronome",lfo_metronome,16,::y-dsp_latency()); 
}

void SongPlayerLogic::breakLfo()
{
   lfo_set("Break",lfo_break,1,::y-dsp_latency());
}

void SongPlayerLogic::openBpmCounter()
{
   if (visiblebpmcounter)
     bpmcounter->hide();
   else
     bpmcounter->show();
   visiblebpmcounter=!visiblebpmcounter;   
}
