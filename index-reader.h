/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
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

#include "dirscanner.h"
#include "avltree.h"

class SongSelectorLogic;
class Song;
class QProgressBar;
class QLabel;
class DataBase;

class IndexReader:
  public DirectoryScanner
{
 public: 
  int total_files;
 protected:
  bool reading_bib;
  QProgressBar * progress;
  QLabel * reading;
  virtual void checkfile(const QString prefix, const QString filename);
  virtual void recursing(const QString dirname);
  void add(Song * song);
  DataBase * database;
  AvlTree<QString> tree;
 public:
  IndexReader(QProgressBar * b, QLabel * l, DataBase*);
};
