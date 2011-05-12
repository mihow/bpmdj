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

#include <pthread.h>
#include <stdio.h>
#include "analyzer.h"

Analyzer::Analyzer()
{
}

ThreadedAnalyzer::ThreadedAnalyzer(): 
  Analyzer()
{
  working = false;
  stop_signal = false;
}

ReentrantAnalyzer::ReentrantAnalyzer(): 
  Analyzer()
{
  working = false;
}

void ThreadedAnalyzer::startStopAnalyzer()
{
  if (working)
    {
      stopAnalyzer();
    }
  else
    {
      startAnalyzer();
    }
}

void ThreadedAnalyzer::doit()
{
  run();
  working=false;
  stoppedAnalyzing();
}

void* doit(void* anal)
{
  ((ThreadedAnalyzer*)anal)->doit();
  return NULL;
}

void Analyzer::startAnalyzer()
{
  run();
  stoppedAnalyzing();
}

void ReentrantAnalyzer::startAnalyzer()
{
  if (working) return;
  working=true;
  Analyzer::startAnalyzer();
  working=false;
}

void ThreadedAnalyzer::startAnalyzer()
{
  if (working) return;
  working=true;
  stop_signal=false;
  pthread_t y;
  pthread_create(&y,NULL,::doit,(void*)this);
}


void Analyzer::stopAnalyzer()
{
}

void ThreadedAnalyzer::stopAnalyzer()
{
  if (!working) return;
  stop_signal=true;
  while(working) ;
}

void Analyzer::stoppedAnalyzing()
{
}

void Analyzer::finish()
{
  stopAnalyzer();
  store();
}

