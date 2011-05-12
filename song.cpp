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
#include "qstring-factory.h"
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum.h"
#include "tags.h"

void Song::refill(Index &reader, bool allowed_to_write)
{
  if (allowed_to_write && reader.changed())
    reader.write_idx();
  /* copy everything to object */
  storedin = QStringFactory::create(reader.get_storedin());
  tempo = reader.get_tempo();
  file = QStringFactory::create(reader.get_filename());
  tags = Tags::parse_tags(reader.get_tags());
  time = QStringFactory::create(reader.get_time());
  md5sum = QStringFactory::create(reader.get_md5sum());
  if (spectrum!=no_spectrum) deallocate(spectrum);
  spectrum = reader.get_spectrum_copy();
  title = QStringFactory::create(reader.get_display_title());
  author = QStringFactory::create(reader.get_display_author());
  version = QStringFactory::create(reader.get_display_version());
  albums = reader.copy_albums();
  /* are there any cues stored */
  has_cues = reader.get_cue_z() + reader.get_cue_x() + reader.get_cue_c() + reader.get_cue_v();
}

void Song::checkondisk()
{
  QString songfilename = MusicDir + "/" + file;
  ondisk = exists(songfilename);
}

void Song::clearFloatingFields()
{
  played=Played::IsPlayed(this);
  played_author_at_time = -100;
  color_distance = 0;
  spectrum_string = "";
  distance_string = QString::null;
}

bool Song::contains_tag(const tag_type tag)
{
  if (tag==tag_end) 
    return true;
  tag_type *tagp = tags;
  while(*tagp!=tag_end)
    if (*(tagp++)==tag) 
      return true;
  return false;
}

Song::Song()
{
  title = "";
  author = "";
  version = "";
  tempo = no_tempo;
  storedin = "";
  file = "";
  time = "";
  md5sum = "";
  spectrum = no_spectrum;
  color.setRgb(127,127,127);
  spectrum_string = "";
  distance_string = QString::null;
  played = false;
  ondisk = true;
  has_cues = false;
  tags = NULL;
  played_author_at_time = -100;
}

Song::Song(Index * idx, bool allowwrite, bool check_ondisk, bool accountspectrum)
{
  ondisk = false;
  tags = NULL;
  spectrum = no_spectrum;
  refill(*idx, allowwrite);
  clearFloatingFields();
  if (accountspectrum)
    new_spectrum(spectrum);
  if (check_ondisk)
    checkondisk();
}

void Song::reread()
{
  Index reader((const char*)storedin);
  // if (reader.changed()) reader.write_idx();
  refill(reader);
}
 

void Song::realize()
{
  // wat we hier doen is een nieuwe index file creeeren en deze op disk schrijven. Dit vereist natuurlijk een unieke naam :)
  if (storedin.endsWith(".bib"))
    {
      Index transfer(storedin);
      char * proposal = transfer.readable_description();
      char fullprop[500];
      sprintf(fullprop,"%s.idx",proposal);
      char * uniquename = findUniqueName(fullprop);
      printf("Debug: realizing song %s as %s\n",proposal, uniquename);
      storedin = uniquename;
      transfer.nolonger_inbib();
      transfer.write_idx(storedin);
      deallocate(uniquename);
      deallocate(proposal);
    }
}

void Song::setColor(QColor transfer)
{
  color = transfer;
  spectrum_string = color.isValid() ? color.name() : QString::null;
}

void Song::simpledump(int d)
{
}

void Song::color_sub_elements(int a, int b, float d)
{
  // d equals 1 if it is the largest intra distance
  if (a==-1)
    {
      color.setRgb(255,255,255);
      spectrum_string= EMPTY;
    }
  else
    {
      color.setHsv(a*360/b,255-(int)(d*255.0),255);
      //color.setHsv(a*360/b,255,255);
      spectrum_string = tonumber(a);
    }
}

void Song::determine_color(float hue, float dummy, int dummy2, int dummy3)
{
  color.setHsv((int)hue,255,255);
  spectrum_string = tonumber((int)hue);
}

float Song::tempo_n_distance(float harmonic, Song* song)
{
  float fa = tempo;
  float fb = song->tempo;
  if (fa == no_tempo || fb == no_tempo) return 1000;
  fa*=harmonic;
  float sa = fa * Config::distance_temposcale;
  float sb = fb * Config::distance_temposcale;
  float s = (sa < sb ? sa : sb);
  return (fa-fb)/s;
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
  float ta = tempo;
  float tb = song->tempo;
  return (tb-ta)*percent+ta;
}

float Song::spectrum_distance(Song* song)
{
  if (spectrum==no_spectrum || song->spectrum==no_spectrum)
    return 1000000;
  spectrum_type b = song->spectrum;
  spectrum_freq distance=0;
  if (Config::log_spectrum_distance)
    {
      // first calculate mean energy mismatch
      spectrum_freq mean = 0.0;
      int cnt = 0;
      for (int i = 0; i < spectrum_size ; i ++ )
	{
	  spectrum_freq mismatch = 0;
	  if (b[i] > 0 && spectrum[i] > 0 && ((mismatch = spectrum[i]/b[i]) > 0)) // assignment intended
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
	  if (b[i] > 0 && spectrum[i] > 0)
	    {
	      mismatch = spectrum[i]/b[i];
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
	  spectrum_freq mismatch = spectrum[i]-b[i];
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
  if (spectrum == no_spectrum) return no_spectrum;
  spectrum_type b = song->spectrum;
  if (b == no_spectrum) return no_spectrum;
  spectrum_type result=allocate_spectrum();
  assert(percent>=0.0 && percent <=1.0);
  for (int i = 0; i < spectrum_size ; i++)
    result[i] = spectrum[i]*(1.0-percent) + b[i]*percent;
  return result;
}

QColor Song::color_between(Song* song, float percent)
{
  QColor a = color;
  QColor b = song->color;
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
  if (measure->tempo != no_tempo)
    result->tempo = tempo_between(song,percent);
  if (measure->spectrum)
    {
      result -> spectrum = spectrum_between(song,percent);
      result -> setColor(color_between(song,percent));
    }
  return result;
}

static SongMetriek spectrum_distance(0.0,1.0);

bool Song::getDistance()
{
  color_distance=2;
  if (spectrum!=no_spectrum)
    if (ProcessManager::playingInMain())
      color_distance = distance(ProcessManager::playingInMain(),&::spectrum_distance);
  if (color_distance>1.0)
    distance_string = QString::null;
  else
    distance_string = tonumber((int)(color_distance*256));
  return color_distance<=1.0;
}

Song::~Song()
{
}

QString Song::getDisplayTitle()
{
  QString result  = "";
  if (!title.isNull())
    result+=title;
  if (!author.isNull())
    result+=QString("[")+author+QString("]");
  return result;
}

QString Song::tempo_str()
{
  // we don't use the factory here because these are only numbers and creating
  // them would simply fill up the factory for nothing: after their usage
  // in listviewitems these are destroyed again
  if (tempo==no_tempo) return slash;
  else if (tempo>=100.0) return QString::number(tempo);
  else if (tempo>0) return zero+QString::number(tempo);
  return QString::number(tempo);
}
