/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__qsong_h__
#define __loaded__qsong_h__
using namespace std;
#line 1 "qsong.h++"
#include <qcolor.h>
#include "qvectorview.h"
#include "config.h"
#include "cluster.h"
#include "song.h"
#include "data.h"

#define LIST_TEMPO 0
#define LIST_DCOLOR 2
#define LIST_TITLE 5
#define LIST_AUTHOR 6
#define LIST_TIME 3
#define LIST_CUES 4
#define LIST_VERSION 7
#define LIST_TAGS 8
#define LIST_ONDISK 9
#define LIST_SPECTRUM 1
#define LIST_INDEX 10
#define LIST_MD5SUM 11
#define LIST_FILE 12
#define LIST_MIN 13
#define LIST_MAX 14
#define LIST_MEAN 15
#define LIST_POWER 16
#define LIST_HISTOGRAM 17
#define LIST_RHYTHM 18
#define LIST_COMPOSITION 19

#define singleton_array_accessors(type,type2,var)  \
  private: static type _##var; \
  private: static inline type get_##var() { return _##var; }; \
  void static inline set_##var(type a) { _##var = a; }; \
  public: static inline type2 get_##var(int idx) { return _##var[idx];}; \
  public: static inline void set_##var(int idx, type2 v) {_##var[idx]=v;};

class QSong: public QVectorViewData
{
  singleton_array_accessors(bool*, bool, selected);
  singleton_array_accessors(Song **, Song *, songs);
  singleton_accessors(int,compare_col);
  singleton_accessors(bool,compare_asc);
  singleton_accessors(int,song_count);
public:
  static void setVector(Song** arr, int cnt);
  static void addVector(Song** arr, int cnt);
  virtual int vectorSize() const;
  virtual bool isSelected(int i) const 
  {
    assert(i>=0); 
    return get_selected(i); 
  };
  virtual void setSelected(int i, bool val=true) 
  {
    get_selected()[i]=val;
  };
  virtual void sort(int col, bool ascending);
  virtual void paintCell(QVectorView* vv, int i, QPainter *p, 
			 const QColorGroup &cg, int col, int wid, int align);
  virtual QString text(int j, int i) const 
  {
    return Text(get_songs(j),i); 
  };
  static  void Sort();
  QSong();
  static Song * songEssence(int i);
  static QColor * colorOfTempoCol(const Song* main, Song* song);
  static QColor * colorOfAuthorCol(Song * song);
  static QColor * colorOfPlaycount(Song * song);
  static QColor * colorOfdColorCol(Song* song);
  static void playedAuthorAtTime(int i, int t) 
  {
    get_songs(i)->set_played_author_at_time(t);
  };
  static QString Text(Song * j, int i);
};
#endif // __loaded__qsong_h__
