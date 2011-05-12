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
#include "songselector.logic.h"
#include "qsongviewitem.h"

QSongViewItem::QSongViewItem(QListViewItem* parent, Song* content) : QListViewItem(parent,"","","","","","","")
{
   assert(content);
   song=content;
}

QSongViewItem::QSongViewItem(QListView* parent, Song* content) : QListViewItem(parent,"","","","","","","")
{
   assert(content);
   song=content;
}

void QSongViewItem::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
   /* if the song is already played we color it red
    * if the song is in range, we color it green
    * if the song is not on the disk, we add some blue 
    */
   int nogreen_value=255;
   if ((!SongSelectorLogic::color_range) && (!SongSelectorLogic::color_played))
     {
	if (SongSelectorLogic::color_notondisk && !song->song_ondisk)
	  {
	     QColorGroup ncg(cg);
	     ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	     QListViewItem::paintCell(p,ncg,col,wid,align);
	  }
	else
	  {
	     QListViewItem::paintCell(p,cg,col,wid,align);
	  }
	return;
     }
   if (SongSelectorLogic::color_played && song->song_played)
     {
	QColorGroup ncg(cg);
	ncg.setColor(QColorGroup::Base,QColor(255,0,0));
	QListViewItem::paintCell(p,ncg,col,wid,align);
	return;
     }
   if (SongSelectorLogic::color_range && (SongSelectorLogic::mainTempo!=0) && song->song_tempo)
     {
	double t=atof(song->song_tempo);
	if (t>SongSelectorLogic::mainTempo*0.94 && t<SongSelectorLogic::mainTempo*1.06)
	  nogreen_value=(int)(fabs(t/SongSelectorLogic::mainTempo-1.0)*255.0/0.06);
     }
   QColorGroup ncg(cg);
   if (SongSelectorLogic::color_notondisk && !song->song_ondisk)
     {
	ncg.setColor(QColorGroup::Base,QColor(255-nogreen_value,255-nogreen_value,255));
     }
   else
     {
	ncg.setColor(QColorGroup::Base,QColor(nogreen_value,255,nogreen_value));
     }
   QListViewItem::paintCell(p,ncg,col,wid,align);
}

QString QSongViewItem::text(int i) const
{
   if (i==LIST_DIR) 
     {
	if (song->isIndex())
	  return ((SongIndex*)song)->song_id;
	else 
	  return "";
     }
   if (i==LIST_VERSION) return song->song_version;
   if (i==LIST_TITLE) return song->song_title;
   if (i==LIST_AUTHOR) return song->song_author;
   if (i==LIST_TEMPO) return song->song_tempo;
   if (i==LIST_INDEX) return song->song_index;
   if (i==LIST_TAGS) return song->song_tags;
   if (i==LIST_FILE) return song->song_file;
   if (i==LIST_ONDISK) 
     {
	if (song->song_ondisk) return "true";
	else return "false";
     }
   if (i==LIST_MD5SUM) return song->song_md5sum;
   return "";
}
