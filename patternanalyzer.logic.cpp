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

#include <qapplication.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <qprogressdialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <qspinbox.h>
#include <unistd.h>
#include <libgen.h>
#include <qslider.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "patternanalyzer.logic.h"
#include "sys/times.h"
#include "fourier.h"
#include "kbpm-play.h"

extern "C"
{
#include "cbpm-index.h"
#include "version.h"
#include "scripts.h"
}

#define COLLAPSE 4
PatternAnalyzerLogic::PatternAnalyzerLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  PatternDialog(0,name,modal,f)
{
  period=normalperiod;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  
  // read in memory and shrink it 
  FILE * raw=openRawFile();
  audiosize=fsize(raw)/4;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize/COLLAPSE,compressed);
  phasesize = audiosize;
  phasesize /= normalperiod;
  phases = allocate(phasesize,int);
  
  QProgressDialog * progress = new QProgressDialog();
  progress->setTotalSteps(audiosize/bufsize);
  progress->show();
  
  for(int i = 0 ; i <phasesize; i ++)
    phases[i]=0;
  
  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      progress->setProgress(pos/bufsize);
      app->processEvents();

      for(int i = 0 ; i<count/COLLAPSE; i++)
	{
	  unsigned4 sum=0;
	  for(int j = 0 ; j < COLLAPSE ; j++)
	    {
	      assert(i*COLLAPSE+j<count);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.left)/(1<<7);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.right)/(1<<7);
	    }
	  sum /= COLLAPSE*2;
	  assert(i + pos/COLLAPSE < audiosize/COLLAPSE);
	  data[i+pos/COLLAPSE]=sum;;
	}
      pos+=count;
    }

  fclose(raw);
  delete(progress);
  
  // normalize the audio
  unsigned4 maximum = 0;
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    if (data[i]>maximum)
      maximum=data[i];
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    data[i]=(signed8)data[i]*(signed8)255/maximum;
  
  // immediatelly show data, user wouldn't select pattern if it wasn't to view
  // the darn thing...
  showPattern();
}

void PatternAnalyzerLogic::showPattern()
{ 
  int meass = measures->value();
  unsigned4 period2 = period/COLLAPSE;
  unsigned4 size = audiosize/COLLAPSE;
  int X = size/(period2*meass)-1;
  int Y = pattern->contentsRect().height();
  int dc = color->value();

  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  
  float yscale = meass*period2;
  yscale/=Y;

  signed4 *project;
  project = allocate(Y,signed4);
  assert(project);
  
  for(int y =  0 ; y < Y ; y ++)
    project[y]=0;
  
  for(int x = 0 ; x < X ; x++)
    {
      bool red = false;
      unsigned4 idx = x * period2 * meass;
      idx += phases[x];
      if (::x/(normalperiod*meass) == x)
	red = true;
      if (red)
	for(int y = 0 ; y < Y ; y++)
	  {
	    unsigned4 idx2 = (int)((float)y*yscale);
	    int val = data[idx+idx2];
	    project[y]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,0,0));
	    p.drawPoint(x,y);
	  }
      else
	for(int y = 0 ; y < Y ; y++)
	  {
	    unsigned4 idx2 = (int)((float)y*yscale);
	    int val = data[idx+idx2];
	    project[y]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,val,val));
	    p.drawPoint(x,y);
	  }
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int y = 0 ; y < Y ; y+=Y/8)
    p.drawLine(0,y,X-1,y);
  p.end(); 
  pattern->setPixmap(*pm);
  
  signed4 maximum=0;
  for(int y = 0 ; y < Y ; y ++)
    if (project[y]>maximum)
      maximum=project[y];
  for(int y = 0 ; y < Y ; y ++)
    project[y]=((signed8)project[y])*(signed8)255/(signed8)maximum;
  
  QPixmap *sm = new QPixmap(1,Y);
  QPainter l;
  l.begin(sm);
  for(int y = 0 ; y < Y ; y++)
    {
      int val;
      val = project[y];
      l.setPen(QColor(val,val,val));
      l.drawPoint(0,y);
    }
  l.end();
  projection->setPixmap(*sm);
}

void PatternAnalyzerLogic::fixPhases()
{
  printf("FIXING PHASES !!\n");
  int meass = measures->value();
  unsigned4 period2 = period/COLLAPSE;
  unsigned4 size = audiosize/COLLAPSE;
  int X = size/(period2*meass)-1;
  int x = 1;
  unsigned4 last = 0;
  unsigned4 db = period2 * meass / 32;
  // first we create the projection...
  signed4 *project;
  project = allocate(period2*meass,signed4);
  for(unsigned int x = 0 ; x < period2 * meass ; x ++)
    project[x]=0;
  for(unsigned int x = 0 ; x < size ; x ++)
    project[x%(period2*meass)]+=data[x];
  
  // signed4 maximum=0;
  // for(int y = 0 ; y < period2 * meass ; y ++)
  // if (project[y]>maximum)
  // maximum=project[y];
  // for(int y = 0 ; y < period2 * meass ; y ++)
  //project[y]=((signed8)project[y])*(signed8)255/(signed8)maximum;

  for(unsigned int y = 0 ; y < period2 * meass ; y ++)
    project[y]/=X;
  
  // for every period...
  while(x<phasesize)
    {
      unsigned4 now = last + period2 * meass;
      unsigned4 lo_now = now - db;
      unsigned4 hi_now = now + db;
      if (hi_now+period2*meass>size)
	break;
      // find best phasephit
      signed4 mindiffat = 0;
      unsigned4 mindiff = (unsigned4)-1;
      for(unsigned int d = lo_now ; d < hi_now; d ++)
	{
	  unsigned4 diff = 0;
	  for(unsigned int y = 0 ; y < period2 * meass ; y ++)
	    diff += abs((signed int)data[d+y]-
			(signed int)project[y]);  //data[last+y]);
	  if (diff<mindiff)
	    {
	      mindiff=diff;
	      mindiffat = d;
	    }
	}
      // assign best phasephit to delta...
      phases[x] = mindiffat - period2 * meass * x;
      // now we should be able to dump it again...
      printf("Fitting column %d : %d\n",x,phases[x]);
      fflush(stdout);
      last = mindiffat;
      x++;
    }
  showPattern();
}

void PatternAnalyzerLogic::slantChanged()
{
  period=normalperiod+periodDelta->value()+periodDelta10->value();
  if (period<=0) period=1;
  showPattern();
}

void PatternAnalyzerLogic::balanceChanged()
{
  showPattern();
}

void PatternAnalyzerLogic::setTempo()
{
  normalperiod = normalperiod+periodDelta->value()+periodDelta10->value();
  if (normalperiod<=0) normalperiod = 1;
  ::y = ::x * currentperiod / normalperiod;
  periodDelta->setValue(0);
  periodDelta10->setValue(0);
  index_period = normalperiod/4;
  index_changed = 1;
  // immediatelly write to disk
  index_write();
}
