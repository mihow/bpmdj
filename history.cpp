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
#ifndef __loaded__history_cpp__
#define __loaded__history_cpp__
using namespace std;
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <qlabel.h>
#include <qbytearray.h>
#include <qlineedit.h>
#include <assert.h>
#include <stdio.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include "history.h"
#include "common.h"
#include "scripts.h"
#include "vector-iterator.h"
#include "qstring-factory.h"
#include "selector.h"

Song * History::t_2 = NULL;
Song * History::t_1 = NULL;
Song * History::t_0 = NULL;
init_singleton_var(History,songs_played,unsigned4,0);
QTreeWidget * History::log_ui = NULL;
FILE *History::file = NULL;
QByteArray History::filename;

void History::mark_all_played_songs(DataBase *db)
{
  FILE * f = fopen(filename,"rb");
  if (!f) return;
  char  *line = NULL;
  size_t blen = 0;
  ssize_t len = 0;
  while((len=getline(&line,&blen,f))!=-1)
    {
      line[len-1]=0;
      QString s = QStringFactory::create(line);
      mark_as_played(db, s);
    }
  if (line) 
    bpmdj_deallocate(line);
  /** 
   * We place this nicely at the end. Otherwise we ahve an O(n*n) 
   * insertion operation. Shitty Qt4
   */
  selector->playedList->header()->
    setResizeMode(QHeaderView::ResizeToContents);
}

void History::init(const QString fname, QTreeWidget * putin)
{
  filename=fname.toAscii();
  log_ui = putin;
  file=fopen(filename,"ab");
}

void History::mark_as_played(DataBase * db, QString s)
{
  Song * song = db->find(s);
  if (!song) return;
  mark_as_played(song);
}

void History::mark_as_played(Song *song)
{
  song->set_played(true);
  set_songs_played(get_songs_played() + 1);
  if (log_ui)
    {
      QStringList a;
      a << tonumber(get_songs_played()) 
	<< song->get_title()
	<< song->get_author();
      new QTreeWidgetItem(log_ui,a);
    }
}

void History::save_history()
{
  QString s = QFileDialog::getSaveFileName(NULL,"Save history to ?", 
					   "history.log",
					   "History (*.log)");
  if (s.isNull()) return;
  start_cp(filename,s);
}

void History::clear_history(DataBase * db)
{
  // Ensure the user is notified that he does something stupid
  if (QMessageBox::question(NULL,"Clear History", 
			    "Are you sure that you want\n"
			    "to clear the history ?", 
			    QMessageBox::Yes, 
			    QMessageBox::No|QMessageBox::Default)
      !=QMessageBox::Yes) return;
  // clear history view
  log_ui->clear();
  // clear file
  fclose(file);
  file=fopen(filename,"wb");
  // clear memory marks
  const vector<Song*> &songs = db->getAllSongs();
  constVectorIterator<Song*> song(songs);
ITERATE_OVER(song)

    if (song.val())
    {
      song.val()->init_played_author_at_time();
      song.val()->init_played();
    }
  }
}

void History::this_is_playing(Song * main_now)
{
  if (main_now && file)
    {
      QByteArray name = main_now->get_file().toAscii();
      fprintf(file,"%s\n",(const char*)name);
      fflush(file);
      mark_as_played(main_now);
    }
  // check the history and update records as necessary...
  // t_1 its prev should point to t_2
  // t_1 its next should point to t_0
  t_2 = t_1;
  t_1 = t_0;
  t_0 = main_now;
  if (t_1)
    {
      Index i(t_1->get_storedin());
      HistoryField * f;
      if (t_0) 
	{
	  f = i.add_next_song(t_0->get_file());
	  QString info = f -> comment;
	  if (Config::ask_mix)
	    {
	      bool ok;
	      QString from_text = t_1->get_title() + "[" +
		t_1->get_author()+"]" + t_1->get_version();
	      QString to_text = t_0->get_title() + "[" + 
		t_0->get_author() + "]" + t_0->get_version();
	      QString mixinfo = QInputDialog::getText(NULL,
				      "How did the mix go ?",
				      "From : " + from_text + "\n"+
				      "To: " + to_text + "\n",
				      QLineEdit::Normal,info,&ok);
	      if (ok) f->comment = mixinfo;
	    }
	}
      if (t_2) 
	f = i.add_prev_song(t_2->get_file());
      i.write_idx();
    }
}
#endif // __loaded__history_cpp__
