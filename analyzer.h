/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__analyzer_h__
#define __loaded__analyzer_h__
using namespace std;
#line 1 "analyzer.h++"
class Analyzer
{
protected:
  // keeps track of the current running state
  volatile bool working;
  // true when the process should voluntarily stop
  volatile bool stop_signal;
public:
  Analyzer(): working(false), stop_signal(false) {};
  // call to start the analyzer
  virtual void start(bool own_thread = false);
  // call to stop the analyzer. The method will wait until the 
  // analyzer has stopped.
  virtual void stop();
  // call this to start/stop analyzer depending on its current state
  virtual void toggle();
  // called when analyzer has been started
  virtual void started();
  // called to analyze the process, should be filled in 
  // with some appropriate logic
  virtual void analyze() = 0;
  // called when analyzer has been stopped
  virtual void stopped() { working = false; };
  // a method that will call started(), analyze() and stopped()
  virtual void run();
  virtual ~Analyzer() { stop(); };
};
#endif // __loaded__analyzer_h__
