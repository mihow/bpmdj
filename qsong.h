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

#include <qlistview.h>
#include "config.h"
#include "cluster.h"
#include "song.h"

class QSong: 
  public QListViewItem
{
  private:
    Song * song;
    static QString TRUE_TEXT;
    static QString FALSE_TEXT;
  public:
    // Constructor
    QSong(Song * s, QListView* parent);
    // Accessor
    Song *  songEssence() const {return song;};
    QString tags() const {return song->tags;};
    QString md5sum() const {return song->md5sum;};
    QString spectrum() const {return song->spectrum;};
    QString index() const {return song->index;};
    QString title() const {return song->title;};
    QString author() const {return song->author;};
    bool    ondisk() const {return song->ondisk;};
    void    ondisk(bool b) const {song->ondisk=b;};
    bool    played() const {return song->played;};
    void    played(bool b) const {song->played=b;};
    QColor  color() const {return song->color;};
    QString file() const {return song->file;};
    QString tempo() const {return song->tempo;};
    void    playedAuthorAtTime(int t) const {song->played_author_at_time = t;};
    // Inherited
    void reread() 
      { song->reread(); };
    void invertColor(bool r, bool g, bool b) 
      { song->invertColor(r,g,b); };
    void setColor(QColor transfer) 
      { song->setColor(transfer); };
    bool getDistance() 
      { return song->getDistance(); };
    bool containsTag(const QString which) 
      { return song->containsTag(which); };
    virtual void paintCell(QPainter *p, const QColorGroup &cg, int col, int wid, int align);
    virtual QString text(int i) const;
};
