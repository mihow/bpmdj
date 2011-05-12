/****
 Duploc: 
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

#include "duploc-dialog.h"

class Duploc : public DuplocDialog
{
  Q_OBJECT
public:
  char * * line;
  char * * orig;
  int lastline;
  int lines;
  bool **matrix;
  bool *keepx;
  bool *keepy;
  int   keepxc;
  int   keepyc;
  int   *mapx;
  int   *mapy;
  Duploc(QWidget *parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  void readFiles();
  void readFile(char* f);
  void addline(char* fn, int nr, char*o, char*l);
  void dump();
  void correlate();
  void fold();
  virtual void mousePressEvent(QMouseEvent * e);
public slots:
  virtual void showPattern();
};
