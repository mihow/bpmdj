/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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


#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
#include <qlistview.h>
#include "song.h"
#include "database.h"
#include "data.h"
/**
 * A class to represent the song already played
 * This played class is reworked in comparison to v2.7
 * From v2.8 on we read in the played data after all songs have been
 * read into memory. This allows us to mark all songs much faster
 * and reduces the time to read all song when the playlist is larger.
 * It also ensures that we can fill in the right information in the 
 * logfile view list.
 */
class History
{
 private:
  static Song * t_2;  // T - 2
  static Song * t_1;  // T - 1
  static Song * t_0;  // T: the current main
  static FILE* f;
  static QString history_filename;
  singleton_accessors(int,songs_played);
  static QListView * log_ui;
  static void mark_as_played(DataBase * db, QString s);
  static void mark_as_played(Song *song);
 public:
  History(const QString filename, DataBase * db, QListView * putin);
  static void this_is_playing(Song * main_now);
  static void save_history();
  static void clear_history(DataBase * db);
};
