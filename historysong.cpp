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
#include <stdlib.h>
#include <qlistview.h>
#include "qstring-factory.h"
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "kbpm-played.h"
#include "dirscanner.h"
#include "spectrum.h"
#include "historysong.h"
#include "index.h"

#define HISTORY_MAINRELATION 0
#define HISTORY_COMMENT 1
#define HISTORY_TEMPO 2
#define HISTORY_SPECTRUM 3
#define HISTORY_DCOLOR 4
#define HISTORY_TIME 5
#define HISTORY_CUES 6
#define HISTORY_TITLE 7
#define HISTORY_AUTHOR 8
#define HISTORY_VERSION 9
#define HISTORY_TAGS 10
#define HISTORY_ONDISK 11

/**
 * this is a mess... (TM)
 */
void HistorySong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  switch(col)
    {
    case HISTORY_CUES:
      if (Config::color_cues && !song->has_cues)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case HISTORY_TEMPO: 
      {
	Song * main = ProcessManager::playingInMain();
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
      }
      break;
      
    case HISTORY_TITLE:
      if (Config::color_played && song->played)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(255,0,0));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case HISTORY_AUTHOR:
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
      
    case HISTORY_DCOLOR:
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
      
    case HISTORY_SPECTRUM:
      if (Config::color_spectrum)
	if (!song->spectrum.isNull())
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,song->color);
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      break;
      
    case HISTORY_ONDISK:
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

QString HistorySong::text(int i) const
{
  switch (i)
    {
    case HISTORY_MAINRELATION : return relation;
    case HISTORY_COMMENT : return comment;
    case HISTORY_VERSION : return song->version;
    case HISTORY_TITLE : return song->title;
    case HISTORY_AUTHOR : return song->author;
    case HISTORY_TEMPO : return song->tempo;
    case HISTORY_TAGS : return song->tags;
    case HISTORY_TIME : return song->time;
    case HISTORY_DCOLOR : return song->distance_string;
    case HISTORY_SPECTRUM : return song->spectrum_string;
    case HISTORY_ONDISK :
      if (song->ondisk) 
	return TRUE_TEXT;
      else 
	return FALSE_TEXT;
    case HISTORY_CUES :
      if (song->has_cues) 
	return TRUE_TEXT;
      else 
	return FALSE_TEXT;
    }
  return QString::null;
}

HistorySong::HistorySong(Song * s, QString r, QString c, QListView* parent) :
  QListViewItem(parent,"","","","","","","")
{
  song =  s;
  relation = r;
  comment = c;
}

QString HistorySong::TRUE_TEXT("Yes");
QString HistorySong::FALSE_TEXT("No");
