/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__fragment_cpp__
#define __loaded__fragment_cpp__
using namespace std;
#line 1 "fragment.c++"
#include "files.h"
#include "do-fragment.h"

FragmentInMemoryData::FragmentInMemoryData()
{
  samples=NULL;
  size=0;
};

FragmentInMemoryData::FragmentInMemoryData(QString filename)
{
  FILE * f = fopen(filename.ascii(),"rb");
  if (!f)
    {
      printf("Could not read fragment \"%s\"\n",filename.ascii());
      size = 0;
      samples = NULL;
      return;
    }
  fseek(f,0,SEEK_END);
  size = ftell(f)/4;
  fseek(f,0,SEEK_SET);
  samples = bpmdj_allocate(size,stereo_sample2);
  size = readsamples(samples,size,f);
  fclose(f);
};

FragmentInMemoryData::~FragmentInMemoryData()
{
  bpmdj_deallocate(samples);
}

bool FragmentFileData::isEmpty() 
{
  return f.isEmpty();
};

FragmentFileData::FragmentFileData(): f(""), song(NULL)
{
};

FragmentFileData::FragmentFileData(Song *s, QString f): f(f), song(s)
{
};

FragmentInMemory FragmentFileData::getFragment()
{
  if(f.isEmpty())
    return FragmentInMemory();
  return FragmentInMemory(f);
};

FragmentFileData::~FragmentFileData()
{
  if (!f.isEmpty())
    {
      QByteArray ba = f.toAscii();
      remove(ba.data());
    }
};

#endif // __loaded__fragment_cpp__
