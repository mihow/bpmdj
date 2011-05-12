/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum.h"
#include "tags.h"
#include "memory.h"

init_singleton_var(QSong,selected,bool*,NULL);
init_singleton_var(QSong,compare_col,int,LIST_TEMPO);
init_singleton_var(QSong,compare_asc,bool,true);
init_singleton_var(QSong,songs,Song**,NULL);
init_singleton_var(QSong,song_count,int,0);

void QSong::setVector(Song**arr, int cnt)
{
  if (get_songs()) deallocate(get_songs());
  if (get_selected()) deallocate(get_selected());
  set_songs(arr);
  set_song_count(cnt);
  set_selected(allocate(get_song_count(),bool));
  for(int i = 0 ; i < cnt ; i ++)
    set_selected(i,false);
  assert(cnt>=0 &&
	 (cnt>0 ? arr!=NULL : arr==NULL));
  Sort();
}

int QSong::vectorSize() const
{
  return get_song_count();
}

QSong::QSong() : QVectorViewData()
{
}

static QColor * mixColor(QColor a, double d, QColor b)
{
  double ar = a.red();
  double br = b.red();
  double ag = a.green();
  double bg = b.green();
  double ab = a.blue();
  double bb = b.blue();
  return new QColor((int)(ar+d*(br-ar)),
		    (int)(ag+d*(bg-ag)),
		    (int)(ab+d*(bb-ab)));
}

QColor * QSong::colorOfTempoCol(Song* main, Song* song)
{
  if (!Config::color_range) return NULL;
  if (!main) return NULL;
  if (song->get_tempo().none()) return NULL;
  double d = song->tempo_distance(main);
  if ( d <= 1 )
    return mixColor(Config::color_tempo44,d,Qt::white);

  if (Config::show_tempo54)
    {
      d = minimum(song->tempo_distance(5.0/4.0,main), song->tempo_distance(4.0/5.0,main));
      if ( d <= 1 )
	return mixColor(Config::color_tempo54,d,Qt::white);
    }
  
  if (Config::show_tempo64)
    {
      d = minimum(song->tempo_distance(6.0/4.0,main), song->tempo_distance(4.0/6.0,main));
      if ( d <= 1 )
	return mixColor(Config::color_tempo64,d,Qt::white);
    }
  
  if (Config::show_tempo74)
    {
      d = minimum(song->tempo_distance(7.0/4.0,main), song->tempo_distance(4.0/7.0,main));
      if ( d <= 1 )
	return mixColor(Config::color_tempo74,d,Qt::white);
    }
  
  if (Config::show_tempo84)
    {
      d = minimum(song->tempo_distance(8.0/4.0,main), song->tempo_distance(4.0/8.0,main));
      if ( d <= 1 )
	return mixColor(Config::color_tempo84,d,Qt::white);
    }
  
  return NULL;
}

QColor * QSong::colorOfAuthorCol(Song* song)
{
  if (!Config::color_authorplayed) return NULL;
  int played_author_songs_ago = Played::songs_played - song->get_played_author_at_time();
  if (played_author_songs_ago < Config::authorDecay)
    return mixColor(Config::color_played_author, (float)played_author_songs_ago/(float)Config::authorDecay ,Qt::white);
  return NULL;
}

QColor * QSong::colorOfdColorCol(Song* song)
{
  if (!Config::color_dcolor) return NULL;
  if (song->get_color_distance() > 1.0) return NULL;
  return mixColor(Config::color_dcolor_col, song -> get_color_distance(), Qt::white);
}

void QSong::paintCell(QVectorView* vv, int i, QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  Song * main = ProcessManager::playingInMain();
  Song * song = get_songs(i);
  QColor *color;
  switch(col)
    {
    case LIST_CUES:
      if (Config::color_cues && !song->get_has_cues())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_TEMPO:
      if (color=colorOfTempoCol(main,song))
	{
	  QColorGroup ncg(cg);
          ncg.setColor(QColorGroup::Base,*color);
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case LIST_TITLE:
      if (Config::color_played && song->get_played())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,Config::color_played_song);
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_AUTHOR:
      if (color=colorOfAuthorCol(song))
	{
	  QColorGroup ncg(cg);
          ncg.setColor(QColorGroup::Base,*color);
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case LIST_DCOLOR:
      if (color=colorOfdColorCol(song))
	{
	  QColorGroup ncg(cg);
          ncg.setColor(QColorGroup::Base,*color);
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case LIST_SPECTRUM:
      if (Config::color_spectrum)
	if (song->get_spectrum()!=no_spectrum)
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,song->get_color());
	    QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	    return;
	  }
      break;
    }
  
  // the normal color depends wether the song is on the disk or not
  if (Config::color_ondisk && !song->get_ondisk())
    {
      QColorGroup ncg(cg);
      ncg.setColor(QColorGroup::Base,Config::color_unavailable);
      QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
    }  
  else
    QVectorViewData::paintCell(vv,i,p,cg,col,wid,align);
}

QString QSong::Text(Song * j, int i)
{
  switch (i)
    {
    case LIST_TEMPO : return j->tempo_str();
    case LIST_VERSION : return j->get_version();
    case LIST_TITLE : return j->get_title();
    case LIST_AUTHOR : return j->get_author();
    case LIST_INDEX : return j->get_storedin();
    case LIST_TAGS : return Tags::full_string(j->get_tags());
    case LIST_TIME : return j->get_time();
    case LIST_MD5SUM : return j->get_md5sum();
    case LIST_DCOLOR : return j->get_distance_string();
    case LIST_SPECTRUM : return j->get_spectrum_string();
    case LIST_FILE : return j->get_file();
    case LIST_ONDISK :
      if (j->get_ondisk()) return TRUE_TEXT;
      else                 return FALSE_TEXT;
    case LIST_CUES :
      if (j->get_has_cues()) return TRUE_TEXT;
      else                   return FALSE_TEXT;
    case LIST_MIN: 
      if (!j->get_min_amp().fully_defined()) return EMPTY;
      return QString::number(j->get_min_amp().left)+" / "+QString::number(j->get_min_amp().right);
    case LIST_MAX: 
      if (!j->get_max_amp().fully_defined()) return EMPTY;
      return QString::number(j->get_max_amp().left)+" / "+QString::number(j->get_max_amp().right);
    case LIST_MEAN:
      if (!j->get_mean_amp().fully_defined()) return EMPTY;
      return QString::number(j->get_mean_amp().left)+" / "+QString::number(j->get_mean_amp().right);
    case LIST_POWER: 
      if (!j->get_power().fully_defined()) return EMPTY;
      return QString::number(j->get_power().left)+" / "+QString::number(j->get_power().right);
    }
}

static int compare_songs_text(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  QString AT = QSong::Text(A,QSong::get_compare_col());
  QString BT = QSong::Text(B,QSong::get_compare_col());
  return clip(QString::compare(AT,BT));
}

static int compare_songs_tempo(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  return compare_tempo(A->get_tempo(),B->get_tempo());
}

static int compare_songs_power(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  float8 pa = A->get_power().left + A->get_power().right;
  float8 pb = B->get_power().left + B->get_power().right;
  if (pa > pb)
    return 1;
  else if (pa < pb)
    return -1;
  return 0;
}

void QSong::sort(int col, bool ascending)
{
  set_compare_col(col);
  set_compare_asc(ascending);
  Sort();
}

void QSong::Sort()
{
  if (get_compare_col()<0 || !get_songs() || get_song_count()<=0)
    return;
  if (get_compare_col()==LIST_TEMPO)
    {
      printf("Sorting according to tempo column\n");
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_tempo);
    }
  else if (get_compare_col()==LIST_POWER)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_power);
    }
  else 
    {
      // printf("Sorting according to column %d\n",compare_col);
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_text);
    }
  if (!get_compare_asc())
    {
      for(int i = 0 ; i <get_song_count() / 2 ; i++)
	{
	  Song * tmp = get_songs(i);
	  set_songs(i,get_songs(get_song_count()-1-i));
	  set_songs(get_song_count()-1-i,tmp);
	}
    }
  // printf("Done sorting\n");
}

Song * QSong::songEssence(int i) 
{
  if (i<0 || i >=get_song_count()) return NULL; 
  else return get_songs(i);
};

