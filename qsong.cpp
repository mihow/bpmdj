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
#line 1 "qsong.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <qpainter.h>
#include <stdlib.h>
#include "songselector.logic.h"
#include "song-metric.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "tags.h"
#include "memory.h"
#include "pixmap-cache.h"

//-----------------------------------------------------------
//            qsong
//-----------------------------------------------------------
init_singleton_var(QSong,selected,bool*,NULL);
init_singleton_var(QSong,compare_col,int,LIST_TEMPO);
init_singleton_var(QSong,compare_asc,bool,true);
init_singleton_var(QSong,songs,Song**,NULL);
init_singleton_var(QSong,song_count,int,0);
    
static PixmapCache echo_icon_cache; 
static PixmapCache rythm_icon_cache;
static PixmapCache composition_icon_cache;

void QSong::setVector(Song**arr, int cnt)
{
  if (get_songs()) bpmdj_deallocate(get_songs());
  if (get_selected()) bpmdj_deallocate(get_selected());
  set_songs(arr);
  set_song_count(cnt);
  set_selected(bpmdj_allocate(get_song_count(),bool));
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

QColor * QSong::colorOfTempoCol(const Song* main, Song* song)
{
  if (!Config::color_range) return NULL;
  if (!main) return NULL;
  if (song->get_tempo().none()) return NULL;
  double d = SongMetriek::std.tempo_diff(*main,*song);
  int harmonic = 0;
  d = SongMetriek::std.find_harmonic(d,harmonic);
  d = fabs(d);
  if ( d > 1 ) return NULL;
  if (harmonic == 0) return mixColor(Config::get_color_tempo44(),d,Qt::white);
  if (harmonic == 1) return mixColor(Config::get_color_tempo54(),d,Qt::white);
  if (harmonic == 2) return mixColor(Config::get_color_tempo64(),d,Qt::white);
  if (harmonic == 3) return mixColor(Config::get_color_tempo74(),d,Qt::white);
  if (harmonic == 4) return mixColor(Config::get_color_tempo84(),d,Qt::white);
  return NULL;
}

QColor * QSong::colorOfAuthorCol(Song* song)
{
  if (!Config::color_authorplayed) return NULL;
  int played_author_songs_ago = History::get_songs_played() - song->get_played_author_at_time();
  if (played_author_songs_ago < Config::get_authorDecay())
    return mixColor(Config::get_color_played_author(), (float)played_author_songs_ago/(float)Config::get_authorDecay() ,Qt::white);
  return NULL;
}

QColor * QSong::colorOfPlaycount(Song* song)
{
  float count = song->get_alltime_playcount();
  float total = song->get_max_alltime_total();
  if (count)
    return mixColor(Qt::white, count/total, Config::get_color_alltime());
  return NULL;
}

QColor * QSong::colorOfdColorCol(Song* song)
{
  if (!Config::color_dcolor) return NULL;
  if (song->get_color_distance() > 1.0) return NULL;
  return mixColor(Config::get_color_dcolor_col(), song -> get_color_distance(), Qt::white);
}

void QSong::paintCell(QVectorView* vv, int i, QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  Song * main = ::main_song;
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
      if ((color=colorOfTempoCol(main,song)))
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
	  ncg.setColor(QColorGroup::Base,Config::get_color_played_song());
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_AUTHOR:
      if ((color=colorOfAuthorCol(song)))
	{
	  QColorGroup ncg(cg);
          ncg.setColor(QColorGroup::Base,*color);
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  delete(color);
	  return;
	}
      break;
      
    case LIST_DCOLOR:
      if ((color=colorOfdColorCol(song)))
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
    case LIST_HISTOGRAM:
      if (!song->get_histogram().empty())
	{
	  int h = height() - 1;
	  // do we have a cached version ?
	  QPixmap * picture = echo_icon_cache.find(song,main,echo_prop_sx,h);
	  if (picture->isNull())
	    {
	      picture->resize(echo_prop_sx,h+1);
	      QPainter icon;
	      icon.begin(picture);
	      
	      echo_property his = song->get_histogram();
	      if (main && !main->get_histogram().empty())
		{
		  echo_property mis = main->get_histogram();
		  for(int y = 0 ; y <= h ; y ++)
		    {
		      int z = y*23/h;
		      smallhistogram_type<echo_prop_sx> his_band = his.get_band(z);
		      smallhistogram_type<echo_prop_sx> mis_band = mis.get_band(z);
		      signed2 c1,c2;
		      for(int x = 0 ; x < echo_prop_sx ; x ++)
			{
			  c1 = his_band.get_probability(x);
			  c2 = mis_band.get_probability(x);
			  icon.setPen(QColor(c1,(c1+c2)/2,c2));
			  icon.drawPoint(x,h-y);
			}
		    }
		}
	      else
		for(int y = 0 ; y <= h ; y ++)
		  {
		    int x,z = y*23/h;
		    smallhistogram_type<echo_prop_sx> his_band = his.get_band(z);
		    unsigned1 c;
		    for(x = 0 ; x < echo_prop_sx ; x ++)
		      {
			c = his_band.get_probability(x);
			icon.setPen(QColor(c,c,c));
			icon.drawPoint(x,h-y);
		      }
		  }
	      icon.end();
	    }

	  p->drawPixmap(0,0,*picture);
	  if (wid>echo_prop_sx)
	    {
	      p->setPen(Qt::white);
	      p->eraseRect(96,0,wid-96,24);
	    }
	  return;
	}
      break;
    case LIST_RYTHM:
      if (!song->get_rythm().empty())
	{
	  int h = height() - 1;
	  QPixmap *picture = rythm_icon_cache.find(song,main,wid,h);
	  if (picture->isNull())
	    {
	      picture->resize(wid,h+1);
	      QPainter icon;
	      icon.begin(picture);
	      
	      rythm_property his = song->get_rythm();
	      QColor co;
	      if (main && !main->get_rythm().empty())
		{
		  rythm_property mis = main->get_rythm();
		  for(int y = 0 ; y <= h ; y ++)
		    {
		      int z = y*23/h;
		      smallhistogram_type<rythm_prop_sx> his_band = his.get_band(z);
		      smallhistogram_type<rythm_prop_sx> mis_band = mis.get_band(z);
		      co.setHsv(z*10,255,255);
		      int r = co.red();
		      int g = co.green();
		      int b = co.blue();
		      for(int v = 0 ; v < wid ; v ++)
			{
			  int x = v*rythm_prop_sx/wid;
			  signed2 c1 = his_band.get_energy(x);
			  signed2 c2 = mis_band.get_energy(x);
			  if (c1<86) c1=86;
			  if (c2<86) c2=86;
			  signed2 c = abs(c1-c2) * 3 / 2;
			  co.setRgb(r*c/255,g*c/255,b*c/255);
			  icon.setPen(co);
			  icon.drawPoint(v,h-y);
			}
		    }
		}
	      else
		for(int y = 0 ; y <= h ; y ++)
		  {
		    int z = y*23/h;
		    smallhistogram_type<rythm_prop_sx> his_band = his.get_band(z);
		    co.setHsv(z*10,255,255);
		    int r = co.red();
		    int g = co.green();
		    int b = co.blue();
		    for(int v = 0 ; v < wid; v ++)
		      {
			int x = v*rythm_prop_sx/wid;
			signed2 c = his_band.get_energy(x);
			if (c<86)
			  c=0;
			else
			  {
			    c*=3;
			    c/=2;
			    c-=128;
			  }
			co.setRgb(r*c/255,g*c/255,b*c/255);
			icon.setPen(co);
			icon.drawPoint(v,h-y);
		      }
		  }
	      
	      icon.end();
	    }
	  p->drawPixmap(0,0,*picture);
	  return;
	}
      break;
    case LIST_COMPOSITION:
      if (!song->get_composition().empty())
	{
	  int h = height() - 1;
	  QPixmap *picture = composition_icon_cache.find(song,main,wid,h);

	  if (picture->isNull())
	    {
	      picture->resize(wid,h+1);
	      QPainter icon;
	      icon.begin(picture);
	      
	      composition_property his = song->get_composition();
	      QColor co;
	      if (main && !main->get_composition().empty())
		{
		  composition_property mis = main->get_composition();
		  for(int y = 0 ; y <= h ; y ++)
		    {
		      int z = y*23/h;
		      smallhistogram_type<composition_prop_sx> his_band = his.get_band(z);
		      smallhistogram_type<composition_prop_sx> mis_band = mis.get_band(z);
		      co.setHsv(z*10,255,255);
		      for(int v = 0 ; v < wid ; v ++)
			{
			  int x = v*composition_prop_sx/wid;
			  float c1 = (his_band.get_energy(x)-127.0)*his_band.scale;
			  float c2 = (mis_band.get_energy(x)-127.0)*mis_band.scale;
			  if (c1>c2)
			    {
			      int c = (int)((c1-c2)*10);
			      if (c>255) c = 255;
			      co.setRgb(c,c,0);
			    }
			  else
			    {
			      int c = (int)((c2-c1)*10);
			      if (c>255) c = 255;
			      co.setRgb(0,c,c);
			    }
			  icon.setPen(co);
			  icon.drawPoint(v,h-y);
			}
		    }
		}
	      else
		for(int y = 0 ; y <= h ; y ++)
		  {
		    int z = y*23/h;
		    smallhistogram_type<composition_prop_sx> his_band = his.get_band(z);
		    for(int v = 0 ; v < wid; v ++)
		      {
			int x = v*composition_prop_sx/wid;
			signed2 c = his_band.get_energy(x);
			if (c>127)
			  {
			    c = (c-127)*2;
			    co.setRgb(c,c,0);
			  }
			else
			  {
			    c=(127-c)*2;
			    co.setRgb(0,c,c);
			  }
			icon.setPen(co);
			icon.drawPoint(v,h-y);
		      }
		  }
	      icon.end();
	    }
	  p->drawPixmap(0,0,*picture);
	  return;
	}
      break;
    }
  
  // the normal color depends wether the song is on the disk or not
  if (Config::color_ondisk)
    {
      if (!song->get_ondisk_checked())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,Config::get_color_unchecked());
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  return;
	}
      else if (!song->get_ondisk())
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,Config::get_color_unavailable());
	  QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	  return;
	}
    }

  // how many times did the song play ?
  if (col==LIST_TITLE || col==LIST_AUTHOR || col==LIST_TIME
      || col == LIST_VERSION || col==LIST_TAGS || col == LIST_INDEX 
      || col == LIST_MD5SUM || col==LIST_FILE || col == LIST_MIN
      || col == LIST_MAX || col==LIST_MEAN || col==LIST_POWER)
    if (Config::color_songs_based_on_history && (color=colorOfPlaycount(song)))
      {
	QColorGroup ncg(cg);
	ncg.setColor(QColorGroup::Base,*color);
	QVectorViewData::paintCell(vv,i,p,ncg,col,wid,align);
	delete(color);
	return;
      }
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
    case LIST_HISTOGRAM:
    case LIST_RYTHM:
    case LIST_COMPOSITION:
      return EMPTY;
    default:
      assert(0);
    }
  return "NA";
  
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

static int compare_songs_rythm(const void * a, const void * b)
{
  assert(a);
  assert(b);
  Song * A = *((Song **)a);
  Song * B = *((Song **)b);
  assert(A);
  assert(B);
  rythm_property AT = A->get_rythm();
  rythm_property BT = B->get_rythm();
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
  else if (get_compare_col()==LIST_RYTHM)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_rythm);
    } 
  else if (get_compare_col()==LIST_COMPOSITION)
    {
      qsort(get_songs(),get_song_count(),sizeof(Song*),compare_songs_composition);
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
