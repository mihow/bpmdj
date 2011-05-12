/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__queuedsong_cpp__
#define __loaded__queuedsong_cpp__
using namespace std;
#line 1 "queuedsong.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "queuedsong.h"
#include "tags.h"
#include "config.h"
#include "scripts.h"

#define QUEUED_ANKER 0
#define QUEUED_DLINE 1
#define QUEUED_TEMPO 2
#define QUEUED_SPECTRUM 3
#define QUEUED_TIME 4
#define QUEUED_CUES 5
#define QUEUED_TITLE 6
#define QUEUED_AUTHOR 7
#define QUEUED_VERSION 8
#define QUEUED_TAGS 9
#define QUEUED_ONDISK 10
#define QUEUED_INDEX 11
#define QUEUED_MD5SUM 12
#define QUEUED_FILE 13

QString QueuedAnalSong::text(int i) const
{
  switch (i)
    {
    case ANAL_TITLE : return song->get_title();
    case ANAL_AUTHOR : return song->get_author();
    case ANAL_INDEX : return song->get_storedin();
    case ANAL_RMS : return needs_energy() ? ANAL_NEC : ANAL_NOTNEC;
    case ANAL_TEMPO : return needs_tempo() ? ANAL_NEC : ANAL_NOTNEC;
    case ANAL_SPECTRUM : return needs_spectrum() ? ANAL_NEC : ANAL_NOTNEC;
    case ANAL_ECHO : return needs_echo() ? ANAL_NEC : ANAL_NOTNEC;
    case ANAL_RYTHM : return needs_rythm() ? ANAL_NEC : ANAL_NOTNEC;
    case ANAL_COMPOSITION : return needs_composition() ? ANAL_NEC : ANAL_NOTNEC;
    }
  return Q3ListViewItem::text(i);
}

void QueuedSong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  if (col!=QUEUED_ANKER && !song)
    {
      Q3ListViewItem::paintCell(p,cg,col,wid,align);
      return;
    }
  switch(col)
    {
    case QUEUED_ANKER:
      if (anker)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,QColor(255,0,0));
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      else
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,QColor(255,255,0));
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
    case QUEUED_CUES:
      if (Config::color_cues && !song->get_has_cues())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,QColor(0,0,255));
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case QUEUED_TEMPO: 
      break;
      
    case QUEUED_TITLE:
      if (Config::color_played && song->get_played())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,Config::get_color_played_song());
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case QUEUED_AUTHOR:
      { 
	QColor * color;
	if ( (color=QSong::colorOfAuthorCol(song)) )
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Window,*color);
	    Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	    delete(color);
	    return;
	  }
	break;
      }
      
    case QUEUED_DLINE:
      {
	int yellow = 255;
	QColorGroup ncg(cg);
	if (distance<=1)
	  yellow = (int)(distance*255.0);
	ncg.setColor(QColorGroup::Window,QColor(yellow,255-yellow,0));
	Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	return;
      }
      
    case QUEUED_SPECTRUM:
      if (Config::color_spectrum)
	if (song->get_spectrum()!=no_spectrum)
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Window,song->get_color());
	    Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      break;
    }
  if (Config::color_ondisk && !song->get_ondisk())
    {
      QColorGroup ncg(cg);
      ncg.setColor(QColorGroup::Window,Config::get_color_unavailable());
      Q3ListViewItem::paintCell(p,ncg,col,wid,align);
    }
  else
    Q3ListViewItem::paintCell(p,cg,col,wid,align);
}

QString QueuedSong::text(int i) const
{
  if (!song && i != QUEUED_ANKER) 
    return QString::null;
  switch (i)
    {
    case QUEUED_VERSION : return song->get_version();
    case QUEUED_TITLE : return song->get_title();
    case QUEUED_AUTHOR : return song->get_author();
    case QUEUED_INDEX : return song->get_storedin();
    case QUEUED_TAGS : return Tags::full_string(song->get_tags());
    case QUEUED_TIME : return song->get_time();
    case QUEUED_MD5SUM : return song->get_md5sum();
    case QUEUED_DLINE : return QString::number(distance,'f',3);
    case QUEUED_SPECTRUM : return song->get_spectrum_string();
    case QUEUED_FILE : return song->get_file();
    case QUEUED_ONDISK :
      if (song->get_ondisk()) return TRUE_TEXT;
      else                    return FALSE_TEXT;
    case QUEUED_CUES :
      if (song->get_has_cues()) return TRUE_TEXT;
      else 	return FALSE_TEXT;
    case QUEUED_ANKER : 
      return tonumber(pos);
    }
  return Q3ListViewItem::text(i);
}

QueuedSong::QueuedSong(Song * s, Q3ListView* parent) :
  Q3ListViewItem(parent,"","","","","","","")
{
  song = s;
  setText(QUEUED_TEMPO,song->tempo_str());
  anker = true;
  distance = 0; 
  mark = false;
  pos = parent->childCount();
  // setAutoFillBackground(true);
}

QueuedSong::QueuedSong(Q3ListView* parent, Q3ListViewItem *after) :
  Q3ListViewItem(parent,after)
{
  song = NULL;
  anker = false;
  distance = -1; 
  mark = false;
  pos = parent->childCount();
}

QueuedAnalSong::QueuedAnalSong(Q3ListView* parent, Song * s) :
  Q3ListViewItem(parent), song(s) 
{
};

void QueuedSong::setSong(Song* s, float8 d)
{
  song=s;
  distance=d;
  if (song)
    setText(QUEUED_TEMPO, song->tempo_str());
}

QString QueuedAnalSong::getCommand(SongProcess &analyzer)
{
  return analyzer.getAnalCommand(needs_tempo(),
				 Config::get_anal_bpm_technique(),
				 Config::get_anal_bpm_from(), Config::get_anal_bpm_to(),
				 needs_spectrum() || needs_echo(),
				 needs_energy(),
				 needs_rythm() || needs_composition(),
				 song->get_storedin());
}
#endif // __loaded__queuedsong_cpp__
