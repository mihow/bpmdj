/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#ifndef SONG_H
#define SONG_H
#include <qstring.h>
#include <time.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "qvectorview.h"
#include "data.h"
#include "config.h"
#include "cluster.h"
#include "index.h"

class QSong;
QString tonumber(const int b);

class Song: public Point
{
  singleton_accessors(int, max_alltime_total);
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
  accessors(rythm_property,rythm,rythm_property());
  accessors(composition_property,composition,composition_property());
  // calculated as necessary
  accessors(QColor,color,QColor(127,127,127));
  accessors(QString, spectrum_string, "");
  accessors(QString, distance_string, QString::null);
  accessors(float,color_distance,0);
  accessors(AlbumField**,albums,NULL);
  accessors(bool,played,false);
  accessors(bool,ondisk,true);
  accessors(bool,ondisk_checked,false);
  accessors(int,has_cues,0);
  accessors(int,played_author_at_time,-100);
  accessors(int,alltime_playcount,0);
 private:
  void  init(const QString fullname, bool checkondisk);
  void  clearFloatingFields();
 public:
  QString  tempo_str();
 public:
  Song();
  Song(Index* idx, bool allow_write, bool checkondisk);
  // WVB -- need to add delete operation here...
  void refill(Index &read, bool allowed_to_write=false);
  void reread();
  bool has_all_cluster_fields();
  void checkondisk();
  void setColor(QColor c);
  QString getDisplayTitle();
  bool get_distance_to_main(float limit=1.0);
  bool contains_tag(const tag_type which);
  tempo_type tempo_between(Song*, float);
  bool    tempo_show(const Song* main, bool uprange, bool downrange);
  QColor  color_between(Song* song, float percent);
  virtual float distance(Point* point, Metriek * dp, double limit);
  virtual float distance(Point* point, Metriek* dp);
  virtual float distance(Song* a, float wa, Song* b, float wb, Metriek * dp);
 public:
  virtual void simpledump(int d);
  virtual void determine_color(float hue, float, int, int);
  virtual void color_sub_elements(int a, int b, float d);
  virtual ~Song();
  bool    modifiedOnDisk();

  bool needs_energy() const { return  !get_power().fully_defined(); };
  bool needs_tempo() const { return !get_tempo().valid(); };
  bool needs_spectrum() const { return get_spectrum()==no_spectrum; };
  bool needs_echo() const { return get_histogram().empty(); };
  bool needs_rythm() const { return get_rythm().empty(); };
  bool needs_composition() const { return get_composition().empty(); };
  bool needs_analysis();
};

#endif
