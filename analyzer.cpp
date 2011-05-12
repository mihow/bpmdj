/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__analyzer_cpp__
#define __loaded__analyzer_cpp__
using namespace std;
#line 1 "analyzer.c++"
#include <pthread.h>
#include <stdio.h>
#include "analyzer.h"

void Analyzer::run()
{
  if (working) return;
  started();
  analyze();
  stopped();
}

void Analyzer::started()
{
  working = true;
  stop_signal=false;
};

void Analyzer::toggle()
{
  if (working)
    stop();
  else
    start();
}

void Analyzer::stop()
{
  if (!working) return;
  stop_signal=true;
  while(working) ;
}

void* doit(void* anal)
{
  ((Analyzer*)anal)->run();
  return NULL;
}

void Analyzer::start(bool own_thread)  
{
  if (own_thread)
    {
      if (working) return;
      pthread_t y;
      pthread_create(&y,NULL,::doit,(void*)this);
    }
  else
    run(); 
}
#endif // __loaded__analyzer_cpp__
