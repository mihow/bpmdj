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

#include "config.h"
#include "cluster.h"

class QSong: 
  public QListViewItem,
  public Point
{
  public:
    QString song_title;
    QString song_author;
    QString song_version;
    QString song_tempo;
    QString song_index;
    QString song_tags;
    QString song_file;
    QString song_time;
    QString song_md5sum;
    QString song_spectrum;
    QColor  color;
    QString spectrum_string;
    QString distance_string;
    int     color_distance;
    bool    song_played;
    bool    song_ondisk;
    int     has_cues;
    int     played_author_at_time;
  private:
    static QString TRUE_TEXT;
    static QString FALSE_TEXT;
    void init(const QString filename, const QString currentpath);
  public:
    QSong(QString filename, QString currentpath, QListView* parent);
    bool obtainTitleAuthor(char * fulltitle);
    void reread();
    void setColor(QColor c);
    void invertColor(bool r, bool g, bool b);
    bool getDistance();
    bool containsTag(const QString which);
    virtual void paintCell(QPainter *p, const QColorGroup &cg, int col, int wid, int align);
    virtual QString text(int i) const;
    virtual float distance(Point* point);
    virtual void simpledump(int d);
    virtual void determine_color(float hue, float, int, int);
};
