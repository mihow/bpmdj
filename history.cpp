/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <assert.h>
#include <stdio.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include "history.h"
#include "common.h"
#include "scripts.h"

Song * History::t_2 = NULL;
Song * History::t_1 = NULL;
Song * History::t_0 = NULL;
init_singleton_var(History,songs_played,int,0);
QListView * History::log_ui = NULL;
FILE *History::f = NULL;
QString History::history_filename;

History::History(const QString filename, DataBase *db, QListView * putin)
{
  log_ui = putin;
  if (f) fclose(f);
  f = fopen(filename,"rb");
  if (f!=NULL)
    {
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
    }
  history_filename = filename;
  f=fopen(filename,"ab");
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
    new QListViewItem(log_ui,
		      tonumber(get_songs_played()),
		      song->get_title(),
		      song->get_author());
}

void History::save_history()
{
  // ask for filename
  QString s = QFileDialog::getSaveFileName("","History (*.log)",NULL,"Save Play History","Choose a filename" );
  if (s.isNull()) return;
  start_cp(history_filename,s);
}

void History::clear_history(DataBase * db)
{
  // check user stupidity
  if (QMessageBox::question(NULL,"Clear History", 
			    "Are you sure that you want\n"
			    "to clear the history ?", 
			    QMessageBox::Yes, 
			    QMessageBox::No|QMessageBox::Default)
      !=QMessageBox::Yes) return;
  // clear history view
  log_ui->clear();
  // clear file
  fclose(f);
  f=fopen(history_filename,"wb");
  // clear memory marks
  GrowingArray<Song*> *songs = db->getAllSongs();
  for(int i = 0 ; i < songs->count ; i++)
    {
      Song * song = songs->elements[i];
      if (song)
	{
	  song->init_played_author_at_time();
	  song->init_played();
 	}
    }
}

void History::this_is_playing(Song * main_now)
{
  if (main_now)
    {
      // write to disk..
      const QString name = main_now->get_file();
      fprintf(f,"%s\n",(const char*)name);
      fflush(f);
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
      QString info;
      HistoryField * f;
      if (t_0) 
	{
	  f = i.add_next_song(t_0->get_file());
	  info = f -> comment;
	  if (Config::ask_mix)
	    {
	      bool ok;
	      QString mixinfo = QInputDialog::getText("How did the mix go ?",
						      "From : " + t_1->get_title() + "[" + t_1->get_author()+"]" + t_1->get_version() + "\n"+
						      "To: " + t_0->get_title() + "[" + t_0->get_author() + "]" + t_0->get_version() + "\n",QLineEdit::Normal,
						      info,&ok);
	      if (ok) f-> comment = strdup(mixinfo);
	    }
	}
      if (t_2) 
	{
	  f = i.add_prev_song(t_2->get_file());
	  // if (info.isNull()) f -> comment = strdup("");
	  // else f -> comment = strdup(info);
	}
      i.write_idx();
    }
}