/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__song_cpp__
#define __loaded__song_cpp__
using namespace std;
#line 1 "song.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <qpainter.h>
#include "players-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum-type.h"
#include "composition-property.h"
#include "tags.h"
#include "memory.h"
#include "statistics.h"
#include "song-metric.h"
#include "song.h"
#include "qstring-factory.h"

init_singleton_var(Song,max_alltime_total,int,1);

void Song::refill(Index &reader, bool allowed_to_write)
{
  if (allowed_to_write && reader.changed())
    reader.write_idx();
  /* copy everything to object */
  set_storedin(QStringFactory::create(reader.get_storedin()));
  set_tempo(reader.get_tempo());
  set_file(QStringFactory::create(reader.get_filename()));
  set_tags(Tags::parse_tags(reader.get_tags()));
  set_time(QStringFactory::create(reader.get_time()));
  set_md5sum(QStringFactory::create(reader.get_md5sum()));
  if (get_spectrum()!=no_spectrum) bpmdj_deallocate(get_spectrum());
  set_spectrum(reader.get_spectrum());
  set_title(QStringFactory::create(reader.get_display_title()));
  set_author(QStringFactory::create(reader.get_author()));
  set_version(QStringFactory::create(reader.get_version()));
  set_albums(reader.copy_albums());
  set_min_amp(reader.get_min());
  set_max_amp(reader.get_max());
  set_mean_amp(reader.get_mean());
  set_power(reader.get_power());
  set_histogram(reader.get_histogram());
  set_rhythm(reader.get_rhythm());
  set_composition(reader.get_composition());
  set_has_cues(reader.get_cue_z() || reader.get_cue_x() || reader.get_cue_c()
	       || reader.get_cue_v());
  int t = reader.get_playcount();
  set_alltime_playcount(t);
  if (t>get_max_alltime_total())
    set_max_alltime_total(t);
}

bool Song::has_all_cluster_fields()
{
  return get_spectrum()!=no_spectrum
    && get_tempo().valid()
    && !get_histogram().empty()
    && !get_composition().empty()
    && !get_rhythm().empty();
}

void Song::checkondisk()
{
  static struct stat scratchhere;
  QString songfilename = MusicDir + "/" + get_file();
  int result=stat(songfilename.toAscii().data(),&scratchhere);
  set_ondisk(result!=-1);
  set_ondisk_checked(true);
}

void Song::clearFloatingFields()
{
  set_played(false);
  init_played_author_at_time();
  init_color_distance();
  init_spectrum_string();
  init_distance_string();
}

bool Song::contains_tag(const tag_type tag)
{
  if (tag==tag_end) 
    return true;
  tag_type *tagp = get_tags();
  while(*tagp!=tag_end)
    if (*(tagp++)==tag) 
      return true;
  return false;
}

Song::Song()
{
  init_title();
  init_author();
  init_version();
  init_tempo();
  init_storedin();
  init_file();
  init_time();
  init_md5sum();
  init_spectrum();
  init_color();
  init_spectrum_string();
  init_distance_string();
  init_played();
  init_ondisk();
  init_has_cues();
  init_tags();
  init_min_amp();
  init_max_amp();
  init_mean_amp();
  init_power();
  init_played_author_at_time();
  init_histogram();
  init_rhythm();
  init_composition();
  init_ondisk_checked();
  init_alltime_playcount();
}

Song::Song(Index * idx, bool allowwrite, bool check_ondisk)
{
  init_ondisk();
  init_ondisk_checked();
  init_tags();
  init_spectrum();
  refill(*idx, allowwrite);
  clearFloatingFields();
  if (check_ondisk)
    checkondisk();
}

void Song::reread()
{
  Index reader(get_storedin());
  refill(reader);
}

void Song::setColor(QColor transfer)
{
  if (transfer.isValid())
    {
      set_color(transfer);
      set_spectrum_string(transfer.name());
    }
  else
    {
      set_color(QColor(255,255,255));
      set_spectrum_string(QString::null);
    }
}

/**
 * This routine is necessary for the cluster debug output.
 * Printing the songname might be a good idea
 */
void Song::simpledump(unsigned2 d)
{
  assert(0);
}

void Song::color_sub_elements(int a, int b, float4 d)
{
  // d equals 1 if it is the largest intra distance
  QColor c;
  if (a==-1)
    {
      c.setRgb(255,255,255);
      set_spectrum_string(EMPTY);
    }
  else
    {
      c.setHsv(a*360/b,255-(int)(d*255.0),255);
      set_spectrum_string(tonumber((unsigned4)a));
    }
  set_color(c);
}

void Song::determine_color(float4 hue, float4 dummy, int dummy2, int dummy3)
{
  QColor c;
  c.setHsv((int)hue,255,255);
  assert(c.isValid());
  set_color(c);
  set_spectrum_string(tonumber((unsigned4)hue));
}

bool Song::tempo_show(const Song* main, bool uprange, bool downrange)
{
  float8 d = SongMetriek::std.tempo_diff(*this,*main);
  int harmonic = 0;
  d = SongMetriek::std.find_harmonic(d,harmonic);
  if (uprange   && d >= 0 && d <= 1) return true;
  if (downrange && d <= 0 && d >= -1) return true;
  return false; // there is no reason to show the song with this tempo;
}

tempo_type Song::tempo_between(Song* song,  float4 percent)
{
  return between_tempos(get_tempo(),song->get_tempo(),percent);
}

QColor Song::color_between(Song* song, float4 percent)
{
  QColor a = get_color();
  QColor b = song->get_color();
  QColor result;
  int r1,g1,b1;
  int r2,g2,b2;
  a.getRgb(&r1,&g1,&b1);
  b.getRgb(&r2,&g2,&b2);
  float4 R1,G1,B1;
  float4 R2,G2,B2;
  float4 R3,G3,B3;
  R1=r1;
  G1=g1;
  B1=b1;
  R2=r2;
  B2=b2;
  G2=g2;
  R3=(R2-R1)*percent+R1;
  G3=(G2-G1)*percent+G1;
  B3=(B2-B1)*percent+B1;
  result.setRgb((int)R3,(int)G3,(int)B3);
  return result;
}

float4 Song::distance(Point* point, Metriek* dp)
{
  return distance(point,dp,1000);
}

float4 Song::distance(Point* point, Metriek* dp, float8 limit)
{
  SongMetriek * sm = (SongMetriek*)dp;
  return sm->distance((const Song &)*this,(const Song &)*(Song*)point,limit);
}

float4 Song::distance(Song* a, float4 wa, Song* b, float4 wb, Metriek * dp)
{
  float8 d1 = distance(a,dp,100000);
  float8 d2 = distance(b,dp,100000);
  float8 d  = (d1*wa+d2*wb)/(wa+wb);
  return d;
}

bool Song::get_distance_to_main(float4 limit)
{
  set_color_distance(2);
  if (get_spectrum()!=no_spectrum)
    if (::main_song)
      {
	if (songs_to_avoid.size())
	  {
	    int s = songs_to_avoid.size();
	    float8 d = SongMetriek::std.distance(*this, *::main_song,2);
	    for(int i = 0 ; i < s ; i++)
	      d+=1.0-SongMetriek::std.distance(*this, *songs_to_avoid[i],2);
	    d/=s+1.0;
	    set_color_distance(d);
	  }
	else
	  set_color_distance(SongMetriek::std.distance(*this, *::main_song,
						       limit));
      }
  if (get_color_distance()>1.0)
    set_distance_string(QString::null);
  else
    set_distance_string(tonumber((unsigned4)(get_color_distance()*256)));
  return get_color_distance()<=1.0;
}

Song::~Song()
{
}

QString Song::getDisplayTitle()
{
  QString result  = "";
  if (!get_title().isNull())
    result+=get_title();
  if (!get_author().isNull())
    result+=QString("[")+get_author()+QString("]");
  return result;
}

QString Song::tempo_str()
{
  return get_tempo().qstring();
}

bool Song::needs_analysis()
{
  return needs_energy()
    || needs_tempo()
    || needs_spectrum()
    || needs_echo()
    || needs_rhythm()
    || needs_composition();
}
#endif // __loaded__song_cpp__
