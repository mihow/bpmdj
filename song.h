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

#ifndef SONG_H
#define SONG_H
#include <qstring.h>
#include <time.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "config.h"
#include "cluster.h"
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

QString tonumber(const int b);

class SongMetriek:
  public Metriek
{
  public: 
    // which properties needs to be taken into account
    bool tempo;
    bool pattern;
    bool tempo_weight;
    bool spectrum;  
    // standard metrics
    static SongMetriek SPECTRUM;
    static SongMetriek TEMPO;
    static SongMetriek PATTERN;
    static SongMetriek ALL;
    static SongMetriek ALL_WITHOUT_TEMPO_WEIGHT;
    // constructor
    SongMetriek(bool t, bool t2, bool s, bool p)
      {
	tempo = t;
	tempo_weight = t2;
	spectrum = s;
	pattern  = p;
      };
};

class Song: public Point
{
  public:
    // stored in the index files
    QString title;
    QString author;
    QString version;
    QString tempo;
    QString storedin;
    QString tags;
    QString file;
    QString time;
    QString md5sum;
    QString spectrum;
    // calculated as necessary
    QColor  color;
    QString spectrum_string;
    QString distance_string;
    AlbumField ** albums;
    int     color_distance;
    bool    played;
    bool    ondisk;
    int     has_cues;
    int     played_author_at_time;
  private:
    void init(const QString fullname, bool checkondisk);
    void clearFloatingFields();
  public:
    Song();
    Song(Index* idx, bool allow_write, bool checkondisk, bool account_spectrum);
    // WVB -- need to add delete operation here...
    void refill(Index &read);
    void reread();
    void realize();
    void checkondisk();
    void setColor(QColor c);
    bool getDistance();
    bool containsTag(const QString which);
    QString tempo_between(Song*, float);
    float   tempo_distance(Song* song);
    QString spectrum_between(Song*, float);
    QColor  color_between(Song* song, float percent);
    float   spectrum_distance(Song* song);
    virtual float   distance(Point* point, Metriek* dp);
    virtual Point* percentToward(Point * other, Metriek * dp, float percent);
    virtual void simpledump(int d);
    virtual void determine_color(float hue, float, int, int);
    virtual ~Song();
    bool modifiedOnDisk();
};

#endif
