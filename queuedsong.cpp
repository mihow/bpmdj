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

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "queuedsong.h"
#include "tags.h"

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


/**
 * this is a mess... (TM)
 */
void QueuedSong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  if (col!=QUEUED_ANKER && !song)
    {
      QListViewItem::paintCell(p,cg,col,wid,align);
      return;
    }
  switch(col)
    {
    case QUEUED_ANKER:
      if (anker)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(255,0,0));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      else
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(255,255,0));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
    case QUEUED_CUES:
      if (Config::get_color_cues() && !song->get_has_cues())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case QUEUED_TEMPO: 
      break;
      
    case QUEUED_TITLE:
      if (Config::get_color_played() && song->get_played())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,Config::get_color_played_song());
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case QUEUED_AUTHOR:
      { 
	QColor * color;
	if ( (color=QSong::colorOfAuthorCol(song)) )
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,*color);
	    QListViewItem::paintCell(p,ncg,col,wid,align);
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
	ncg.setColor(QColorGroup::Base,QColor(yellow,255-yellow,0));
	QListViewItem::paintCell(p,ncg,col,wid,align);
	return;
      }
      
    case QUEUED_SPECTRUM:
      if (Config::get_color_spectrum())
	if (song->get_spectrum()!=no_spectrum)
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,song->get_color());
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      break;
    }
  if (Config::get_color_ondisk() && !song->get_ondisk())
    {
      QColorGroup ncg(cg);
      ncg.setColor(QColorGroup::Base,Config::get_color_unavailable());
      QListViewItem::paintCell(p,ncg,col,wid,align);
    }
  else
    QListViewItem::paintCell(p,cg,col,wid,align);
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
  return QListViewItem::text(i);
}

QueuedSong::QueuedSong(Song * s, QListView* parent) :
  QListViewItem(parent,"","","","","","","")
{
  song = s;
  setText(QUEUED_TEMPO,song->tempo_str());
  anker = true;
  distance = 0; 
  mark = false;
  pos = parent->childCount();
}

QueuedSong::QueuedSong(QListView* parent, QListViewItem *after) :
  QListViewItem(parent,after)
{
  song = NULL;
  anker = false;
  distance = -1; 
  mark = false;
  pos = parent->childCount();
}

void QueuedSong::setSong(Song* s, double d)
{
  song=s; 
  distance=d;   
  if (song)
    setText(QUEUED_TEMPO, song->tempo_str()); 
}

