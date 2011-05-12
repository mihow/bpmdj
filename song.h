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

class SongMetriek:
  public Metriek
{
  public: 
    // which properties needs to be taken into account
    float tempo;
    float spectrum;
    SongMetriek(float tw, float sw)
      {
	tempo = tw;
	spectrum = sw;
      };
};

class Song: public Point
{
    // taken from the index file
    accessors(QString, title,"");
    accessors(QString, author,"");
    accessors(QString,version,"");
    accessors(QString,storedin,"");
    accessors(QString,file,"");
    accessors(QString,md5sum,"");
    accessors(QString,time,"");
    accessors(tempo_type,tempo,tempo_type());
    accessors(spectrum_type,spectrum,no_spectrum); 
    accessors(tags_type,tags,NULL);
    accessors(sample_type, max_amp,sample_type());
    accessors(sample_type,min_amp,sample_type());
    accessors(sample_type,mean_amp,sample_type());
    accessors(power_type,power,power_type());
    // calculated as necessary
    accessors(QColor,color,QColor(127,127,127));
    accessors(QString,spectrum_string,"");
    accessors(QString,distance_string,QString::null);
    accessors(float,color_distance,0);
    accessors(AlbumField**,albums,NULL);
    accessors(bool,played,false);
    accessors(bool,ondisk,true);
    accessors(int,has_cues,0);
    accessors(int,played_author_at_time,-100);
  private:
    void init(const QString fullname, bool checkondisk);
    void clearFloatingFields();
  public: 
    QString  tempo_str();
  public:
    Song();
    Song(Index* idx, bool allow_write, bool checkondisk, bool account_spectrum);
    // WVB -- need to add delete operation here...
    void refill(Index &read, bool allowed_to_write=false);
    void reread();
    void realize();
    void checkondisk();
    void setColor(QColor c);
    QString getDisplayTitle();
    bool getDistance();
    bool contains_tag(const tag_type which);
    tempo_type tempo_between(Song*, float);
    float   tempo_n_distance(float harmonic, Song* song); // without taking the abs value
    float   tempo_distance(float harmonic, Song* song);
    float   tempo_distance(Song* song);
    spectrum_type spectrum_between(Song*, float);
    QColor  color_between(Song* song, float percent);
    float   spectrum_distance(Song* song);
    virtual float   distance(Point* point, Metriek * dp);
    virtual Point* percentToward(Point * other, Metriek * dp, float percent);
    virtual void simpledump(int d);
    virtual void determine_color(float hue, float, int, int);
    virtual void color_sub_elements(int a, int b, float d);
    virtual ~Song();
    bool    modifiedOnDisk();
};

#endif
