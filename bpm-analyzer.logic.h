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

#include "analyzer.h"
#include "songplayer.h"
#include "bpmcounter.h"

typedef double fft_type;

class BpmAnalyzerDialog : public CountDialog, ThreadedAnalyzer
{
   Q_OBJECT
 private:
   // fine scanning
   unsigned char * audio;
   unsigned long   audiosize;
   unsigned long   audiorate;
   unsigned long   startbpm, stopbpm;
   unsigned long   startshift, stopshift;
   long     int    bufsiz;
	    // progress indicator fields
            int    reading_progress;
            int    processing_progress;
	    // tapping fields
            int    tapcount;
            int    starttime;
	    // fft fields
        fft_type * freq;
	fft_type * peak_bpm;
	fft_type * peak_energy;
	    int  * peak_fit;
 	    int    windowsize;
	    int    peaks;
   void          set_measured_period(QString technique, int period);
	    // Enveloppe fft's
   void          enveloppe_spectrum();
   void          autocorrelate_spectrum();
	    // FFT guidance routines
   void          fft();        // do a quick fft to obtain a set of 'hints'
   void          fft_draw(QPainter &p, int xs, int ys, int shifter, double bpm_divisor);
   void          autocorrelate_draw(QPainter &p, int xs, int ys, int shifter);
            // finding the error fit of a curve
   unsigned long phasefit_mult(unsigned long i);
   unsigned long phasefit_diff(unsigned long i);
   unsigned long phasefit(unsigned long i);
   unsigned long phasefit(unsigned long i, unsigned long clip);
   void          rayshoot_scan();
   void          peak_scan();   // scan based on fft-peaks
   void          readAudio();  // reads the file in memory
   void          readAudioBlock(int blocksize);  // reads the file in memory divided by blocks
 public:
   void          setPercentBounds(long startper, long stopper);
   void          setBpmBounds(long start, long stop);
   void          getMd5();     // retrieves MD5 sum
   void          writeAudio(); // writes audio to disk
   void          run();
   void          rangeCheck();
   void          removeRaw();
   SongPlayer *  player;
   BpmAnalyzerDialog(SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
 public slots:
   virtual void startStopButton();
   virtual void startAnalyzer();
   virtual void stopAnalyzer();
   virtual void stoppedAnalyzing();
   virtual void timerTick();
 
   // call this if you want to stop the current process
   virtual void finish();
   virtual void store();
 
   // user interface responses
   virtual void tap();
   virtual void reset();
   virtual void incBpm();
   virtual void decBpm();
};
