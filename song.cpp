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
#include "kbpm-played.h"
#include "dirscanner.h"
#include "spectrum.h"
#include "index.h"

#define LIST_DCOLOR 2
#define LIST_TITLE 5
#define LIST_AUTHOR 6
#define LIST_TEMPO 0
#define LIST_TIME 3
#define LIST_CUES 4
#define LIST_VERSION 7
#define LIST_TAGS 8
#define LIST_ONDISK 9
#define LIST_SPECTRUM 1
#define LIST_INDEX 10
#define LIST_MD5SUM 11
#define LIST_FILE 12


//float SongMetriek::tempo_scale = 0.06; 
//float SongMetriek::spectrum_scale = 1.0/512.0;
SongMetriek SongMetriek::SPECTRUM(false,false,true,false);
SongMetriek SongMetriek::TEMPO(true,true,false,false);
SongMetriek SongMetriek::ALL(true,true,true,false);
SongMetriek SongMetriek::ALL_WITHOUT_TEMPO_WEIGHT(true,false,true,false);
SongMetriek SongMetriek::PATTERN(false,false,false,true);

void Song::reread(bool checkfileonline)
{
  Index::read((const char*)index);
  /* when the index changes immediately (new version and so on)
   * write it out again
   */
  if (Index::index->index_changed)
    Index::index->write_idx();
  /* read last modification time */
  struct stat status;
  if (stat((const char*)index,&status)==0)
    modification_time = status.st_mtime;
  /* copy everything to object */
  tempo = Index::index->get_tempo_str();
  file = Index::index->index_file;
  tags = Index::index->index_tags;
  time = Index::index->index_time;
  md5sum = Index::index->index_md5sum;
  spectrum = Index::index->index_spectrum;
  title = Index::index->get_display_title();
  author = Index::index->get_display_author();
  version = Index::index->get_display_version();
  // pattern_size = Index::index->index_pattern_size;
  // pattern = NULL;
  // if (pattern_size > 0)
  //{
  //pattern = allocate(pattern_size,unsigned char);
  // memcpy(pattern,Index::index->index_pattern,pattern_size);
  //}
  /* are there any cues stored */
  has_cues = Index::index->index_cue_z + Index::index->index_cue_x + Index::index->index_cue_c + Index::index->index_cue_v;
  /* free all */
  delete Index::index;
  /* try to open the song */
  if (checkfileonline)
    checkondisk();
}

void Song::checkondisk()
{
  QString songfilename = MusicDir + "/" + file;
  ondisk = DirectoryScanner::exists(songfilename);
}

void Song::init(const QString filename, const QString currentpath, bool checkondisk) 
{
  char *fulltitle;
  int len = filename.length();
  ondisk = false;
  /* check for .idx at the end */
  assert(filename.contains(".idx"));
  /* full index file */
  index = currentpath+"/"+filename;
  /* retrieve fullname without .idx */
  fulltitle=strdup(filename);
  assert(fulltitle);
  fulltitle[len-4]=0;
  free(fulltitle);
  /* read the index file */
  reread(checkondisk);
  /* set played flag */
  played=Played::IsPlayed(index);
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
  index = "";
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
  modification_time = 0;
}

Song::Song(QString a, QString b, bool checkondisk)
{
  init(a,b, checkondisk);
  newSpectrum(spectrum);
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

void Song::invertColor(bool r, bool g, bool b)
{
  QColor c;
  c.setRgb(r ? 255-color.red(): color.red(),
	   g ? 255-color.green() : color.green(),
	   b ? 255-color.blue() : color.blue());
  setColor(c);
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
void Song::toStream(QDataStream & stream)
{
  stream << title;
  stream << author;
  stream << version;
  stream << tempo;
  stream << index;
  stream << tags;
  stream << file;
  stream << time;
  stream << md5sum;
  stream << spectrum;
  stream << color;
  stream << has_cues;
  stream << modification_time;
}

void Song::fromStream(QDataStream & stream)
{
  stream >> title;
  stream >> author;
  stream >> version;
  stream >> tempo;
  stream >> index;
  stream >> tags;
  stream >> file;
  stream >> time;
  stream >> md5sum;
  stream >> spectrum;
  stream >> color;
  stream >> has_cues;
  stream >> modification_time;

  newSpectrum(spectrum);
}
*/

bool Song::modifiedOnDisk()
{
  time_t new_modification_time;
  if (modification_time == 0)
    return true;
  struct stat status;
  if (stat((const char*)index,&status)!=0)
    return true;
  new_modification_time = status.st_mtime;
  return new_modification_time!=modification_time;
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
