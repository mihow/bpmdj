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

#include "songplayer.h"
#include "tempolineanalyzer.h"
#include "common.h"

class TempoLineAnalyzerLogic : public TempoLineDialog
{
  Q_OBJECT
public:
  long audiosize;
  long audiorate;
  longtrick * data;
  TempoLineAnalyzerLogic(SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
public slots:
  virtual void showTempoLine_1();
  //- normal correlation between 80 and 180
  //- maximum/minimum search per image
  //- rescaled tot 0 -> 1 en kwadraat genomen
  //- maxima per row aangeduid
  virtual void showTempoLine_2();
  //- absolute correlatie 
  //- between 80 en 180
  //- maximum/minimum search per row
  //- rescaled tot 0 -> 1 en kwadraat genomen
  //- maxima per row aangeduid
  virtual void showTempoLine_3();
  //- abs waarde van afstand tussen absolute values
  //- between 80 en 180
  //- maximum/minimum search per image
  //- rescaled tot 0 -> 1, geen kwadraat genomen
  //- maxima per row aangeduid
  virtual void showTempoLine_4();
  //- som (abs waarde van afstand tussen absolute values in het kwadraat)
  //- between 80 en 180
  //- maximum/minimum search per column
  //- rescaled tot 0 -> 1, geen kwadraat genomen
  //- maxima per row aangeduid
  virtual void showCorrelationMatrix1();
  virtual void showCorrelationMatrix2();
  virtual void showTempoLine() {showCorrelationMatrix2();};
  virtual void balanceChanged();
};
