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

#include "songplayer.h"
#include "pattern-analyzer.h"

typedef unsigned char compressed;
class PatternAnalyzerLogic : public PatternDialog
{
  Q_OBJECT
private:
  int     filtersize;
  double *filter;
  double  lo;
  double  hi;
  double  g_frequency_response(double freq);
  double  db_frequency_response(double freq, double clip);
  void    showFilter();
  double  getGain(double freq);
  double  inverseDtft(double, double);
  double *doubleFilter(double *, int&);
  void    createFilter();
public:
  long period;
  long audiosize;
  compressed * data;
  PatternAnalyzerLogic(bool showreaderprogress, SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  void readFile(bool showreaderprogress);
  // virtual void run();
public slots:
  virtual void showPattern();
  virtual void balanceChanged();
  virtual void slantChanged();
  virtual void setTempo();
  virtual void filterChanged();
  // virtual void dumpPatternToIdx();
  // run will calculate the projection of the pattern and dump it into the index file...
};
