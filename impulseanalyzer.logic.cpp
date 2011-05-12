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
#include <pthread.h>
#include "impulseanalyzer.logic.h"
#include "sys/times.h"
#include "fourier.h"
#include "kbpm-play.h"
#include "version.h"
#include "scripts.h"

ImpulseAnalyzerLogic::ImpulseAnalyzerLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  ImpulseDialog(0,name,modal,f)
{
  long int bufsize = 65536;
  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
  audiosize=fsize(raw)/4;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize,longtrick);
  QProgressDialog * progress = new QProgressDialog();
  progress->setTotalSteps(audiosize);
  progress->show();
  while(pos<audiosize)
    {
      pos += readsamples((unsigned4*)data+pos,bufsize,raw);
      progress->setProgress(pos);
      app->processEvents();
    }
  fclose(raw);
  delete(progress);
  showImpulse();
}

void ImpulseAnalyzerLogic::showImpulse()
{ 
  int X = 80;
  double correlation[X];
  for(int i = 0 ; i <X; i ++)
    correlation[i]=-1;
  int Y = impulse->contentsRect().height();
  int dc = color->value();
  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.setPen(QColor(0,0,0));
  double *squared;
  double *doubled;
  squared = allocate(audiosize,double);
  doubled = allocate(audiosize,double);
  for(int i = 0 ; i < audiosize  ; i ++)
    {
      squared[i]=doubled[i]=(double)data[i].leftright.left;
      squared[i]*=squared[i];
    }
  double covar[X];
  double devx[X];
  double devy[X];
  for(int i = 0 ; i < X ; i ++)
    {
      devx[i]=0;
      devy[i]=0;
      covar[i]=0;
    }
  
  QProgressDialog * progress = new QProgressDialog();
  progress->setTotalSteps(audiosize);
  progress->show();
  for(int i = 0 ; i < audiosize ; i ++)
    {
      if (!(i%44100))
	{
	  progress->setProgress(i);
	  app->processEvents();
	}
      double xi = doubled[i];
      double s = squared[i];
      for(int x = 0 ; x < X && i-x >= 0; x++)
	{
	  double yi = doubled[i-x];
	  covar[x] += xi*yi;
	  devx[x] += s;
	  devy[x] += squared[i-x];
	}
    }
  for(int x = 0 ; x < X; x ++)
    {
      covar[x]/=(double)audiosize;
      devx[x]/=(double)audiosize;
      devy[x]/=(double)audiosize;
      devx[x]=sqrt(devx[x]);
      devy[x]=sqrt(devy[x]);
      double cor = covar[x]/(devx[x]*devy[x]);
      correlation[x]=cor;
      printf("%g\n",cor);
      cor/=2;
      cor*=Y;
      p.drawPoint(x,(int)(Y/2-cor));
    }

  /*  for(int x = 0 ; x < X && cor > 0; x++)
    {
      double covar = 0;
      double devx = 0;
      double devy = 0;
      for(int i = 0 ; i < audiosize - x ; i ++)
	{
	  // hoe kunnen we ervoor zorgen dat de grotere signalen meer impact hebben.
	  // de 0-achtige signalen betekenen uiteindelijk niet echt zoveel...
	  double xi = doubled[i];
	  double yi = doubled[i-x];
	  covar += xi*yi;
	  devx += squared[i];
	  devy += squared[i-x];
	}
      covar/=(double)audiosize;
      devx/=(double)audiosize;
      devy/=(double)audiosize;
      devx=sqrt(devx);
      devy=sqrt(devy);
      cor = covar/(devx*devy);
      correlation[x]=cor;
      printf("%g\n",cor);
      cor/=2;
      cor*=Y;
      p.drawPoint(x,Y/2-cor);
    }
  */

  p.setPen(QColor(0,(128+dc)%256,0));
  for(int y = 0 ; y < Y ; y+=Y/2)
    p.drawLine(0,y,X-1,y);
  p.end(); 
  impulse->setPixmap(*pm);
  free(squared);
  free(doubled);
  FILE * f = fopen("impulse.doubles","wb");
  fwrite(correlation,sizeof(double),X,f);
  fclose(f);
}

void ImpulseAnalyzerLogic::balanceChanged()
{
  showImpulse();
}
