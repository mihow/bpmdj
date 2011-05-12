/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__song_h__
#define __loaded__song_h__
using namespace std;
#line 1 "song.h++"
#include <qstring.h>
#include <time.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "data.h"
#include "config.h"
#include "cluster.h"
#include "index.h"

class QSong;

class Song: public Point
{
  singleton_accessors(signed4, max_alltime_total);
  // taken from the index file
  accessors(QString, title,"");
  accessors(QString, author,"");
  accessors(QString, version,"");
  accessors(QString, storedin,"");
  accessors(QString, file,"");
  accessors(QString, md5sum,"");
  accessors(QString, time,"");
  accessors(tempo_type,tempo,tempo_type());
  accessors(spectrum_type*,spectrum,no_spectrum); 
  accessors(tags_type,tags,NULL);
  accessors(sample4_type, max_amp,sample4_type());
  accessors(sample4_type,min_amp,sample4_type());
  accessors(sample4_type,mean_amp,sample4_type());
  accessors(power_type,power,power_type());
  accessors(echo_property,histogram,echo_property());
  accessors(rhythm_property,rhythm,rhythm_property());
  accessors(composition_property,composition,composition_property());
  // calculated as necessary
  accessors(QColor,color,QColor(127,127,127));
  accessors(QString, spectrum_string, "");
  accessors(QString, distance_string, QString::null);
  accessors(float4,color_distance,0);
  accessors(AlbumField**,albums,NULL);
  accessors(bool,played,false);
  accessors(bool,ondisk,true);
  accessors(bool,ondisk_checked,false);
  accessors(bool,has_cues,false);
  accessors(signed2,played_author_at_time,-100);
  accessors(unsigned4,alltime_playcount,0);
private:
  void init(const QString fullname, bool checkondisk);
  void clearFloatingFields();
 public:
  QString  tempo_str();
 public:
  Song();
  Song(Index* idx, bool allow_write, bool checkondisk);
  void refill(Index &read, bool allowed_to_write=false);
  void reread();
  bool has_all_cluster_fields();
  void checkondisk();
  void setColor(QColor c);
  QString getDisplayTitle();
  bool get_distance_to_main(float4 limit=1.0);
  bool contains_tag(const tag_type which);
  tempo_type tempo_between(Song*, float4);
  bool    tempo_show(const Song* main, bool uprange, bool downrange);
  QColor  color_between(Song* song, float4 percent);
  virtual float4 distance(Point* point, Metriek * dp, float8 limit);
  virtual float4 distance(Point* point, Metriek* dp);
  virtual float4 distance(Song* a, float4 wa, Song* b, float4 wb, Metriek * dp);
 public:
  virtual void simpledump(unsigned2 d);
  virtual void determine_color(float4 hue, float4, int, int);
  virtual void color_sub_elements(int a, int b, float4 d);
  virtual ~Song();
  bool    modifiedOnDisk();

  bool needs_energy() const { return  !get_power().fully_defined(); };
  bool needs_tempo() const { return !get_tempo().valid(); };
  bool needs_spectrum() const { return get_spectrum()==no_spectrum; };
  bool needs_echo() const { return get_histogram().empty(); };
  bool needs_rhythm() const { return get_rhythm().empty(); };
  bool needs_composition() const { return get_composition().empty(); };
  bool needs_analysis();
};
#endif // __loaded__song_h__
