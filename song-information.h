/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __BPMDJ___SONG_INFORMATION_H__
#define __BPMDJ___SONG_INFORMATION_H__
using namespace std;
#line 1 "song-information.h++"
#include "capacity.h"
#include "ui-song-information.h"

class Index;
class SongInformation: public QWidget, public Ui::SongInformation
{
  Q_OBJECT
  Index * showing;
  capacity_type old_disabled_capacity;
public:
  SongInformation(QWidget * parent): QWidget(parent)
  {
    setupUi(this);
    showing=NULL;
  }
  void updateDataFrom(Index * where);
public slots:
 virtual void on_save_clicked();
};
#endif
