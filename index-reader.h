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
#ifndef __BPMDJ___INDEX_READER_H__
#define __BPMDJ___INDEX_READER_H__
using namespace std;
#line 1 "index-reader.h++"
#include "dirscanner.h"

class SongSelectorLogic;
class Song;
class QProgressBar;
class QLabel;
class BasicDataBase;

class IndexReader: public DirectoryScanner
{
  protected:
    int  step_size;
    QProgressBar  * progress;
    QLabel        * reading;
    BasicDataBase * database;
    virtual void checkfile(const QString prefix, const QString filename);
    virtual void recursing(const QString dirname);
    void add(Song * song);
  public:
    int total_files;
    int idx_files;
    IndexReader(QProgressBar * b, QLabel * l, BasicDataBase*, int expected);
};
#endif
