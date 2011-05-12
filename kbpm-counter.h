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
#include "bpmcounter.h"

class BpmCountDialog : public CountDialog
{
   Q_OBJECT
 private:
   unsigned char *audio;
   unsigned long audiosize;
   unsigned long audiorate;
   unsigned long startbpm, stopbpm;
   unsigned long startshift, stopshift;
   long     int  bufsiz;
            int  reading_progress;
            int  processing_progress;
            bool stop_signal;
   unsigned long phasefit(long i);
   int tapcount;
   int starttime;
 public:
   volatile bool working;
   void          setPercentBounds(long startper, long stopper);
   void          setBpmBounds(long start, long stop);
   void          getMd5();     // retrieves MD5 sum
   void          writeAudio(); // writes audio to disk
   void          readAudio();  // reads the file in memory
   void          doit(); // does everything at the moment... not good
   void          doitwrapper();
   void          rangeCheck();
   void          removeRaw();
   void          stopWorking();
   SongPlayer *  player;
   BpmCountDialog(SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
 public slots:
   virtual void startAutomaticCounter();
   virtual void timerTick();
   
   // call this if you want to stop the current process
   virtual void finish();
   
   // user interface responses
   virtual void tap();
   virtual void reset();
   virtual void incBpm();
   virtual void decBpm();
   virtual void about();
   virtual void fetchSpectrum();
};
