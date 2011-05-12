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

#include "songplayer.h"
#include "rythm-analyzer.h"
#include "analyzer.h"
#include "Data/types.h"

class RythmDialogLogic : public RythmDialog, public ReentrantAnalyzer
{
   Q_OBJECT
 public:
   RythmDialogLogic(SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
 public slots:
   virtual void calculateRythmPattern2();
   virtual void run() { calculateRythmPattern2();};
   virtual void calculate() { startAnalyzer(); };
};

void write_out_projection(long slice_size, unsigned4 *phases, int maximum_slice, const char * target);
