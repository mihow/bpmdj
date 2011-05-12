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
//#include <qlistview.h>
#include "qvectorview.h"
#include "config.h"
#include "cluster.h"
#include "index.h"

//#define SONGLIST_SET 0

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
 public:
    // stored in the index files
    QString title;
    QString author;
    QString version;
    QString storedin;
    QString file;
    QString time;
    QString md5sum;
    tempo_type    tempo;
    spectrum_type spectrum;
    tags_type     tags;
    
    // calculated as necessary
    QColor  color;
    QString spectrum_string;  // the spectrum column text
    QString distance_string;  // the dcolor column text
    float   color_distance;   // the dcolor distance value
    AlbumField ** albums;     
    bool    played;
    bool    ondisk;
    int     has_cues;
    int     played_author_at_time;
  private:
    void init(const QString fullname, bool checkondisk);
    void clearFloatingFields();
  public: // accessors
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
