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

#ifndef ANALYZER_H
#define ANALYZER_H

class Analyzer
{
 public:
  Analyzer();
  virtual void startAnalyzer();
  // call this to start the analyzer
  virtual void stopAnalyzer();
  // call this to start the analyzer
  virtual void stoppedAnalyzing();
  // called when analyzer has been stopped
  virtual void finish();
  // call to finish the entire analyzing process
  virtual void run() = 0;
  // fill this in with appropriate logic
  virtual void store() {};
  // fill in with appropriate index store logic
};

class ThreadedAnalyzer: public Analyzer
{
 protected:
  volatile bool stop_signal;
  volatile bool working;
 public:
  void doit();
  void doitwrapper();
 public:
  ThreadedAnalyzer();
  virtual void startStopAnalyzer();
  // call this to start/stop analyzer depending on its current state
  virtual void startAnalyzer();
  // call this to start the analyzer
  virtual void stopAnalyzer();
  // call this to stop the analyzer

  virtual void run() = 0;
  // fill this in with appropriate logic
  virtual void store() = 0;
  // fill in with appropriate index store logic
};

#endif
