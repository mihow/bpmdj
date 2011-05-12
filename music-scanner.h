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
#ifndef __loaded__music_scanner_h__
#define __loaded__music_scanner_h__
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
#endif // __loaded__music_scanner_h__
