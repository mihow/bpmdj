/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__bos_cpp__
#define __loaded__bos_cpp__
using namespace std;
#line 1 "bos.c++"
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "overseer.h"

static int total_processes=0;
const int M=20;
const int T=10;

class TestProcess: public Process
{
public:
  int expect_return;
  TestProcess()
  {
    expect_return=random()%256;
    vcommand("sleep %ld; exit %ld",random()%20,expect_return);
    total_processes++;
    if (random()%2==0)
      exec();
    else
      spawn();
  }
  virtual void died(ExitStatus s)
  {
    total_processes--;
    assert(s.exit()==expect_return);
    if (total_processes==0) 
      ::exit(0);
    Process::died(s);
  }
};

void* process_creator(void * ignore)
{
  for(int i = 0 ; i < M; i++)
    new TestProcess();
  return NULL;
}

int main(int argc, char* argv[])
{
  for(int t=0;t<T;t++)
    {
      pthread_t *y=(pthread_t*)malloc(sizeof(pthread_t));
      pthread_create(y,NULL,process_creator,NULL);
    }
  PollDelay d(0,5000,250000);
  while(true)
    {
      if (poll_death()) 
	d.reset();
      d.wait();
    }
}
#endif // __loaded__bos_cpp__
