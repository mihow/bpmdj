/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include "common.h"
#include "spectrum-analyzer.h"
#include "analyzer.h"

class SpectrumDialogLogic : public SpectrumDialog, public ReentrantAnalyzer
{
   Q_OBJECT
 public:
   SpectrumDialogLogic(SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
 public slots:
   virtual void fetchSpectrum_old();
   virtual void fetchSpectrum_normal();
   virtual void run() { fetchSpectrum_normal();};
   virtual void fetchSpectrum() {startAnalyzer(); };
};
