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
#ifndef __loaded__analyzer_impl_h__
#define __loaded__analyzer_impl_h__
using namespace std;
#include <boost/smart_ptr.hpp>
#include <vector>
#include <map>
#include <qstring.h>
#include <typeinfo>
#include "common.h"
using namespace std;

class AnalyzerProgress
{
public:
  virtual void status(const char* name)=0;
  virtual void progress(int a, int t)=0;
  virtual void finished()=0;
};

struct Axis
{
  float8 min;
  float8 max;
  string description;
  float8 show_grid;
  Axis(const char* label);
  void val(float8 v);
};

struct LineData
{
  map<float8, float8> xy;
  Axis* X;
  Axis* Y;
  string description;
  LineData();
  void point(float8 x, float8 y);
};

struct AnalyzerPlot
{
  vector<Axis*> allocated;
  map<int, LineData > line_xy;
  virtual void point(int z, float8 x, float8 y);
  Axis* axis(const char* tmpstr);
  void axes(int z, Axis* X, Axis* Y, string description);
  void clear_plot_data();
};

/**
 * The essential content of an analysis class is that it can report
 * its progress and that it can work in a stepwise fashion.
 */
class AnalyzerImpl
{
protected:
  AnalyzerProgress* progress_impl;
  AnalyzerPlot* plot;
  
  void status(const char* name, ...);
  void progress(int a, int b)
  {
    if (progress_impl) progress_impl->progress(a,b);
  }
  Axis* axis(const char* name, ...);
  void axes(int z, Axis* X, Axis* Y, string description)
  {
    if (plot) plot->axes(z,X,Y,description);
  }
  void point(int z, float8 x, float8 y)
  {
    if (plot) plot->point(z,x,y);
  }
  void finished()
  {
    if (progress_impl) progress_impl->finished();
  }
public:
  AnalyzerImpl();
  void attach_progress(AnalyzerProgress* p);
  void attach_plot(AnalyzerPlot* p);
  virtual bool step()=0;
  virtual ~AnalyzerImpl();
};

typedef boost::shared_ptr<AnalyzerImpl> AnalyzerAlg;

//---------------------------------------------------------
// A helper class to deal with a textual output
//---------------------------------------------------------
class TextualAnalyzerProgress: public AnalyzerProgress
{
  int last_progress;
public:
  TextualAnalyzerProgress();
  virtual void status(const char*);
  virtual void progress(int a, int t);
  virtual void finished();
};
#endif // __loaded__analyzer_impl_h__
