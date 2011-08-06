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
#ifndef __loaded__analyzer_progress_qt_cpp__
#define __loaded__analyzer_progress_qt_cpp__
using namespace std;
#include <math.h>
#include <set>
#include <qpainter.h>
#include "analyzer-progress-qt.h"
using namespace std;

float8 grid(Axis* a)
{
  float8 delta=a->max-a->min;
  float8 candidate1=pow(10,trunc(log(delta)/log(10)));
  float8 candidate2=pow(10,trunc(log(delta)/log(10))-1);
  int tick_c1=delta/candidate1;
  int tick_c2=delta/candidate2;
  if (fabs(tick_c1-10)<fabs(tick_c2-10)) 
    return candidate1;
  else
    return candidate2;
}

void QAnalyzerProgress::paint(QPaintDevice* target)
{
  QPainter painter;
  painter.begin(target);
  QRect arearect(QRect(0,0,target->width(),target->height()));
  painter.fillRect(arearect,Qt::white);
  if (line_xy.size())
    {
      // first draw the axes at both sides
      set<Axis*> horizontals;
      set<Axis*> verticals;
      for(map<int, LineData>::iterator it=line_xy.begin(); it!=line_xy.end(); it++)
	{
	  horizontals.insert(it->second.X);
	  verticals.insert(it->second.Y);
	}
      int lineheight=painter.fontMetrics().height();
      int space_needed_bottom=lineheight*2*horizontals.size();  // twice because we want to print the axis description as well
      int space_needed_left=0;
      int sx=target->width()-space_needed_left;
      int sy=target->height()-space_needed_bottom;
      int bottomline=sy;
      int leftline=space_needed_left;
      // draw the grid 
      painter.setPen(Qt::DotLine);
      for(int horizontal = 0; horizontal < 2; horizontal++)
	{
	  set<Axis*>* S=horizontal ? & horizontals : & verticals;
	  int axis_no=0;
	  for(set<Axis*>::iterator it=S->begin(); it!=S->end(); it++)
	    {
	      Axis* A=*it;
	      if (!A->show_grid) continue;
	      float8 stepsize=grid(A);
	      if (!stepsize) continue;
	      float8 start=(round(A->min/stepsize)-1)*stepsize;
	      float8 stop=(round(A->max/stepsize)+1)*stepsize;
	      float8 delta=A->max-A->min;
	      if (delta==0) delta=1;
	      for(float8 cur=start; cur<=stop; cur+=stepsize)
		{
		  float relpos=(cur-A->min)/delta;
		  if (relpos<0 || relpos>1) continue;
		  if (horizontal)
		    {
		      int x=leftline+relpos*sx;
		      painter.drawLine(x,0,x,bottomline);
		      painter.drawText(QRect(x-100,bottomline,200,lineheight),Qt::AlignTop|Qt::AlignHCenter,QString::number(cur));
		    }
		  else
		    {
		      int y=bottomline-1-relpos*sy;
		      painter.drawLine(leftline,y,leftline+sx,y);
		      painter.drawText(QRect(0,y-lineheight/2,leftline,lineheight),Qt::AlignRight|Qt::AlignVCenter,QString::number(cur));
		    }
		}
	      if (horizontal)
		painter.drawText(QRect(leftline,bottomline+lineheight*(1+2*axis_no),sx,lineheight),Qt::AlignTop|Qt::AlignRight,QString(A->description.c_str()));
	    }
	}

      // draw border around plot
      painter.setPen(Qt::black);
      painter.drawRect(QRect(leftline,0,sx,sy));
      
      // then in the remaining area we can draw our curves
      painter.setPen(Qt::SolidLine);
      int min_z=line_xy.begin()->first;
      int max_z=(--line_xy.end())->first;
      int z_count=max_z-min_z;
      if (z_count==0) z_count=1;
      
      for(map<int, LineData>::iterator it=line_xy.begin(); it!=line_xy.end(); it++)
	{
	  float8 min_x=it->second.X->min;
	  float8 max_x=it->second.X->max;
	  float8 min_y=it->second.Y->min;
	  float8 max_y=it->second.Y->max;
	  float8 fac_x=max_x-min_x;
	  if (fac_x==0) fac_x=1;
	  else fac_x=(float8)sx/fac_x;
	  float8 fac_y=max_y-min_y;
	  if (fac_y==0) fac_y=1;
	  else fac_y=(float8)sy/fac_y;
	  
	  int curve_hue=(it->first-min_z)*240/z_count;
	  QColor curve_color;
	  curve_color.setHsv(curve_hue,255,255);
	  painter.setPen(curve_color);
	  LineData& curve=it->second;
	  QPoint previous(-1,-1);
	  for(map<float8,float8>::iterator point=curve.xy.begin(); point!=curve.xy.end(); point++)
	    {
	      float8 x=(point->first-min_x)*fac_x;
	      float8 y=(point->second-min_y)*fac_y;
	      QPoint newpoint(leftline+x,bottomline-1-y);
	      if (previous.x()==-1)
		painter.drawPoint(newpoint);
	      else
		painter.drawLine(previous,newpoint);
	      previous=newpoint;
	    }
	}
      
      // print curve labels topright
      int currentline=0;
      for(map<int, LineData>::iterator it=line_xy.begin(); it!=line_xy.end(); it++)
	{
	  int curve_hue=(it->first-min_z)*240/z_count;
	  QColor curve_color;
	  curve_color.setHsv(curve_hue,255,255);
	  painter.setPen(curve_color);
	  painter.drawText(QRect(leftline,currentline,sx,lineheight),Qt::AlignRight|Qt::AlignVCenter,QString(it->second.description.c_str()));
	  currentline+=lineheight;
	}
      // painter.drawText(arearect,Qt::AlignCenter,"test");
      // QString("(%1,%2)").arg(min_x).arg(min_y));
      // painter.drawText(arearect,Qt::AlignRight|Qt::AlignTop,QString("(%1,%2)").arg(max_x).arg(max_y));
    }
  painter.end();
}
#endif // __loaded__analyzer_progress_qt_cpp__
