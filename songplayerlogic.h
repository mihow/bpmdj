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

#include <qtimer.h>
#include "songplayer.h"

class SongPlayerLogic : public SongPlayer
{
   Q_OBJECT
 private:
   QTimer *timer;
   int tempo_fade;
   int fade_time;
   void setColor(QButton *button, bool enabled);
   void redrawCues();
 public:
   SongPlayerLogic::SongPlayerLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0);
 public slots:
   virtual void timerTick();
   virtual void nudgePlus();
   virtual void nudgeMinus();
   virtual void nudgePlus1M();
   virtual void nudgeMinus1M();
   virtual void nudgePlus4M();
   virtual void nudgeMinus4M();
   virtual void nudgePlus8M();
   virtual void nudgeMinus8M();
   virtual void accept();
   virtual void nudgePlusB();
   virtual void setCue();
   virtual void shiftBack();
   virtual void stop();
   virtual void retrieveZ();
   virtual void retrieveX();
   virtual void retrieveC();
   virtual void retrieveV();
   virtual void storeZ();
   virtual void storeX();
   virtual void storeC();
   virtual void storeV();
   virtual void targetTempo();
   virtual void normalTempo();
   virtual void fastSwitch();
   virtual void mediumSwitch();
   virtual void slowSwitch();
   virtual void targetStep();
   virtual void nudgeCueBack();
   virtual void nudgeCueForward();
   virtual void nudgeCueBack8M();
   virtual void nudgeCueForward8M();
};
