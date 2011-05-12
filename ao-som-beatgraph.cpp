/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__ao_som_beatgraph_cpp__
#define __loaded__ao_som_beatgraph_cpp__
using namespace std;
#line 1 "ao-som-beatgraph.c++"
#include "memory.h"
#include "player-core.h"
#include "ao-som-beatgraph.h"
#include <qimage.h>
#include "clock-drivers.h"

/**
 * This function is used to find the best matching unit, so we skip the sqrt
 */
bool min_dist(signed1* A, signed1* B, float8&mindist, int size)
{
  unsigned4 curdist=0;
  for(int i = 0 ; i < size && curdist<mindist; i++)
    curdist+=abs((signed2)A[i] - (signed2)B[i]);
  if (curdist<mindist)
    {
      mindist=curdist;
      return true;
    }
  return false;
}

int cdist(int a, int b, int colors)
{
  int d = b-a;
  if (d<0) d=-d;
  if (d>=colors/2) return colors-1-d;
  return d;
}

int gauss(int pos, int zeroat)
{
  if (pos<0) return gauss(-pos,zeroat);
  if (pos>=zeroat) return 0;
  // return (int)(cos((3.141592/2.0)*(float4)pos/(float4)zeroat)*255.0);
  return 255-pos*255/zeroat;
}

void merge(signed1* target, signed1* source, signed4 strength,unsigned4 dimensions)
// strength is an integer from 0 to 255*255.
// when strength is maximal, we simply overwrite target, otherwise we dont
{
  if (strength==0) return;
  assert(strength<=255*255);
  if (strength==255*255) memcpy(target,source,dimensions);
  else
    {
      for(unsigned4 z = 0 ; z < dimensions; z++)
	{
	  signed8 a = target[z]*(255*255-strength);
	  signed8 b = source[z]*strength;
	  target[z]=(a+b)/(255*255);
	}
    }
}

#define BEATS
#define SAMPLED_INIT

elementResult ActiveSomBeatGraph::start()
{
  // calculate the proper size of each dimension
  colors = 90;
  if (!normalperiod.valid()) return Done;
  unsigned4 size = wave_max();
#ifdef BEATS
  unsigned4 period = normalperiod/4; // we want to look at the beats, not the measures 
#endif
#ifdef MEASURES
  unsigned4 period = normalperiod; // we want to look at the measures
#endif
  vectors = size/period;
  if (vectors*period>size) vectors--;
  if (vectors<1) return Done;
  dimensions = period / 4; // 11025 Hz is quite fine insterad of 44100
  
  //---------------------------
  // Allocation of the mapping & vectors
  printf("This song has %d beats (each beat is a vector we need to place)\n"
	 "and %d dimensions per vector (these should be reduced to a color)\n"
	 "and we will be using a color scale of %d values\n"
	 "In total we will need to allocate %d kB for our neural network\n"
	 "and %d kB for our input vectors\n",
	 vectors, dimensions, colors, colors*dimensions/1024,vectors*dimensions/1024);
  mapping=bpmdj_allocate(colors,signed1*);
  newmapping=bpmdj_allocate(colors,signed4*);
  newmappingcount=bpmdj_allocate(colors,unsigned2);
  newmappingstrength=bpmdj_allocate(colors,unsigned4);
  points=bpmdj_allocate(vectors,signed1*);
  for(int c = 0 ; c < colors; c++)
    {
      mapping[c]=bpmdj_allocate(dimensions,signed1);
      newmapping[c]=bpmdj_allocate(dimensions,signed4);
      newmappingcount[c]=0;
      newmappingstrength[c]=0;
    }
  for(unsigned4 v = 0 ; v < vectors; v++)
    points[v]=bpmdj_allocate(dimensions,signed1);
  pointcolors=bpmdj_allocate(vectors,unsigned1);
  
  //---------------------------
  // Initialisation with random values
#ifndef SAMPLED_INIT
  for(int c = 0 ; c < colors; c++)
    for(unsigned4 z = 0 ; z < dimensions ; z++)
      mapping[c][z]=random()%256-128;
#endif
  
  //-------------------------------
  // Read all the vectors in memory
  FILE * raw=openCoreRawFile();
  if (!raw) return Done;
  for(unsigned4 v = 0 ; v < vectors ; v++)
    {
#ifdef BEATS
      unsigned4 pos = v*normalperiod; // this includes all the convertions (44.1 -> 11.025 kHz, stereo->mono, 16b->8bit)
      // and cancels nicely out to the normalperiod. If we use the understandable arithmetic we ewnd up with 
      // slightly different positions.
#endif
#ifdef MEASURES
      unsigned4 pos = v*normalperiod*4; // this includes all the convertions (44.1 -> 11.025 kHz, stereo->mono, 16b->8bit)
#endif
      // printf("%d - %ld\n",v,v*4*4*dimensions);
      assert(pos<size*2*2);
      fseek(raw,pos,SEEK_SET);
      signed4 bufsize=dimensions*4;  // 4 because of the downsampling of 4 (which did not happen yet)
      stereo_sample2 buffer[bufsize];
      long count=readsamples(buffer,bufsize,raw);
      assert(count==bufsize);
      for(unsigned4 k = 0 ; k < dimensions; k++)
	{
	  int i=k*4;
	  assert(i<bufsize);
	  signed4 sum=0;
	  for(int j = 0 ; j < 4 ; j++)
	    {
	      sum+=buffer[i+j].left;
	      sum+=buffer[i+j].right;
	    }
	  sum /= 256; // 16bit to 8bit
	  sum /= 8;   // left right for 4 consecutive bytes
	  points[v][k]=sum;
	}
    }
  fclose(raw);

  //---------------------------
  // Initialization with the averages of a number of input points
#ifdef SAMPLED_INIT
  for(int c = 0 ; c < colors; c++)
    {
      unsigned4 v1 = c * vectors / colors;
      if (v1>=vectors) v1=vectors-1;
      unsigned4 v2 = (c+1) * vectors / colors;
      if (v2>=vectors) v2=vectors-1;
      unsigned4 count = v2-v1+1;
      for(unsigned4 z = 0 ; z < dimensions ; z++)
	{
	  signed8 total = 0;
	  for(unsigned4 v = v1; v <= v2 ; v++)
	    total+=points[v][z];
	  total/=count;
	  mapping[c][z]=total;
	}
    }
#endif
  
  // the target map must be signed and be able to accept a total of vectors vectors.
  // we also want some detail like 
  // 0-255 for the strength
  // 0-255 for the distance
  // vectors is the potential amount of additions
  // 0-255 for the value we actually have
  // 
  //---------------------------
  // Run an epoch
  int width = colors/2;
  int lrate = 255;
  for(int epoch = 0 ; epoch < 20 ; epoch++)
    {
      int pos = (19-epoch)*100/19;
      width = colors*pos/(2*100);
      lrate = 255*pos/100;

      printf("Epoch %d (width=%d, rate=%d)\n",epoch,width,lrate);
      // clear the target map
      for(unsigned4 p = 0 ; p < colors; p++)
	{
	  signed4* nm = newmapping[p];
	  for(unsigned4 q = 0 ; q < dimensions ; q++)
	    nm[q]=0;
	  newmappingcount[p]=0;
	  newmappingstrength[p]=0;
	}
      // create a new target map
      for(unsigned4 v = 0 ; v < vectors ; v++)
	{
	  signed1 *V = points[v];
	  // A- find the best matching neuron
	  float8 mindist=dimensions*256.0*256.0;
	  int bmu=-1;
	  for(int c = 0 ; c < colors ; c++)
	    if (min_dist(V,mapping[c],mindist,dimensions))
	      bmu=c;
	  assert(bmu>=0);
	  pointcolors[v]=bmu;
	  // B- update the map according to the current learning rate and distance
	  if (lrate)
	    for(int c = 0 ; c < colors ; c++)
	      {
		int d=cdist(c,bmu,colors);
		int s1=gauss(d,width);
		if (s1)
		  {
		    int s2=lrate;
		    // instead of merging it into the existing map, we create a new target map
		    // merge(mapping[c],V,s1*s2,dimensions);
		    int strength = s1*s2; // maximum of 255*255
		    signed4* nm = newmapping[c];
		    for(unsigned4 z = 0 ; z < dimensions ; z++)
		      nm[z]+=V[z]*strength;
		    newmappingcount[c]++;
		    newmappingstrength[c]+=strength;
		  }
	      }
	}
      // join the target map with the old map and replace the old map
#ifdef CHECK_BOUNDARIES
      typedef signed8 sumtype;
#else
      typedef signed4 sumtype;
#endif
      for(int c = 0 ; c < colors ; c++)
	{
	  if (!newmappingcount[c]) continue;
	  sumtype maxll = 255*255*(unsigned8)newmappingcount[c];
	  assert(maxll<127*255*255*255);
	  sumtype sumst = newmappingstrength[c]; // the sum of all the strengths together
	  for(unsigned4 z = 0 ; z < dimensions ; z++)
	    {
	      sumtype A = mapping[c][z];  // the original mapping
	      A*=(maxll-sumst);           // weighted properly
	      A+=newmapping[c][z];        // summed with the already weighted new mapping
#ifdef CHECK_BOUNDARIES
	      assert(A<127L*256L*256L*256L);
#endif
	      A/=maxll;                   // divided by the total weight
	      mapping[c][z]=A;
	    }
	}
      // C - dump the map to show the current layout
      float8 total_residue=0;
#ifdef BEATS
      QImage im(vectors/4,400,QImage::Format_RGB32);
      int ch = 100;
      for(unsigned4 x = 0 ; x < vectors/4; x++)
	for(int b = 0 ; b < 4; b++)
	  {
	    int v = b+x*4;
#endif
#ifdef MEASURES
    QImage im(vectors,400,QImage::Format_RGB32);
    int ch = 400;
    for(unsigned4 x = 0 ; x < vectors; x++)
	{
	  int b = 0;
	  int v = x;
#endif
	  signed1 *V = points[v];
	  // A- find the best matching unit
	  float8 mindist=dimensions*256.0*256.0;
	  int bmu=-1;
	  for(int c = 0 ; c < colors ; c++)
	    if (min_dist(V,mapping[c],mindist,dimensions))
	      bmu=c;
	  assert(bmu>=0);
	  total_residue+=mindist;
	  for(int y = 0 ; y < ch ; y++)
	    {
	      int z = ch*b+y;
	      int u = y*(dimensions-1)/ch;
	      int h = bmu*359/colors;
	      int i = abs(points[v][u])*2;
	      QColor c;
	      c.setHsv(h,255,i);
	      ((QRgb*)im.scanLine(z))[x]=c.rgb();
	    }
	}
      printf("  residue = %g per beat\n",total_residue/vectors);
      char tmp[100];
      sprintf(tmp,"som%d.png",100+epoch);
      im.save(tmp,"PNG");
    }
  return Done;
};

#endif // __loaded__ao_som_beatgraph_cpp__
