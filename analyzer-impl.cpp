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
#ifndef __loaded__analyzer_impl_cpp__
#define __loaded__analyzer_impl_cpp__
using namespace std;
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "analyzer-impl.h"

AnalyzerImpl::AnalyzerImpl(): progress_impl(NULL), plot(NULL)
{
}

void AnalyzerImpl::attach_progress(AnalyzerProgress* p)
{
  progress_impl=p;
}

void AnalyzerImpl::attach_plot(AnalyzerPlot* p)
{
  plot=p;
}

AnalyzerImpl::~AnalyzerImpl()
{
};

TextualAnalyzerProgress::TextualAnalyzerProgress()
{
  last_progress=-1;
}
  
void AnalyzerImpl::status(const char* fstr, ...)
{
  if (!progress_impl) return;
  char text[1024];
  va_list ap;
  va_start(ap,fstr);
  vsnprintf(text,1024,fstr,ap);
  va_end(ap);
  progress_impl->status(text);
}

Axis* AnalyzerImpl::axis(const char* fstr, ...)
{
  if (!plot) return NULL;
  char text[1024];
  va_list ap;
  va_start(ap,fstr);
  vsnprintf(text,1024,fstr,ap);
  va_end(ap);
  return plot->axis(text);
}

Axis* AnalyzerPlot::axis(const char* tmpstr)
{
  Axis* answer=new Axis(tmpstr);
  allocated.push_back(answer);
  return answer;
}

void TextualAnalyzerProgress::status(const char* text)
{
  if (last_progress!=-1)
    {
      printf("\r");
      last_progress=-1;
    }
  printf("%s\n",text);
}

void TextualAnalyzerProgress::progress(int t, int m)
{
  int pct=t*100/m;
  if (pct!=last_progress)
    {
      printf("\r%d%%  ",pct);
      fflush(stdout);
      last_progress=pct;
    }
}

void TextualAnalyzerProgress::finished()
{
}

LineData::LineData(): X(NULL), Y(NULL)
{
}

Axis::Axis(const char* lable): description(lable), show_grid(false)
{
  min=nan("crap");
  max=nan("crap");
}

void Axis::val(float8 v)
{
  if (isnan(min)) min=max=v;
  if (v<min) min=v;
  if (v>max) max=v;
}

void LineData::point(float8 x, float8 y)
{
  assert(X);
  assert(Y);
  X->val(x);
  Y->val(y);
  xy[x]=y;
}

void AnalyzerPlot::axes(int z, Axis* X, Axis* Y, string description)
{
  line_xy[z].X=X;
  line_xy[z].Y=Y;
  line_xy[z].description=description;
}

void AnalyzerPlot::point(int z, float8 x, float8 y)
{
  line_xy[z].point(x,y);
}

void AnalyzerPlot::clear_plot_data()
{
  line_xy.clear();
  for(unsigned2 i = 0 ; i <allocated.size(); i++)
    delete allocated[i];
  allocated.clear();
}
#endif // __loaded__analyzer_impl_cpp__
