/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "historysong.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "selector.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "historysong.h"
#include "tags.h"

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

void HistorySong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  QColor * color;
  switch(col)
    {
    case HISTORY_CUES:
      if (Config::color_cues && !song->get_has_cues())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,QColor(0,0,255));
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case HISTORY_TEMPO: 
      {
	Song * main = ::main_song;
	if ( (color=QSong::colorOfTempoCol(main,song)) )
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Window,*color);
	    Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	    delete(color);
	    return;
	  }
	break;
      }

    case HISTORY_TITLE:
      if (Config::color_played && song->get_played())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,Config::get_color_played_song());
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case HISTORY_AUTHOR:
      if ( (color=QSong::colorOfAuthorCol(song)) )
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,*color);
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case HISTORY_DCOLOR:
      if ( (color=QSong::colorOfdColorCol(song)) )
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Window,*color);
	  Q3ListViewItem::paintCell(p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case HISTORY_SPECTRUM:
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

QString HistorySong::text(int i) const
{
  switch (i)
    {
    case HISTORY_MAINRELATION : return relation;
    case HISTORY_COMMENT : return comment;
    case HISTORY_VERSION : return song->get_version();
    case HISTORY_TITLE : return song->get_title();
    case HISTORY_AUTHOR : return song->get_author();
    case HISTORY_TEMPO : return song->tempo_str();
    case HISTORY_TAGS : return Tags::full_string(song->get_tags());
    case HISTORY_TIME : return song->get_time();
    case HISTORY_DCOLOR : return song->get_distance_string();
    case HISTORY_SPECTRUM : return song->get_spectrum_string();
    case HISTORY_ONDISK :
      if (song->get_ondisk()) 	return TRUE_TEXT;
      else 	return FALSE_TEXT;
    case HISTORY_CUES :
      if (song->get_has_cues())  return TRUE_TEXT;
      else 	return FALSE_TEXT;
    }
  return QString::null;
}

HistorySong::HistorySong(Song * s, QString r, QString c, Q3ListView* parent) :
  Q3ListViewItem(parent,"","","","","","","")
{
  song =  s;
  relation = r;
  comment = c;
}
