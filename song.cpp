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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <qpainter.h>
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum.h"
#include "tags.h"
#include "memory.h"

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
  set_spectrum(reader.get_spectrum_copy());
  set_title(QStringFactory::create(reader.get_display_title()));
  set_author(QStringFactory::create(reader.get_display_author()));
  set_version(QStringFactory::create(reader.get_display_version()));
  set_albums(reader.copy_albums());
  set_min_amp(reader.get_min());
  set_max_amp(reader.get_max());
  set_mean_amp(reader.get_mean());
  set_power(reader.get_power());
  /* are there any cues stored */
  set_has_cues(reader.get_cue_z() + reader.get_cue_x() + reader.get_cue_c() + reader.get_cue_v());
}

void Song::checkondisk()
{
  QString songfilename = MusicDir + "/" + get_file();
  set_ondisk(exists(songfilename));
}

void Song::clearFloatingFields()
{
  set_played(Played::IsPlayed(this));
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
}

Song::Song(Index * idx, bool allowwrite, bool check_ondisk, bool accountspectrum)
{
  init_ondisk();
  init_tags();
  init_spectrum();
  refill(*idx, allowwrite);
  clearFloatingFields();
  if (accountspectrum)
    new_spectrum(get_spectrum());
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
  // wat we hier doen is een nieuwe index file creeeren en deze op disk schrijven. Dit vereist natuurlijk een unieke naam :)
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

float Song::tempo_n_distance(float harmonic, Song* song)
{
  tempo_type fa = get_tempo();
  tempo_type fb = song->get_tempo();
  if (fa.none() || fb.none()) return 1000;
  float ffa = fa.tempo;
  float ffb = fb.tempo;
  ffa*=harmonic;
  float sa = ffa * Config::distance_temposcale;
  float sb = ffb * Config::distance_temposcale;
  float s = (sa < sb ? sa : sb);
  return (ffa-ffb)/s;
}

float Song::tempo_distance(float harmonic, Song* song)
{
  return(fabs(tempo_n_distance(harmonic, song)));
}

float Song::tempo_distance(Song* song)
{
  return tempo_distance(1.0,song);
}

tempo_type Song::tempo_between(Song* song,  float percent)
{
  return between_tempos(get_tempo(),song->get_tempo(),percent);
}

float Song::spectrum_distance(Song* song)
{
  if (get_spectrum()==no_spectrum || song->get_spectrum()==no_spectrum)
    return 1000000;
  spectrum_type b = song->get_spectrum();
  spectrum_freq distance=0;
  if (Config::log_spectrum_distance)
    {
      // first calculate mean energy mismatch
      spectrum_freq mean = 0.0;
      int cnt = 0;
      for (int i = 0; i < spectrum_size ; i ++ )
	{
	  spectrum_freq mismatch = 0;
	  if (b[i] > 0 && get_spectrum()[i] > 0 && ((mismatch = get_spectrum()[i]/b[i]) > 0)) // assignment intended
	    {
	      mean += 10.0*log(mismatch);
	      cnt++;
	    }
	}
      if (cnt==0) 
	mean = 0;
      else
	mean/=(spectrum_freq)cnt;
      // now calculate the actual energy mismatch without taking the mean
      // fix into account
      for (int i = 0; i < spectrum_size ; i ++ )
	{
	  spectrum_freq mismatch = 0;
	  if (b[i] > 0 && get_spectrum()[i] > 0)
	    {
	      mismatch = get_spectrum()[i]/b[i];
	      if (mismatch>0)
		mismatch = 10.0*log(mismatch) - mean;
	      else
		mismatch = 0;
	    }
	  distance+=fabs(mismatch);
	}
      distance /= 9.0 * spectrum_size;
    }
  else
    {
      for (int i = 0; i < spectrum_size ; i ++ )
	{
	  spectrum_freq mismatch = get_spectrum()[i]-b[i];
	  mismatch*=scales[i]; // to normalize the weight of this band
	  mismatch*=mismatch;
	  distance+=mismatch;
	}
      distance *= 0.048828;
      // multiplying by this value is done in order to keep the same scale as
      // previous versions...
    }
  return distance * Config::distance_spectrumscale;
}

spectrum_type Song::spectrum_between(Song* song, float percent)
{
  if (get_spectrum() == no_spectrum) return no_spectrum;
  spectrum_type b = song->get_spectrum();
  if (b == no_spectrum) return no_spectrum;
  spectrum_type result=allocate_spectrum();
  assert(percent>=0.0 && percent <=1.0);
  for (int i = 0; i < spectrum_size ; i++)
    result[i] = get_spectrum()[i]*(1.0-percent) + b[i]*percent;
  return result;
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
  // printf("%g %g %g  // %g %g %g // %g %g %g\n",R1,G1,B1,R2,G2,B2,R3,G3,B3);
  result.setRgb((int)R3,(int)G3,(int)B3);
  return result;
}

float Song::distance(Point* point, Metriek* dp)
{
  Song *song = (Song*)point;
  SongMetriek *measure = (SongMetriek*)dp;
  float total = measure -> tempo + measure -> spectrum;
  if (total == 0) return 1000;
  float sum = 0;
  if (measure->tempo > 0)
    sum+=tempo_distance(song)*measure->tempo;
  if (measure->spectrum > 0)
    sum+=spectrum_distance(song)*measure->spectrum;
  return sum/total;
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
      result -> set_spectrum ( spectrum_between(song,percent) );
      result -> setColor(color_between(song,percent));
    }
  return result;
}

static SongMetriek spectrum_distance(0.0,1.0);

bool Song::getDistance()
{
  set_color_distance(2);
  if (get_spectrum()!=no_spectrum)
    if (ProcessManager::playingInMain())
      set_color_distance(distance(ProcessManager::playingInMain(),&::spectrum_distance));
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
