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
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <qmessagebox.h>
#include "pattern-analyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "index.h"
#include "version.h"
#include "scripts.h"

#define COLLAPSE 4
PatternAnalyzerLogic::PatternAnalyzerLogic(bool showreaderprogress, SongPlayer*parent, const char*name, bool modal, WFlags f) :
  PatternDialog(0,name,modal,f)
{
  period=normalperiod;
  data = NULL;
  readFile(showreaderprogress);
  showPattern();
}

void PatternAnalyzerLogic::readFile(bool showreaderprogress)
{
  if (data)
    return;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  // read in memory and shrink it 
  FILE * raw=openRawFile(arg_rawpath);
  audiosize=fsize(raw)/4;
  if (audiosize == 0) return;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize/COLLAPSE,compressed);
  
  QProgressDialog * progress = NULL;
  if (showreaderprogress) 
    {
      progress = new QProgressDialog();
      progress->setTotalSteps(audiosize/bufsize);
      progress->show();
    }

  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      if (progress) progress->setProgress(pos/bufsize);
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
  if (maximum==0)
    {
      audiosize=0;
      return;
    }
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    data[i]=(signed8)data[i]*(signed8)255/maximum;
}

void PatternAnalyzerLogic::showPattern()
{ 
  if (!audiosize)
    {
      QMessageBox::warning(this,"Fragment too small","There is simply no raw data on disk,\nHence, I can't display the beat graph");
      return;
    }
  if (!period)
    {
      QMessageBox::warning(this,"No period estimate","No period estimate, hence cannot show the beat graph.\n"
			   "Please go to the bpm counter and measure the tempo first");
      return;
    }
  int beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  array(project, window_ysize, signed4);
  for(int row =  0 ; row < window_ysize ; row ++)
    project[row]=0;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      if (::x/(normalperiod*beats_per_column/4) == column)
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,0,0));
	    p.drawPoint(column,row);
	  }
      else
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,val,val));
	    p.drawPoint(column,row);
	  }
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  for(int row = 0 ; row < window_ysize ; row ++)
    if (project[row]>maximum)
      maximum=project[row];
  for(int row = 0 ; row < window_ysize ; row ++)
    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  for(int row = 0 ; row < window_ysize ; row++)
    {
      int val;
      val = project[row];
      l.setPen(QColor(val,val,val));
      l.drawPoint(0,row);
    }
  l.end();
  projection->setPixmap(*sm);
}

/*void PatternAnalyzerLogic::run()
{ 
  unsigned int row, beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4 * COLLAPSE;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  array(pattern,collapsed_period, double);
  for(row =  0 ; row < collapsed_period ; row ++)
    pattern[row]=0;
  for(unsigned4 pos = 0 ; pos < collapsed_size ; pos ++)
    pattern[pos%collapsed_period]+=data[pos];
  double maximum = 0;
  for(row = 0 ; row < collapsed_period ; row ++)
    if (fabs(pattern[row])>maximum) maximum = fabs(pattern[row]);
  for(row = 0 ; row < collapsed_period ; row ++)
    pattern[row]=fabs(pattern[row]*255.0/maximum);
  for(row = 0 ; row < collapsed_period ; row ++)
    if (pattern[row]==255.0)
      break;
  int phase = row;
*/
  // hier zitten we met het probleem van de boel op te slaan op disk...
  // we willen dat zo klein mogelijk...
  // en toch zoveel mogelijk nuttige informatie behouden...
  // die textfiles 
  // in karakters kan ik het misschien der nog bijproppen
/*  array(pattern_for_index,collapsed_period , unsigned char);
  for(row = 0 ; row < collapsed_period ; row ++)
    pattern_for_index[row]=(unsigned char)pattern[(phase+row)%collapsed_period];
  index_pattern = pattern_for_index;
  index_pattern_size = collapsed_period;
  index_changed = 1;
  index_write();
}

void PatternAnalyzerLogic::dumpPatternToIdx()
{
  run();
}

*/

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
  Index::index->set_period(normalperiod/4);
}
