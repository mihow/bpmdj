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
#ifndef __loaded__fragment_cpp__
#define __loaded__fragment_cpp__
using namespace std;
#line 1 "fragment.c++"
#include "files.h"
#include "do-fragment.h"
#include "info.h"

FragmentInMemoryData::FragmentInMemoryData()
{
  samples=NULL;
  size=0;
};

FragmentInMemoryData::FragmentInMemoryData(QString filename)
{
  FILE * f = fopen(filename.toAscii().data(),"rb");
  if (!f)
    {
      Warning("Could not read fragment \"%s\"",
	      filename.toAscii().data());
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
