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
#ifndef __BPMDJ___MUSIC_SCANNER_H__
#define __BPMDJ___MUSIC_SCANNER_H__
using namespace std;
#line 1 "music-scanner.h++"
#include "ui-importing.h"
#include "selector.h"
#include "dirscanner.h"

class QString;

class MusicScanner:
  public QDialog,
  public DirectoryScanner,
  public Ui::ImportProgress
{
  Q_OBJECT
  SongSelectorLogic * selector;
  map<QString,Song*> database;
protected:
  virtual void recursing(const QString dirname);
  virtual void checkfile(const QString pathname, const QString filename);
  virtual bool matchextension(const QString filename);
public:
  MusicScanner(SongSelectorLogic* sroot);
};
#endif
