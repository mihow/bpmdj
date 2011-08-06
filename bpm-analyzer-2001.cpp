/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__bpm_analyzer_2001_cpp__
#define __loaded__bpm_analyzer_2001_cpp__
using namespace std;
#include <stdlib.h>
#include <math.h>
#include "bpm-analyzer-2001.h"
#include "memory.h"

BpmAnalyzer2001::BpmAnalyzer2001(stereo_sample2 *block, unsigned4 n, 
				 float8 lower_boundary, float8 higher_boundary): 
  BpmAnalyzerImpl(44100,true)
{
  input_audio=block;
  input_samples=n;
  startbpm=lower_boundary;
  stopbpm=higher_boundary;
}

void BpmAnalyzer2001::readAudioBlock(signed4 blocksize)
{
  audiosize=input_samples;  // uitgedrukt in samples
  audiosize /= blocksize;        // uitgedrukt in blokken
  audio = bpmdj_allocate(audiosize, unsigned1);
  status("Reading audio (block size %d)",blocksize);
  unsigned4 pos = 0;
  while((signed8)pos<(signed8)audiosize)
    {
      progress(pos,audiosize);
      unsigned4 total = 0;
      const stereo_sample2* buffer=input_audio+blocksize*pos;
      for (signed4 i = 0 ; i < blocksize ; i ++ )
	{
	  signed4 left, right;
	  left=abs(buffer[i].left);
	  right=abs(buffer[i].right);
	  total+=(left+right)/256;
	}
      audio[pos++]=total/blocksize;
    }  
}

unsigned4 BpmAnalyzer2001::phasefit(unsigned4 i)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  for(c=i;c<(unsigned4)audiosize;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

unsigned4 BpmAnalyzer2001::phasefit(unsigned4 i, unsigned4 clip)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  if (!clip) return phasefit(i);
  assert(audiosize>=0 && i<(unsigned4)audiosize);
  for(c=i;c<(unsigned4)audiosize && mismatch <= clip ;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

bool BpmAnalyzer2001::step()
{
  const signed4 blockshifter_max = 8;  
  signed4 blockshifter = blockshifter_max;
  signed* mismatch_array[blockshifter_max+1];
  unsigned4 mean[blockshifter_max+1];
  unsigned4 minimum_at = 0;
  unsigned4 minima[blockshifter_max+1];
  for (signed4 i = 0 ; i <= blockshifter_max; i++) mean[i]=0;
  unsigned4 maxima[blockshifter_max+1];
  signed4 blockshifter_min=2;
  // we assume audiorate == 44100
  stopshift = 44100 * 60 * 4 / startbpm ;
  startshift = 44100 * 60 * 4 / stopbpm ;
  for(signed4 i = 0; i <= blockshifter_max ; i ++)
    {
      mismatch_array[i] = bpmdj_allocate(stopshift-startshift, signed);
      for(unsigned4 j = 0 ; j < stopshift-startshift ; j ++)
	mismatch_array[i][j] = -1;
    }
  while(blockshifter>=blockshifter_min)
    {
      unsigned4 blocksize = 1 << blockshifter;
      // first read audio
      readAudioBlock(blocksize);
      // calculate all mismatches
      signed * mismatch = mismatch_array[blockshifter] - startshift;
      signed * prev_mismatch = NULL;
      unsigned4 prev_maximum = 0;
      if (blockshifter < blockshifter_max)
	{
	  prev_mismatch = mismatch_array[blockshifter+1] - startshift;
	  // the mismatch array goes always from 0 to stophift-startshift-1
	  // the prev_mismatch  hence goes from  startshift to stopshift - 1
	  prev_maximum = mean[blockshifter+1]; 
	}
      status("Scanning %d/%d",blockshifter_max-blockshifter,blockshifter_max-blockshifter_min);
      if (!prev_mismatch)
	for (unsigned4 i = startshift ; i < stopshift; i ++ )
	  {
	    unsigned4 phase = i >> blockshifter;
	    unsigned4 store = phase << blockshifter;
	    progress(i - startshift,stopshift - startshift);
	    if (store!=i) continue;
	    unsigned4 m = phasefit(phase);
	    mismatch[store]=m;
	  }
      else
	{
	  // first a dry run to see how many times we would execute the 
	  // phasefit;
	  signed4 phasefit_called = 0;
	  signed4 phasefit_total = 0;
	  for (unsigned4 i = startshift ; i < stopshift; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      if (store!=i) continue;
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		phasefit_total++;
	    }
	  if (!phasefit_total) phasefit_total = 1;
	  // now wet run..
	  for (unsigned4 i = startshift ; i < stopshift; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      progress(phasefit_called,phasefit_total);
	      if (store!=i) continue;
	      // we hebben een positie die een macht van 2 is...
	      // dus de vorige was op - en + blocksize *2
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		{
		  unsigned4 m = phasefit(phase);  
		  // we moeten de phase kleiner maken omdat de array
		  //  geresampled is op blocksize
		  mismatch[store]=m;
		  phasefit_called++;
		}
	    }
	}

      // find minimum, translate, maximum, mean
      signed minimum = mismatch[startshift];
      minimum_at = startshift;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (minimum == -1 || (mismatch[i] >= 0 && mismatch[i] < minimum))
	  minimum = mismatch[ minimum_at = i ];
      minima[blockshifter] = minimum_at;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0)
	  mismatch[i]-=minimum;
      signed maximum = -1;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0 && (mismatch [i] > maximum || maximum < 0)) 
	  maximum = mismatch[i];
      signed8 total = 0;
      signed4 count = 0;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i]>=0)
	  {
	    count++;
	    total+=mismatch[i];
	  }
      maxima[blockshifter]=maximum;
      if (!count) count = 1;
      mean[blockshifter]=total/count;

      if (plot)
	{
	  // unsigned4 pos = r.height() - ((signed8)mean[i] * (signed8) r.height()/(signed8)slice_maximum);     
	  int lineno=blockshifter_max-blockshifter;
	  char tmp[100];
	  sprintf(tmp,"Autodifference (ws=%d)",1<<blockshifter);
	  axes(lineno,horizontal(),axis("relative position"),tmp);
	  // to mark the tempo we choose
	  // float8 bpm = (float8)(4.0*60.0*44100.0)/(float8)minima[i];
	  // mark_point(line_no,minima[i],pos)
	  // pos = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  // p.drawLine(pos,0,pos,r.height());
	  for( unsigned4 j = startshift ; j < stopshift ; j ++ )
	    {
	      float8 y = mismatch_array[blockshifter][j-startshift];
	      if (y < 0) continue;
	      period_point(lineno,j,y);
	      // bpm = (float8)(4.0*60.0*44100.0)/(float8)j;
	    }
	}
      blockshifter --;
    }

  tempo=44100.*4*60/(double)minimum_at;
  status("Tempo is %g",tempo);
  finished();
  return false;
}

BpmAnalyzer2001::~BpmAnalyzer2001()
{
}
#endif // __loaded__bpm_analyzer_2001_cpp__
