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
#ifndef __loaded__qsong_h__
#define __loaded__qsong_h__
using namespace std;
#include <qabstractitemmodel.h>
#include <qtableview.h>
#include <qcolor.h>
#include "config.h"
#include "song.h"

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
#define LIST_COLCOUNT 20

#define singleton_array_accessors(type,type2,var)			\
  private: static type _##var;						\
private: static inline type get_##var() { return _##var; };		\
  void static inline set_##var(type a) { _##var = a; };			\
public: static inline type2 get_##var(int idx) { return _##var[idx];};	\
public: static inline void set_##var(int idx, type2 v) {_##var[idx]=v;};

class QSong: public QAbstractTableModel
{
  singleton_array_accessors(Song **, Song *, songs);
  singleton_accessors(int,compare_col);
  singleton_accessors(bool,compare_asc);
  singleton_accessors(int,song_count);
public:
  void setVector(Song** arr, int cnt);
  void addVector(Song** arr, int cnt);
  virtual void sort(int col, bool ascending);
  static  void Sort();
  QSong();
  static Song * songEssence(int i);
  static Song * songEssence(const QModelIndex& i);
  static QColor colorOfTempoCol(const Song* main, Song* song);
  static QColor colorOfAuthorCol(Song * song);
  static QColor colorOfPlaycount(Song * song);
  static QColor colorOfdColorCol(Song* song);
  static QColor normalColor(Song* song);
  static QString text(Song* song, int col);
  static void playedAuthorAtTime(int i, int t) 
  {
    get_songs(i)->set_played_author_at_time(t);
  }; 
  virtual QVariant headerData(int col, Qt::Orientation orientation, int role) const;
  virtual int rowCount(const QModelIndex&) const;
  virtual int columnCount(const QModelIndex&) const;
  virtual QVariant data(const QModelIndex&, int) const;
  void aSongChangedBehindQSongsBack(Song* song);
  static QModelIndex indexof(Song* song);
};

Song * get_current_song();
/**
 * If scrolltoit is true then the view will ensure the item is visible
 * (scrollto) and the item will be selected directly (or not ?)
 */
void set_current_song(Song*, bool scrolltoit);
#endif // __loaded__qsong_h__
