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
#ifndef __loaded__beatgraph_label_cpp__
#define __loaded__beatgraph_label_cpp__
using namespace std;
#line 1 "beatgraph-label.c++"
#include <Qt/qpixmap.h>
#include <QtGui/QPaintEvent>
#include <iostream>
#include "beatgraph-label.h"
#include "player-core.h"
#include "clock-drivers.h"

/*
class AutoMessage
{
  QString m;
public:
  AutoMessage(QString m2): m(2)
  {
    cerr << "entering " << m.toStdString() << "\n";
  };
  ~AutoMessage()
  {
    cerr << "leaving " << m.toStdString() << "\n";
  };
};
*/

void BeatGraphLabel::xorLine(int ox, int&x1, int&x2)
{
  int osx = original.width();
  if (ox>=osx) return;
  int sx = resized.width();
  static QRgb mask=QColor(255,255,0).rgb();
  x1 = -1, x2 = -1;
  if (osx<=1) return; // div by zero 
  x1 = ox*sx/osx; 
  x2 = ((ox+1)*sx/osx);
  if (x1>=sx) x1=sx;
  if (x2>=sx) x2=sx;
  if (x1==x2) return;
  assert(x1<=x2);
  for(int y = 0 ; y < resized.height(); y++)
    {
      QRgb* l = ((QRgb*)resized.scanLine(y));
      for(int x = x1; x < x2 ; x++)
	l[x]^=mask;
    }
}

void BeatGraphLabel::moveRuler(int &x1, int&x2,int &x3, int &x4)
{
  if (resized.isNull()) return; 
  int pos = ::x_diskrate/samples_per_column_diskrate;
  x1=x2=x3=x4=-1;
  if (ruler_x==pos) return;
  if (ruler_x>-1)
    xorLine(ruler_x,x1,x2);
  ruler_x=pos;
  xorLine(ruler_x,x3,x4);
}

void BeatGraphLabel::drawCursor()
{
  //  AutoMessage("drawCursor");
  QMutexLocker _ml(&lock);
  int x1,x2,x3,x4;
  moveRuler(x1,x2,x3,x4);
  if (x1==-1)
    {
      if (x3==-1) return;
      repaint(QRect(x3,0,x4-x3+1,resized.height()));
    }
  else
    {
      if (x3==-1)
	repaint(QRect(x1,0,x2-x1+1,resized.height()));
      else
	{
	  int a = ::min(x1,x3);
	  int b = ::max(x2,x4);
	  if (b-a<2*(x2-x1+x4-x3))
	    repaint(QRect(a,0,b-a+1,resized.height()));
	  else
	    {
	      repaint(QRect(x3,0,x4-x3+1,resized.height()));
	      repaint(QRect(x1,0,x2-x1+1,resized.height()));
	    }
	}
    }
}

void BeatGraphLabel::drawCueText()
{
  if (!(resized.width()>1 && samples_per_column_diskrate>1)) return;
  QPainter paint;
  paint.begin(&resized);
  QColor cue_to_color[5];
  cue_to_color[0]=QColor(255,255,255);
  cue_to_color[1]=QColor(255,0,0);
  cue_to_color[2]=QColor(0,255,0);
  cue_to_color[3]=QColor(0,0,255);
  cue_to_color[4]=QColor(255,255,0);
  int tx=resized.width();
  for(int i = -1 ; i < 4 ; i ++)
    {
      cue_info c = i==-1 ? ::cue_metarate : ::cues_metarate[i];
      if (c==0) continue;
      QPen cue_pen(cue_to_color[i+1], 2);
      paint.setPen(cue_pen);
      int x = metarate_to_diskrate(c)/samples_per_column_diskrate;
      int y = metarate_to_diskrate(c)-x*samples_per_column_diskrate;
      x=x*tx/original.width();
      y=y*resized.height()/samples_per_column_diskrate;
      paint.drawLine(x-10,y,x+10,y);
      paint.drawLine(x,y-10,x,y+10);
    }
  paint.end();
  update();
}

void BeatGraphLabel::paintEvent(QPaintEvent *e)
{
  QMutexLocker _ml(&lock);
  if (resized.isNull())
    {
      QPainter p;
      p.begin(this);
      p.fillRect(e->rect(),Qt::black);
      p.end();
      return;
    }
  if (cues_changed)
    {
      cues_changed=false;
      if (!original.isNull())
	{
	  setImage(original,samples_per_column_diskrate);
	  int a,b,c,d;
	  moveRuler(a,b,c,d);
	}
    }
  QPainter p;
  p.begin(this);
  p.drawImage(e->rect(),resized,e->rect());
  p.end();
}

void BeatGraphLabel::resizeEvent(QResizeEvent * event)
{
  // AutoMessage("resizeEvent");
  QMutexLocker _ml(&lock);
  cues_changed=true;
  update();
}

BeatGraphLabel::BeatGraphLabel(QWidget * parent, const char * name) : 
  QWidget(parent), lock(QMutex::Recursive), original(), resized()
{
  // AutoMessage("constructor");
  QMutexLocker _ml(&lock);
  setWindowTitle(name);
  setAttribute(Qt::WA_OpaquePaintEvent);
  ruler_x=-1;
  cues_changed=false;
  samples_per_column_diskrate=1;
  connect(&timer,SIGNAL(timeout()),SLOT(drawCursor()));
};

void BeatGraphLabel::setImage(QImage image, unsigned8 samplespercol)
{
  // AutoMessage("setImage");
  QMutexLocker _ml(&lock);
  original=image;
  resized=original.scaled(size());
  drawCueText();
  ruler_x = -1;
  samples_per_column_diskrate=samplespercol;
  timer.start(333);
}

BeatGraphLabel::~BeatGraphLabel()
{
  timer.stop();
}
#endif // __loaded__beatgraph_label_cpp__
