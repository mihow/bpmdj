/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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
#include "kbpm-played.h"
#include "common.h"
#include "index.h"
#include "mixinformation.h"

int Played::songs_played = 0;
int Played::size = 1;
int Played::next = 1;
Song * Played::t_2 = NULL;
Song * Played::t_1 = NULL;
Song * Played::t_0 = NULL;
FILE *Played::f = NULL;
QString** Played::names = NULL;

void Played::Add(QString *filename)
{
  /* put line in memory */
  names[next++]=filename;
  if (next>=size)
    {
      size*=2;
      names=reallocate(names,size,QString*);
    }
}

Played::Played(const QString filename)
{
  /* initialise */
  size=100;
  next=0;
  names=allocate(size,QString*);
  /* read the file or create it */
  f=fopen(filename,"rb");
  /* read all lines in memory and sort them */
  if (f!=NULL)
    {
      char  *line = NULL;
      size_t blen = 0;
      ssize_t len = 0;
      while((len=getline(&line,&blen,f))!=-1)
	{
	  line[len-1]=0;
	  Add(new QString(line));
	}
      if (line)
	free(line);
    }
  /* open file for appending */
  f=fopen(filename,"ab");
}

bool Played::IsPlayed(const QString indexname)
{
  for(int i = 0 ; i < next ; i++)
    if (*(names[i])==indexname) 
      return true;
  return false;
}

void Played::Play(Song * main_now)
{
  if (main_now)
    {
      // write to disk..
      const QString indexname = main_now->index;
      fprintf(f,"%s\n",(const char*)indexname);
      fflush(f);
      // increase counter
      songs_played++;
      main_now->played = true;
    }
  // check the history and update records as necessary...
  // t_1 its prev should point to t_2
  // t_1 its next should point to t_0
  t_2 = t_1;
  t_1 = t_0;
  t_0 = main_now;
  if (t_1)
    {
      Index * i = new Index(t_1->index);
      QString info;
      HistoryField * f;
      if (t_0) 
	{
	  f = i->add_next_song(t_0->file);
	  info = f -> comment;
	  if (Config::ask_mix)
	    {
	      MixInformation mixinfo(NULL,NULL,TRUE);
	      mixinfo.fromLabel->setText(t_1->title+"["+t_1->author+"]"+t_1->version);
	      mixinfo.toLabel->setText(t_0->title+"["+t_0->author+"]"+t_0->version);
	      mixinfo.commentEdit->setText(info);
	      if (mixinfo.exec()==QDialog::Accepted)
		f-> comment = strdup(mixinfo.commentEdit->text());
	    }
	}
      if (t_2) 
	{
	  f = i->add_prev_song(t_2->file);
	  // if (info.isNull()) f -> comment = strdup("");
	  // else f -> comment = strdup(info);
	}
      i->write_idx();
      delete i;
    }
}
