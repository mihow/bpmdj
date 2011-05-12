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

#include <qcolor.h>
#include "qvectorview.h"
#include "config.h"
#include "cluster.h"
#include "song.h"
#include "data.h"

#define LIST_TEMPO 0
#define LIST_DCOLOR 2
#define LIST_TITLE 5
#define LIST_AUTHOR 6
#define LIST_TIME 3
#define LIST_CUES 4
#define LIST_VERSION 7
#define LIST_TAGS 8
#define LIST_ONDISK 9
#define LIST_SPECTRUM 1
#define LIST_INDEX 10
#define LIST_MD5SUM 11
#define LIST_FILE 12
#define LIST_MIN 13
#define LIST_MAX 14
#define LIST_MEAN 15
#define LIST_POWER 16
#define LIST_HISTOGRAM 17
#define LIST_RYTHM 18
#define LIST_COMPOSITION 19

class QSong: public QVectorViewData
{
    // accessors
    singleton_array_accessors(bool*, bool, selected);
    singleton_array_accessors(Song **, Song *, songs);
    singleton_accessors(int,compare_col);
    singleton_accessors(bool,compare_asc);
    singleton_accessors(int,song_count);

   public:
    static void setVector(Song** arr, int cnt);
    // functions required by the vectorview
    virtual int vectorSize() const;
    virtual bool isSelected(int i) const { assert(i>=0); return get_selected(i); };
    virtual void setSelected(int i, bool val=true) {get_selected()[i]=val;};
    virtual void sort(int col, bool ascending);
    virtual void paintCell(QVectorView* vv, int i, QPainter *p, const QColorGroup &cg, int col, int wid, int align);
    virtual QString text(int j, int i) const { return Text(get_songs(j),i); };
    
    static  void Sort();
    QSong();
    static Song * songEssence(int i);
    static QColor * colorOfTempoCol(const Song* main, Song* song);
    static QColor * colorOfAuthorCol(Song * song);
    static QColor * colorOfPlaycount(Song * song);
    static QColor * QSong::colorOfdColorCol(Song* song);
    static void playedAuthorAtTime(int i, int t) {get_songs(i)->set_played_author_at_time(t);};
    static QString Text(Song * j, int i);
};
