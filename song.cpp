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

//float SongMetriek::tempo_scale = 0.06; 
//float SongMetriek::spectrum_scale = 1.0/512.0;
SongMetriek SongMetriek::SPECTRUM(false,false,true,false);
SongMetriek SongMetriek::TEMPO(true,true,false,false);
SongMetriek SongMetriek::ALL(true,true,true,false);
SongMetriek SongMetriek::ALL_WITHOUT_TEMPO_WEIGHT(true,false,true,false);
SongMetriek SongMetriek::PATTERN(false,false,false,true);

void Song::refill(Index &reader, bool allowed_to_write)
{
  if (allowed_to_write && reader.changed())
    reader.write_idx();
  /* copy everything to object */
  storedin = QStringFactory::create(reader.get_storedin());
  tempo = QStringFactory::create(reader.get_tempo_str());
  file = QStringFactory::create(reader.get_filename());
  tags = QStringFactory::create(reader.get_tags());
  time = QStringFactory::create(reader.get_time());
  md5sum = QStringFactory::create(reader.get_md5sum());
  spectrum = QStringFactory::create(reader.get_spectrum());
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
  distance_string = "";
}

bool Song::containsTag(const QString tag)
{
  if (!tags) 
    return false;
  return tags.contains(tag)>0;
}


Song::Song()
{
  title = "";
  author = "";
  version = "";
  tempo = "";
  storedin = "";
  tags = "";
  file = "";
  time = "";
  md5sum = "";
  spectrum = "";
  color.setRgb(127,127,127);
  spectrum_string = "";
  distance_string = "";
  played = false;
  ondisk = true;
  has_cues = false;
  played_author_at_time = -100;
}

Song::Song(Index * idx, bool allowwrite, bool check_ondisk, bool accountspectrum)
{
  ondisk = false;
  refill(*idx, allowwrite);
  clearFloatingFields();
  if (accountspectrum)
    newSpectrum(spectrum);
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
      free(uniquename);
      free(proposal);
    }
}

QString tonumber(const int b)
{
  return ( b < 10 ?
	   QString("00")+QString::number(b) :
	   ( b < 100 ?
	     QString("0")+QString::number(b) :
	     QString::number(b)));
}

QString tonumber(const float f)
{
  return QString::number(f);
}

void Song::setColor(QColor transfer)
{
  color = transfer;
  spectrum_string = ( !color.isValid() ? QString::null :
		      color.name());
}

void Song::simpledump(int d)
{
}

void Song::determine_color(float hue, float dummy, int dummy2, int dummy3)
{
  color.setHsv((int)hue,255,255);
  spectrum_string = tonumber((int)hue);
}

float Song::tempo_distance(Song* song)
{
  bool ta = tempo==QString::null;
  bool tb = song->tempo==QString::null;
  if (ta || tb)
    return 1000;
  float fa = atof((const char*)tempo);
  float fb = atof((const char*)song->tempo);
  float sa = fa * Config::distance_temposcale;
  float sb = fb * Config::distance_temposcale;
  float s = (sa < sb ? sa : sb);
  float d = fabs(fa-fb);
  d/=s;
  return d;
}

QString Song::tempo_between(Song* song,  float percent)
{
  float ta = atof(tempo);
  float tb = atof(song->tempo);
  float t  = (tb-ta)*percent+ta;
  return QString::number(t);
}

float Song::spectrum_distance(Song* song)
{
  bool as = spectrum.isNull();
  bool bs = song->spectrum.isNull();
  if (as || bs)
    return 1000000;
  QString b = song->spectrum;
  float distance=0;
  for (int i = 0; i<24;i++)
    {
      char letter1 = spectrum.at(i).latin1();
      char letter2 = b.at(i).latin1();
      float mismatch=letter1-letter2;
      mismatch*=scales[i];
      mismatch*=mismatch;
      distance+=mismatch;
    }
  return distance*Config::distance_spectrumscale/512.0;
}

/*float Song::pattern_distance(Song* song)
{
  if (pattern_size==0 || song->pattern_size==0)
    return 1000000;
  if (pattern_size > song->pattern_size)
    return song->pattern_distance(this);
    // pattern_size is de grootste van de twee
  int phase = 0;
  long minimum=0;
  for(long position = 0; position < pattern_size; position ++)
    {
      long pos2 = position * song->pattern_size/pattern_size;
      long d = (int)pattern[position]-(int)pattern[pos2];
      minimum += d*d;
    }
  assert(pattern_size>=200);
  for(phase = pattern_size/200; phase < pattern_size/4 ; phase += pattern_size/200)
    {
      long mismatch = 0;
      long position, pos1, pos2;
      for(position = 0; position < pattern_size && mismatch < minimum; position ++)
	{
	  pos1 = (position+phase)%pattern_size;
	  pos2 = position * song->pattern_size/pattern_size;
	  int d = (int)pattern[pos1]-(int)pattern[pos2];
	  mismatch += d*d;
	}
      if (minimum==-1 || mismatch<minimum)
	minimum = mismatch;
    }
  minimum/=pattern_size;
  minimum/=8;
  printf("mismatch = %ld\n",minimum);
  return minimum;
}
*/

QString Song::spectrum_between(Song* song, float percent)
{
  QString result="                        ";
  QString b = song->spectrum;
  for (int i = 0; i<24;i++)
    {
      char letter1 = spectrum.at(i).latin1();
      char letter2 = b.at(i).latin1();
      char letter3 = (letter1+letter2)/2;
      result.at(i)=letter3;
    }
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
  float sum = 0, d;
  if (measure->tempo)
    {
      d=tempo_distance(song);
      if (measure->tempo_weight)
	sum+=d*d;
      else sum += d>1 ? 1 : 0;
    }
  if (measure->spectrum)
    {
      d=spectrum_distance(song);
      sum+=d*d;
    }
  if (measure->pattern)
    {
      assert(0);
      // d=pattern_distance(song);
      // sum+=d*d;
    }
  return sum;
}

Point* Song::percentToward(Point * other, Metriek * dp, float percent)
{
  Song * song = (Song*)other;
  SongMetriek * measure = (SongMetriek*)dp;
  assert(other);
  assert(measure);
  Song * result = new Song();
  if (measure->tempo)
    result->tempo = tempo_between(song,percent);
  if (measure->spectrum)
    {
      result->spectrum = spectrum_between(song,percent);
      result->setColor(color_between(song,percent));
    }
  return result;
}

bool Song::getDistance()
{
  color_distance=255;
  if (!spectrum.isNull())
    if (ProcessManager::playingInMain())
      {
	float d = distance(ProcessManager::playingInMain(),&SongMetriek::SPECTRUM);
	d*=256;
	if (d<255) 
	  color_distance = (int)d;
      }
  if (color_distance==255)
    distance_string = QString::null;
  else
    {
      distance_string = tonumber(color_distance);
    }
  return color_distance<255;
}

Song::~Song()
{
}

/*
QPixmap *Song::getPixmap(int width, int height, const QColorGroup &cg)
{
  if (pattern_size==0) 
    return NULL;
  QPixmap *result = new QPixmap(width,height);
  QPainter p;
  p.begin(result);
  QRect r(QRect(0,0,width,height));
  Song* main = ProcessManager::playingInMain();
  if (main)
    {
      int d = (int)pattern_distance(main);
      if (d>255) d = 255;
      p.fillRect(r,QColor(255,d,d));
    }
  else
    p.fillRect(r,cg.base());
  p.setPen(cg.text());
  for(int i = 0 ; i < width ; i++)
    {
      int start = i * pattern_size / width;
      int stop = (i+1) * pattern_size / width;
      double value = 0;
      for(int j = start ; j < stop ; j ++)
	value+=(double)pattern[j];
      value/=(double)(stop-start);
      value*=height-1;
      value/=255.0;
      p.drawPoint(i,height-1-(int)value);
    }
  p.end();
  return result;
}
*/


/*QPixmap *Song::getPixmap(int width, int height, const QColorGroup &cg)
{
  if (pattern_size==0) 
    return NULL;
  QPixmap *result = new QPixmap(width,height);
  QPainter p;
  p.begin(result);
  unsigned char* other_pattern = NULL;
  Song* main = ProcessManager::playingInMain();
  if (main)
    other_pattern=main->pattern;
  for(int d = 0, i = 0 ; i < width ; i++)
    {
      int start = i * pattern_size / width;
      int stop = (i+1) * pattern_size / width;
      double value = 0;
      
      for(int j = start ; j < stop ; j ++)
	value+=(double)pattern[j];
      value/=(double)(stop-start);
      
      if (other_pattern)
	{
	  double value2=0;
	  start = i * main->pattern_size / width;
	  stop = (i+1) * main->pattern_size / width;
	  for(int j = start ; j < stop ; j ++)
	    value2+=(double)other_pattern[j];
	  value2/=(double)(stop-start);
	  d = (int)fabs(value2-value);
	  value = d;
	  d *= d;
	  d /= 256;
	}
      value*=height-1;
      value/=255.0;
      p.setPen(QColor(255,255-d,255-d));
      p.drawLine(i,0,i,height-1);
      p.setPen(cg.text());
      p.drawPoint(i,height-1-(int)value);
    }
  p.end();
  return result;
}
*/

QString Song::getDisplayTitle()
{
  QString result  = "";
  if (!title.isNull())
    result+=title;
  if (!author.isNull())
    result+=QString("[")+author+QString("]");
  return result;
}
