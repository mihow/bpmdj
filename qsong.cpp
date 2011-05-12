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

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <qpainter.h>
#include <stdlib.h>
#include "qstring-factory.h"
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "kbpm-played.h"
#include "dirscanner.h"
#include "spectrum.h"

//#define LIST_PATTERN 2
#define LIST_DCOLOR 2
#define LIST_TITLE 5
#define LIST_AUTHOR 6
#define LIST_TEMPO 0
#define LIST_TIME 3
#define LIST_CUES 4
#define LIST_VERSION 7
#define LIST_TAGS 8
#define LIST_ONDISK 9
#define LIST_SPECTRUM 1
#define LIST_INDEX 10
#define LIST_MD5SUM 11
#define LIST_FILE 12

QSong::QSong(Song * s, QListView* parent) :
  QListViewItem(parent)
{
  song=s;
}

/**
 * this is a mess... (TM)
 */
void QSong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  Song * main = ProcessManager::playingInMain();
  switch(col)
    {
    case LIST_CUES:
      if (Config::color_cues && !song->has_cues)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_TEMPO:
      if (Config::color_range && main && song->tempo)
	{
	  int nogreen_value = 255;
	  double d = song->tempo_distance(main);
	  if (d<1) nogreen_value=(int)(d*255.0);
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(nogreen_value,255,nogreen_value));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_TITLE:
      if (Config::color_played && song->played)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(255,0,0));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_AUTHOR:
      {
	if (Config::color_authorplayed)
	  {
	    int played_author_songs_ago = Played::songs_played - song->played_author_at_time;
	    if (played_author_songs_ago < Config::authorDecay)
	      {
		QColorGroup ncg(cg);
		ncg.setColor(QColorGroup::Base,
			     QColor(255,150+(255-150)*played_author_songs_ago/Config::authorDecay
				    ,255*played_author_songs_ago/Config::authorDecay));
		QListViewItem::paintCell(p,ncg,col,wid,align);
		return;
	      }
	  }
	break;
      }
      
    case LIST_DCOLOR:
      {
	if (Config::color_dcolor)
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,QColor(255,255,song->color_distance));
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
	break;
      }
      
    case LIST_SPECTRUM:
      if (Config::color_spectrum)
	if (!song->spectrum.isNull())
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,song->color);
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      break;
      
      /*    case LIST_PATTERN:
      {
	const QRect size = p->viewport();
	const QPixmap *pattern = getPixmap(listView()->columnWidth(LIST_PATTERN),height(),cg);
	if (pattern)
	  {
	    p->drawPixmap(0,0,*pattern);
	    delete(pattern);
	    return;
	  }
	break;
      }
      */
      
    case LIST_ONDISK:
      if (Config::color_ondisk && !song->ondisk)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
    }
  
  QListViewItem::paintCell(p,cg,col,wid,align);
}


QString QSong::TRUE_TEXT("Yes");
QString QSong::FALSE_TEXT("No");

QString QSong::text(int i) const
{
  switch (i)
    {
    case LIST_VERSION : return song->version;
    case LIST_TITLE : return song->title;
    case LIST_AUTHOR : return song->author;
    case LIST_TEMPO : return song->tempo;
    case LIST_INDEX : return song->storedin;
    case LIST_TAGS : return song->tags;
    case LIST_TIME : return song->time;
    case LIST_MD5SUM : return song->md5sum;
    case LIST_DCOLOR : return song->distance_string;
    case LIST_SPECTRUM : return song->spectrum_string;
    case LIST_FILE : return song->file;
    case LIST_ONDISK :
      if (song->ondisk) return TRUE_TEXT;
      else              return FALSE_TEXT;
    case LIST_CUES :
      if (song->has_cues) return TRUE_TEXT;
      else                return FALSE_TEXT;
    }
  return QString::null;
}
