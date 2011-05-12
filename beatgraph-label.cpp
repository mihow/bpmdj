/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "beatgraph-label.c++"
#include <qpixmap.h>
#include "beatgraph-label.h"
#include "player-core.h"

static QPen ruler_pen(QColor(255,255,0), 3);
void BeatGraphLabel::drawCursor()
{
  QPixmap *pm = pixmap();
  if (!pm) return;
  int window_xsize = pm->width();
  if (window_xsize>1)
    {
      int pos = ::x/samples_per_column;
      pos=pos*(width()-1)/(window_xsize-1);
      if (oldpos==pos)
	return;
      QPainter paint(this);
      paint.setPen(ruler_pen);
      paint.setRasterOp( Qt::XorROP);
      if (oldpos>-1)
	paint.drawLine(oldpos,0,oldpos,height());
      paint.drawLine(pos,0,pos,height());
      oldpos=pos;
      paint.end();
    }
  else
    {
      QPainter paint(this);
      paint.setPen(ruler_pen);
      paint.setRasterOp( Qt::XorROP);
      if (oldpos>-1)
	paint.drawLine(oldpos,0,oldpos,height());
      oldpos=-1;
      paint.end();
    }
}

void BeatGraphLabel::drawRuler()
{
  if (oldpos>-1)
    {
      QPainter paint(this);
      paint.setPen(ruler_pen);
      paint.setRasterOp(Qt::XorROP);
      if (oldpos>-1)
	paint.drawLine(oldpos,0,oldpos,height());
      paint.end();
    }
}

void BeatGraphLabel::drawCueText()
{
  QPixmap *pm = pixmap();
  if (!pm) return;
  int window_xsize = pm->width();
  QPainter paint(this);
  QColor cue_to_color[5];
  cue_to_color[0]=QColor(255,255,255);
  cue_to_color[1]=QColor(255,0,0);
  cue_to_color[2]=QColor(0,255,0);
  cue_to_color[3]=QColor(0,0,255);
  cue_to_color[4]=QColor(255,255,0);
  const int size = 10;
  for(int i = -1 ; i < 4 ; i ++)
    {
      cue_info c = i==-1 ? ::cue : ::cues[i];
      if (c==0) continue;
      QPen cue_pen(cue_to_color[i+1], 2);
      paint.setPen(cue_pen);
      int x = c/samples_per_column;
      int y = c-x*samples_per_column;
      x=x*(width()-1)/(window_xsize-1);
      y=y*(height()-1)/samples_per_column;
      paint.drawLine(x-size,y,x+size,y);
      paint.drawLine(x,y-size,x,y+size);
    }
  paint.end();
}

void BeatGraphLabel::paintEvent(QPaintEvent *e)
{
  QLabel::paintEvent(e);
  drawCueText();
  drawRuler();
}

BeatGraphLabel::BeatGraphLabel(QWidget * parent, const char * name) : 
  QLabel(parent,name)
{
  oldpos=-1;
  setScaledContents(true);
  connect(&timer,SIGNAL(timeout()),SLOT(drawCursor()));
};

void BeatGraphLabel::setPixmap (const QPixmap &pm, unsigned8 samplespercol)
{
  QLabel::setPixmap(pm);
  samples_per_column=samplespercol;
  timer.start(333);
}

BeatGraphLabel::~BeatGraphLabel()
{
  timer.stop();
}
