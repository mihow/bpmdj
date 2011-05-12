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
#ifndef __loaded__cluster_cpp__
#define __loaded__cluster_cpp__
using namespace std;
#line 1 "cluster.c++"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "clusterer.h"
#include "cluster.h"
#include "user-notification.h"
#include "memory.h"
#include "info.h"

/**
 * The old algorithm was fast but a mess. The one below is more readable but
 * slower. A potential optimalization is to store the distances in a speciifc 
 * object itself and use a form of point wrapper. Then it alkso becomes 
 * possible to store all pointpairs in a list according to their intra 
 * distance. That makes it possible to directly find the shortes distance.
 */

/*****************************************************************
 * Distances between points and couples
 *****************************************************************/
static map<Point*, map<Point*, float4> > matrix;
float4 distance_memory(Point* x, Point* y)
{
  assert(x<y);
  assert(matrix.find(x)!=matrix.end());
  map<Point*, float>& m2=matrix[x];
  assert(m2.find(y)!=m2.end());
  return m2[y];
};

static float4 distance_calculate(Point* x, Point* y, Metriek * metriek)
{
  if (x==y) return 0;
  assert(y>=x);
  bool cx = x->isCouple();
  bool cy = y->isCouple();
  float4 result;
  Couple* X = (Couple*)(x);
  Couple* Y = (Couple*)(y);
  if (cx && cy)
    result = X->distance(Y, metriek);
  else if (!cx && cy)
    result = Y -> distance2point(x, metriek);
  else if (cx && !cy)
    result = X->distance2point(y, metriek);
  else if (!cx && !cy)
    result = x->distance(y, metriek);
  matrix[x][y]=result;
  return result;
}

static float4 distance(Point* x, Point* y, Metriek * metriek)
{
  if (x>y)
    {
      Point* tidx = x;
      x = y;
      y = tidx;
    }
  if (matrix.find(x)==matrix.end())
    return distance_calculate(x,y,metriek);
  map<Point*, float>& m2=matrix[x];
  if (m2.find(y)==m2.end())
    return distance_calculate(x,y,metriek);
  return m2[y];
}

float4 Couple::distance2point(Point* idx, Metriek * metriek)
{
  float4 max = ::distance(first,idx,metriek);
  float4 d = ::distance(second,idx,metriek);
  if (d>max)
    return d;
  else
    return max;
}

float4 Couple::distance(Couple * other, Metriek * metriek)
{
  float4 max = ::distance(first,other->first, metriek);
  float4 d = ::distance(first,other->second, metriek);
  if (d>max) 
    max=d;
  d = ::distance(second,other->first, metriek);
  if (d>max) 
    max=d;
  d = ::distance(second,other->second, metriek);
  if (d>max) 
    return d;
  else
    return max;
}

/*****************************************************************
 * Cluster algorithm
 *****************************************************************/
Couple::Couple(Point* a, Point* b) : Point()
{
  assert(a);
  assert(b);
  first = a;
  second = b;
}

void Couple::determine_color(float4 hue_min, float4 hue_max, int depth, 
			     int stopat)
{
  if (depth>stopat)
    {
      first->determine_color(hue_min, hue_max, depth+1, stopat);
      second->determine_color(hue_min, hue_max, depth+1, stopat);
    }
  else
    {
      first->determine_color(
	hue_min + (hue_max-hue_min)*0.0/3.0,
	hue_min + (hue_max-hue_min)*1.0/3.0,
	depth+1, stopat);
      second->determine_color(
        hue_min + (hue_max-hue_min)*2.0/3.0,
	hue_min + (hue_max-hue_min)*3.0/3.0,
	depth+1,
	stopat);
    }
}

Couple *ActiveClusterer::agglomerate_allsteps(Metriek * metriek)
{
  Debug("%d points to cluster",tocluster.size());
  if (tocluster.size()<2)
    Error(true,"Cannot cluster a set with %d elements",tocluster.size());
  Couple *C=NULL;
  while(tocluster.size()>1)
    {
      /**
       * Find the two elements closest to each other
       */
      float minsofar=0;;
      Point* A=NULL;
      Point* B=NULL;
      for(set<Point*>::iterator a=tocluster.begin(); a!=tocluster.end(); a++)
	for(set<Point*>::iterator b=tocluster.begin(); b!=tocluster.end(); b++)
	  {
	    if (*a>=*b) continue;
	    float f=distance(*a,*b,metriek);
	    if (A==NULL || B==NULL|| f<minsofar)
	      {
		minsofar=f;
		A=*a;
		B=*b;
	      }
	  }
      /**
       * Join these two elements into one couple and
       * remove the two input points from the set
       */
      C=new Couple(A,B);
      tocluster.erase(A);
      tocluster.erase(B);
      tocluster.insert(C);
    }
  return C;
}

Clusterer clusterer;

elementResult ActiveClusterer::reset()
{
  Debug("Resetting points to cluster and distances");
  tocluster.clear();
  return Done;
}

elementResult ActiveClusterer::add(Point* p)
{
  tocluster.insert(p);
  return Done;
}

/**
 * This message will currently not chunk the calculation
 * in smaller parts. It will just keep on running until
 * everything has finished
 */
elementResult ActiveClusterer::agglomerate(SongMetriek metriek)
{
  // this takes the longest time
  Couple *result = agglomerate_allsteps(&metriek);
  // updating the colors of the songs
  result->determine_color((float4)0,(float4)360,0,
			  Config::get_color_cluster_depth() - 1);
  // notify the user interface to redraw
  UserNotification* un=new UserNotification();
  un->updateProcessView(true);
  un->send();
  return Done;
}

elementResult ActiveClusterer::terminate()
{
  // Debug("Deleting similarity matrix");
  for(map<Point*, map<Point*, float4> >::iterator a=matrix.begin();
      a!=matrix.end(); a++)
    a->second.clear();
  matrix.clear();
  return deactivate();
}
#endif // __loaded__cluster_cpp__
