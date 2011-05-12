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
#include <qinputdialog.h>
#include "history.h"
#include "common.h"
#include "growing-array.cpp"

int Played::songs_played = 0;
Song * Played::t_2 = NULL;
Song * Played::t_1 = NULL;
Song * Played::t_0 = NULL;
FILE *Played::f = NULL;
GrowingArray<QString> Played::names;

Played::Played(const QString filename)
{
  names.init();
  f=fopen(filename,"rb");
  if (f!=NULL)
    {
      char  *line = NULL;
      size_t blen = 0;
      ssize_t len = 0;
      while((len=getline(&line,&blen,f))!=-1)
	{
	  line[len-1]=0;
	  names.add(QString(line));
	}
      if (line)
	deallocate(line);
    }
  f=fopen(filename,"ab");
}

bool Played::IsPlayed(Song * which)
{
  for(int i = 0 ; i < names.count ; i++)
    if (names.elements[i]==which->get_file()) 
      return true;
  return false;
}

void Played::Play(Song * main_now)
{
  if (main_now)
    {
      // write to disk..
      const QString name = main_now->get_file();
      fprintf(f,"%s\n",(const char*)name);
      fflush(f);
      // increase counter
      songs_played++;
      main_now->set_played(true);
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
	  if (Config::get_ask_mix())
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
