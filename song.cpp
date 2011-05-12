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
#include <sys/types.h>
#include <sys/stat.h>
#include <qpainter.h>
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum-type.h"
#include "composition-property.h"
#include "tags.h"
#include "memory.h"
#include "song-statistics.h"
#include "song-metric.h"

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
  if (get_spectrum()!=no_spectrum) deallocate(get_spectrum());
  set_spectrum(reader.get_spectrum());
  set_title(QStringFactory::create(reader.get_display_title()));
  set_author(QStringFactory::create(reader.get_display_author()));
  set_version(QStringFactory::create(reader.get_display_version()));
  set_albums(reader.copy_albums());
  set_min_amp(reader.get_min());
  set_max_amp(reader.get_max());
  set_mean_amp(reader.get_mean());
  set_power(reader.get_power());
  set_histogram(reader.get_histogram());
  set_rythm(reader.get_rythm());
  set_composition(reader.get_composition());
  set_has_cues(reader.get_cue_z() + reader.get_cue_x() + reader.get_cue_c() + reader.get_cue_v());
}

bool Song::has_all_cluster_fields()
{
  return get_spectrum()!=no_spectrum
    && get_tempo().valid()
    && !get_histogram().empty()
    && !get_composition().empty()
    && !get_rythm().empty();
}

void Song::checkondisk()
{
  QString songfilename = MusicDir + "/" + get_file();
  // printf("Checking ondisk of %s",(const char*)songfilename);
  set_ondisk(exists(songfilename));
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
  init_rythm();
  init_composition();
  init_ondisk_checked();
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
  Index reader((const char*)get_storedin());
  // if (reader.changed()) reader.write_idx();
  refill(reader);
}


void Song::realize()
{
  // hier creeeren we een nieuwe index file omdat kbpm-play enkel met index files overweg kan
  // Dit vereist natuurlijk een unieke naam
  if (get_storedin().endsWith(".bib"))
    {
      Index transfer(get_storedin());
      char * proposal = transfer.readable_description();
      char fullprop[500];
      sprintf(fullprop,"%s.idx",proposal);
      char * uniquename = findUniqueName(fullprop);
      printf("Debug: realizing song %s as %s\n",proposal, uniquename);
      set_storedin(uniquename);
      transfer.nolonger_inbib();
      transfer.write_idx(get_storedin());
      deallocate(uniquename);
      deallocate(proposal);
    }
}

void Song::setColor(QColor transfer)
{
  set_color(transfer);
  set_spectrum_string(get_color().isValid() ? get_color().name() : QString::null);
}

void Song::simpledump(int d)
{
}

void Song::color_sub_elements(int a, int b, float d)
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
      set_spectrum_string(tonumber(a));
    }
  set_color(c);
}

void Song::determine_color(float hue, float dummy, int dummy2, int dummy3)
{
  QColor c;
  c.setHsv((int)hue,255,255);
  set_color(c);
  set_spectrum_string(tonumber((int)hue));
}


bool Song::tempo_show(const Song* main, bool uprange, bool downrange)
{
  double d = SongMetriek::std.tempo_diff(*this,*main);
  int harmonic = 0;
  d = SongMetriek::std.find_harmonic(d,harmonic);
  if (uprange   && d >= 0 && d <= 1) return true;
  if (downrange && d <= 0 && d >= -1) return true;
  return false; // there is no reason to show the song with this tempo;
}

tempo_type Song::tempo_between(Song* song,  float percent)
{
  return between_tempos(get_tempo(),song->get_tempo(),percent);
}

QColor Song::color_between(Song* song, float percent)
{
  QColor a = get_color();
  QColor b = song->get_color();
  QColor result;
  int r1,g1,b1;
  int r2,g2,b2;
  a.rgb(&r1,&g1,&b1);
  b.rgb(&r2,&g2,&b2);
  float R1,G1,B1;
  float R2,G2,B2;
  float R3,G3,B3;
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

float Song::distance(Point* point, Metriek* dp)
{
  return distance(point,dp,1000);
}

float Song::distance(Point* point, Metriek* dp, double limit)
{
  SongMetriek * sm = (SongMetriek*)dp;
  return sm->distance((const Song &)*this,(const Song &)*(Song*)point,limit);
}

Point* Song::percentToward(Point * other, Metriek * dp, float percent)
{
  Song * song = (Song*)other;
  SongMetriek * measure = (SongMetriek*)dp;
  assert(other);
  assert(measure);
  Song * result = new Song();
  if (measure->tempo)
    result->set_tempo(tempo_between(song,percent));
  if (measure->spectrum)
    {
      result -> set_spectrum ( between_spectra(get_spectrum(),song->get_spectrum(),percent) );
      result -> setColor( color_between(song,percent) );
    }
  return result;
}

bool Song::get_distance_to_main(float limit)
{
  set_color_distance(2);
  if (get_spectrum()!=no_spectrum)
    if (ProcessManager::playingInMain())
      set_color_distance(SongMetriek::std.distance(*this, *ProcessManager::playingInMain(),limit));
  if (get_color_distance()>1.0)
    set_distance_string(QString::null);
  else
    set_distance_string(tonumber((int)(get_color_distance()*256)));
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
