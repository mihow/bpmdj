/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__history_h__
#define __loaded__history_h__
using namespace std;
#line 1 "history.h++"
#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
#include <qtreewidget.h>
#include "song.h"
#include "database.h"

/**
 * A singleton class to keep track of the playing history
 */
class History
{
private:
  static Song * t_2;  // T - 2
  static Song * t_1;  // T - 1
  static Song * t_0;  // T
  static FILE* file;
  static QByteArray filename;
  singleton_accessors(unsigned4,songs_played);
  static QTreeWidget * log_ui;
  static void mark_as_played(DataBase * db, QString s);
  static void mark_as_played(Song *song);
public:
  static void mark_all_played_songs(DataBase * db);
  static void init(const QString filename, QTreeWidget * putin);
  void init(const QString filename, DataBase * db, QTreeWidget * putin);
  static void this_is_playing(Song * main_now);
  static void save_history();
  static void clear_history(DataBase * db);
};
#endif // __loaded__history_h__
