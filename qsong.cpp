/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__qsong_cpp__
#define __loaded__qsong_cpp__
using namespace std;
#line 1 "qsong.c++"
#include <qbitmap.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <qpainter.h>
#include <stdlib.h>
#include "song-metric.h"
#include "qsong.h"
#include "history.h"
#include "dirscanner.h"
#include "tags.h"
#include "memory.h"
#include "pixmap-cache.h"
#include "fragment-cache.h"
#include "players-manager.h"
#include "selector.h"
#include "qt-helpers.h"

init_singleton_var(QSong,compare_col,int,LIST_TEMPO);
init_singleton_var(QSong,compare_asc,bool,true);
init_singleton_var(QSong,songs,Song**,NULL);
init_singleton_var(QSong,song_count,int,0);
    
static PixmapCache echo_icon_cache; 
static PixmapCache rhythm_icon_cache;
static PixmapCache composition_icon_cache;

void QSong::setVector(Song**arr, int cnt)
{
  if (get_songs()) bpmdj_deallocate(get_songs());
  set_songs(arr);
  set_song_count(cnt);
  assert(cnt>=0 && (cnt>0 ? arr!=NULL : arr==NULL));
  Sort();
  reset();
}

/**
 * Will not sort the columns as to optimize the incremental reading.
 */
void QSong::addVector(Song**arr, int cnt)
{
  assert(cnt>=0);
  if (!cnt) return;
  int oldcnt=get_song_count();
  int newcnt=oldcnt+cnt;
  beginInsertRows(QModelIndex(),oldcnt,newcnt);
  set_songs(bpmdj_reallocate(get_songs(),newcnt,Song*));
  memcpy(get_songs()+oldcnt,arr,cnt*sizeof(Song*));
  set_song_count(newcnt);
  assert(cnt>=0 && (cnt>0 ? arr!=NULL : arr==NULL));
  endInsertRows();
}

QSong::QSong() : QAbstractTableModel()
{
}

QColor QSong::colorOfTempoCol(const Song* main, Song* song)
{
  if (!Config::color_range) throw normalColor(song);
  if (!main) return normalColor(song);
  if (song->get_tempo().none()) return normalColor(song);
  float4 d = SongMetriek::std.tempo_diff(*main,*song);
  int harmonic = 0;
  d = SongMetriek::std.find_harmonic(d,harmonic);
  d = fabs(d);
  if ( d > 1 ) return normalColor(song);
  if (harmonic == 0) return mixColor(Config::get_color_tempo44(),d,Qt::white);
  if (harmonic == 1) return mixColor(Config::get_color_tempo54(),d,Qt::white);
  if (harmonic == 2) return mixColor(Config::get_color_tempo64(),d,Qt::white);
  if (harmonic == 3) return mixColor(Config::get_color_tempo74(),d,Qt::white);
  if (harmonic == 4) return mixColor(Config::get_color_tempo84(),d,Qt::white);
  return normalColor(song);
}

QColor QSong::colorOfAuthorCol(Song* song)
{
  if (!Config::color_authorplayed) return normalColor(song);
  int played_author_songs_ago = History::get_songs_played() - 
    song->get_played_author_at_time();
  if (played_author_songs_ago < Config::get_authorDecay())
    return mixColor(Config::get_color_played_author(), 
		    (float4)played_author_songs_ago/
		    (float4)Config::get_authorDecay() ,Qt::white);
  return normalColor(song);
}

QColor QSong::colorOfPlaycount(Song* song)
{
  float4 count = song->get_alltime_playcount();
  float4 total = song->get_max_alltime_total();
  if (count)
    return mixColor(Qt::white, count/total, Config::get_color_alltime());
  return QColor(255,255,255);
}

QColor QSong::colorOfdColorCol(Song* song)
{
  if (!Config::color_dcolor) return normalColor(song);
  if (song->get_color_distance() > 1.0) return normalColor(song);
  return mixColor(Config::get_color_dcolor_col(), song -> get_color_distance(),
		  Qt::white);
}

static int compare_songs_text(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  QString AT = QSong::text(A,QSong::get_compare_col());
  QString BT = QSong::text(B,QSong::get_compare_col());
  return clip(QString::compare(AT,BT));
}

static int compare_songs_histogram(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  echo_property AT = A->get_histogram();
  echo_property BT = B->get_histogram();
  if (AT.empty())
    if (BT.empty()) return 0;
    else return 1;
  else 
    if (BT.empty()) return -1;
    else return 0;
}

static int compare_songs_rhythm(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  rhythm_property AT = A->get_rhythm();
  rhythm_property BT = B->get_rhythm();
  if (AT.empty())
    if (BT.empty()) return 0;
    else return 1;
  else 
    if (BT.empty()) return -1;
    else return 0;
}

static int compare_songs_composition(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  composition_property AT = A->get_composition();
  composition_property BT = B->get_composition();
  if (AT.empty())
    if (BT.empty()) return 0;
    else return 1;
  else 
    if (BT.empty()) return -1;
    else return 0;
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
  reset();
}

void QSong::Sort()
{
  if (get_compare_col()<0 || !get_songs() || get_song_count()<=0)
    return;
  if (get_compare_col()==LIST_TEMPO)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_tempo);
    }
  else if (get_compare_col()==LIST_POWER)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_power);
    }
  else if (get_compare_col()==LIST_HISTOGRAM)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_histogram);
    }
  else if (get_compare_col()==LIST_RHYTHM)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_rhythm);
    } 
  else if (get_compare_col()==LIST_COMPOSITION)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),
	    compare_songs_composition);
    }
  else  
    {
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
}

Song * QSong::songEssence(int i) 
{
  if (i<0 || i >=get_song_count()) return NULL; 
  else return get_songs(i);
};

Song * QSong::songEssence(const QModelIndex& idx)
{
  if (!idx.isValid()) return NULL;
  else return songEssence(idx.row());
};

QModelIndex QSong::indexof(Song* song)
{
  if (song)
    for(int i = 0 ; i < QSong::get_song_count() ; i ++)
      if (QSong::get_songs(i)==song)
	return selector->songList->createIndex(i,0,0);
  return QModelIndex();
}

Song * get_current_song()
{
  QModelIndex index = selector->songListView->currentIndex();
  if (index.isValid() && index.row()<QSong::get_song_count() && index.row()>=0)
    return QSong::songEssence(index.row());
  return NULL;
}

void set_current_song(Song* song, bool scrolltoit)
{
  QModelIndex index = QSong::indexof(song);
  selector->songListView->setCurrentIndex(index);
  if (scrolltoit)
    selector->songListView->scrollTo(index);
}

int QSong::rowCount(const QModelIndex&) const
{
  return get_song_count();
}

int QSong::columnCount(const QModelIndex&) const
{
  return LIST_COLCOUNT;
}

QColor QSong::normalColor(Song* song)
{
  if (Config::color_ondisk)
    {
      if (!song->get_ondisk_checked())
	return Config::get_color_unchecked();
      else if (!song->get_ondisk())
	return Config::get_color_unavailable();
    }
  if (Config::color_songs_based_on_history)
    return colorOfPlaycount(song);
  return QColor(255,255,255);
}

QString QSong::text(Song* song, int col)
{
  assert(song);
  switch(col)
    {
    case LIST_TEMPO: return song->tempo_str();
    case LIST_CUES:
      if (song->get_has_cues()) return TRUE_TEXT;
      return FALSE_TEXT;
    case LIST_TITLE:  return song->get_title();
    case LIST_AUTHOR: return song->get_author();
    case LIST_SPECTRUM: return song->get_spectrum_string();
    case LIST_DCOLOR: return song->get_distance_string();
    case LIST_VERSION : return song->get_version();
    case LIST_INDEX : return song->get_storedin();
    case LIST_TAGS: return Tags::full_string(song->get_tags());
    case LIST_TIME: return song->get_time();
    case LIST_MD5SUM: return song->get_md5sum();
    case LIST_FILE : return song->get_file();
    case LIST_ONDISK : 
      if (song->get_ondisk()) return TRUE_TEXT;
      return FALSE_TEXT;
    case LIST_MIN: 
      if (!song->get_min_amp().fully_defined()) return EMPTY;
      return QString::number(song->get_min_amp().left)+" / "+
	QString::number(song->get_min_amp().right);
    case LIST_MAX: 
      if (!song->get_max_amp().fully_defined()) return EMPTY;
      return QString::number(song->get_max_amp().left)+" / "+
	QString::number(song->get_max_amp().right);
    case LIST_MEAN:
      if (!song->get_mean_amp().fully_defined()) return EMPTY;
      return QString::number(song->get_mean_amp().left)+" / "+
	QString::number(song->get_mean_amp().right);
    case LIST_POWER: 
      if (!song->get_power().fully_defined()) return EMPTY;
      return QString::number(song->get_power().left)+" / "+
	QString::number(song->get_power().right);
    case LIST_HISTOGRAM: 
    case LIST_RHYTHM:
    case LIST_COMPOSITION:
      return EMPTY;
    } 
  return "WTF";
}

void QSong::aSongChangedBehindQSongsBack(Song* song)
{
  if (!song) return;
  int l=get_song_count();
  for(int i = 0 ; i < l ; i ++)
    if (QSong::get_songs(i)==song)
      {
	QModelIndex l=createIndex(i,0,0);
	QModelIndex r=createIndex(i,LIST_COLCOUNT,0);
	emit(dataChanged(l,r));
	return;
      }
}

/**
 * TODO: we might improve the performance here by directly passing a pointer
 * as internal identification into each generated QModelIndex
 */
QVariant QSong::data(const QModelIndex& pos, int role) const
{
  int col=pos.column();
  int row=pos.row();
  Song* song=get_songs(row);
  assert(song);
  /**
   * Text is something we can quickly deal with
   */
  if (role==Qt::DisplayRole) return text(song,col);
  
  /** 
   * Color of the background is slightly more complicated
   */
  if (role==Qt::BackgroundRole)
    switch(col)
      {
      case LIST_TEMPO: 
	{
	  Song * main = ::main_song;
	  return colorOfTempoCol(main,song);
	}
      case LIST_CUES:
	if (Config::color_cues && !song->get_has_cues())
	  return QColor(0,0,255);
	return normalColor(song);
      case LIST_TITLE:
	if (Config::color_played && song->get_played())
	  return Config::get_color_played_song();
	return normalColor(song);
      case LIST_AUTHOR:
	return colorOfAuthorCol(song);
      case LIST_SPECTRUM:
	if (Config::color_spectrum && song->get_spectrum()!=no_spectrum)
	  return song->get_color();
	return normalColor(song);
      case LIST_DCOLOR:
	return colorOfdColorCol(song);
      case LIST_VERSION: 
      case LIST_INDEX: 
      case LIST_TAGS: 
      case LIST_TIME: 
      case LIST_MD5SUM:
      case LIST_FILE: 
      case LIST_ONDISK:
      case LIST_MIN: 
      case LIST_MAX: 
      case LIST_MEAN:
      case LIST_POWER:
	return normalColor(song);
      case LIST_HISTOGRAM:
      case LIST_RHYTHM:
      case LIST_COMPOSITION:
	return normalColor(song);
      }

  /**
   * Images for echo, rhythm, composition
   */
  if (role==Qt::DecorationRole)
    {
      int h = selector->fontMetrics().height() - 1;
      Song * main = ::main_song;
      switch(col)
	{
	case LIST_HISTOGRAM:
	  if (!song->get_histogram().empty())
	    {
	      int wid=selector->
		songListView->
		horizontalHeader()->
		sectionSize(LIST_HISTOGRAM);
	      QPixmap * picture = echo_icon_cache.
		find(song,main,wid,h);
	      if (!picture->isNull()) return *picture;
	      QImage target(wid,h+1,QImage::Format_RGB32);
	      echo_property his = song->get_histogram();
	      if (main && !main->get_histogram().empty())
		{
		  echo_property mis = main->get_histogram();
		  for(unsigned2 y = 0 ; y <= h ; y ++)
		    {
		      int z = y*23/h;
		      smallhistogram_type<echo_prop_sx>& his_band = 
			his.get_band(z);
		      smallhistogram_type<echo_prop_sx>& mis_band =
			mis.get_band(z);
		      signed2 c1,c2;
		      QRgb* line=(QRgb*)target.scanLine(h-y);
		      for(unsigned2 v = 0 ; v < wid ; v ++)
			{
			  unsigned2 x = v*echo_prop_sx/wid;
			  c1 = his_band.get_probability(x);
			  c2 = mis_band.get_probability(x);
			  line[v]=qRgb(c1,(c1+c2)/2,c2);
			}
		    }
		}
	      else
		for(unsigned2 y = 0 ; y <= h ; y ++)
		  {
		    unsigned2 z = y*23/h;
		    smallhistogram_type<echo_prop_sx> his_band = 
		      his.get_band(z);
		    unsigned1 c;
		    QRgb* line=(QRgb*)target.scanLine(h-y);
		    for(unsigned2 v = 0 ; v < wid ; v ++)
		      {
			unsigned2 x = v*echo_prop_sx/wid;
			c = his_band.get_probability(x);
			line[v]=qRgb(c,c,c);
		      }
		  }
	      *picture=QPixmap::fromImage(target);
	      return *picture;
	    }
	  return QVariant();
	case LIST_RHYTHM:
	  if (!song->get_rhythm().empty())
	    {
	      int wid=selector->
		songListView->
		horizontalHeader()->
		sectionSize(LIST_RHYTHM);
	      QPixmap *picture = rhythm_icon_cache.find(song,main,wid,h);
	      if (picture->isNull())
		{
		  QColor co;
		  rhythm_property his = song->get_rhythm();
		  QImage target(wid,h+1,QImage::Format_RGB32);
		  if (main && !main->get_rhythm().empty())
		    {
		      rhythm_property mis = main->get_rhythm();
		      for(int y = 0 ; y <= h ; y ++)
			{
			  int z = y*23/h;
			  smallhistogram_type<rhythm_prop_sx> his_band = 
			    his.get_band(z);
			  smallhistogram_type<rhythm_prop_sx> mis_band = 
			    mis.get_band(z);			  
			  QRgb* line=(QRgb*)target.scanLine(h-y);
			  co.setHsv(z*10,255,255);
			  int r = co.red();
			  int g = co.green();
			  int b = co.blue();
			  for(int v = 0 ; v < wid ; v ++)
			    {
			      int x = v*rhythm_prop_sx/wid;
			      signed2 c1 = his_band.get_energy(x);
			      signed2 c2 = mis_band.get_energy(x);
			      if (c1<86) c1=86;
			      if (c2<86) c2=86;
			      signed2 c = abs(c1-c2) * 3 / 2;
			      line[v]=qRgb(r*c/255,g*c/255,b*c/255);
			    }
			}
		    }
		  else
		    for(int y = 0 ; y <= h ; y ++)
		      {
			int z = y*23/h;
			smallhistogram_type<rhythm_prop_sx> his_band = 
			  his.get_band(z);
			QRgb* line=(QRgb*)target.scanLine(h-y);
			co.setHsv(z*10,255,255);
			int r = co.red();
			int g = co.green();
			int b = co.blue();
			for(int v = 0 ; v < wid; v ++)
			  {
			    int x = v*rhythm_prop_sx/wid;
			    signed2 c = his_band.get_energy(x);
			    if (c<86)
			      c=0;
			    else
			      {
				c*=3;
				c/=2;
				c-=128;
			      }
			    line[v]=qRgb(r*c/255,g*c/255,b*c/255);
			  }
		      }
		  *picture=QPixmap::fromImage(target);
		}
	      return *picture;
	    }
	  return QVariant();
	case LIST_COMPOSITION:
	  if (!song->get_composition().empty())
	    {
	      int wid=selector->
		songListView->
		horizontalHeader()->
		sectionSize(LIST_COMPOSITION);
	      QPixmap *picture = composition_icon_cache.find(song,main,wid,h);
	      if (picture->isNull())
		{
		  QImage target(wid,h+1,QImage::Format_RGB32);
		  composition_property his = song->get_composition();
		  if (main && !main->get_composition().empty())
		    {
		      composition_property mis = main->get_composition();
		      for(int y = 0 ; y <= h ; y ++)
			{
			  int z = y*23/h;
			  smallhistogram_type<composition_prop_sx> his_band = 
			    his.get_band(z);
			  smallhistogram_type<composition_prop_sx> mis_band = 
			    mis.get_band(z);
			  QRgb* line=(QRgb*)target.scanLine(h-y);
			  signed2 hisbandscale=his_band.scale*256.0;
			  signed2 misbandscale=mis_band.scale*256.0;
			  for(int v = 0 ; v < wid ; v ++)
			    {
			      int x = v*composition_prop_sx/wid;
			      signed2 c1 =((signed2)his_band.get_energy(x)-127)
			      * hisbandscale;
			      signed2 c2 =((signed2)mis_band.get_energy(x)-127)
			      * misbandscale;
			      if (c1>c2)
				{
				  signed2 c = (int)((c1-c2)/25);
				  if (c>255) c = 255;
				  line[v]=qRgb(c,c,0);
				}
			      else
				{
				  signed2 c = (int)((c2-c1)/25);
				  if (c>255) c = 255;
				  line[v]=qRgb(0,c,c);
				}
			    }
			}
		    }
		  else
		    for(int y = 0 ; y <= h ; y ++)
		      {
			int z = y*23/h;
			smallhistogram_type<composition_prop_sx> his_band = 
			  his.get_band(z);
			QRgb* line=(QRgb*)target.scanLine(h-y);
			for(int v = 0 ; v < wid; v ++)
			  {
			    int x = v*composition_prop_sx/wid;
			    signed2 c = his_band.get_energy(x);
			    if (c>127)
			      {
				c = (c-127)*2;
				line[v]=qRgb(c,c,0);
			      }
			    else
			      {
				c=(127-c)*2;
				line[v]=qRgb(0,c,c);
			      }
			  }
		      }
		  *picture=QPixmap::fromImage(target);
		}
	      return *picture;
	    }
	  return QVariant();
	}
    }
  
  assert(role!=Qt::DisplayRole);
  assert(role!=Qt::BackgroundRole);
  return QVariant();
}

QVariant QSong::headerData(int col, Qt::Orientation orientation, int role) const
{
  if (role!=Qt::DisplayRole || orientation!=Qt::Horizontal) return QVariant();
  switch(col)
    {
    case LIST_TEMPO: return "Tempo";
    case LIST_SPECTRUM: return "Spectrum";
    case LIST_DCOLOR: return "dColor";
    case LIST_TIME: return "Time";
    case LIST_CUES: return "Cues";
    case LIST_TITLE: return "Title";
    case LIST_AUTHOR: return "Author";
    case LIST_VERSION: return "Version";
    case LIST_TAGS: return "Tags";
    case LIST_ONDISK: return "OnDisk";
    case LIST_INDEX: return "Index";
    case LIST_MD5SUM: return "Md5";
    case LIST_FILE: return "Filename on disk";
    case LIST_MIN: return "Minimum";
    case LIST_MAX:   return "Maximum";
    case LIST_MEAN:return "Mean";
    case LIST_POWER:  return "Power";
    case LIST_HISTOGRAM: return "Echo";
    case LIST_RHYTHM: return "Rhythm";
    case LIST_COMPOSITION: return "Composition";
    default: return "NA";
    }
}
#endif // __loaded__qsong_cpp__
