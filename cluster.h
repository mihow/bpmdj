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
#ifndef __loaded__cluster_h__
#define __loaded__cluster_h__
using namespace std;
#line 1 "cluster.h++"
#include <set>
#include <map>
#include <assert.h>
#include "common.h"

class Metriek
{
 public:
  Metriek() 
  {
  };
};

class Point
{
public:
  Point()
  {
  };
  virtual ~Point() 
  {
  };
  virtual bool isCouple()
  {
    return false;
  }
  /**
   * Calculates the distance between 2 points. Is virtual so
   * it can be redefined for songs etcetera.
   */
  virtual float4 distance(Point * other, Metriek * dp) 
  {
    return 0;
  };
  virtual void determine_color(float4 hue_min, float4 hue_max, 
			       int depth, int stopat) 
  {
  };
  virtual unsigned4 elements_count() 
  {
    return 1; 
  };
  virtual float4  intra_distance() 
  {
    return 0;
  };
};

/**
 * A couple is the aggregation of thwo subclusters or points. 
 * The average of those two points is the new location. A
 * second use of the couples is to remember the distance
 * between two points.
 */
struct Couple: public Point
{
  Point* first;
  Point* second;
  Couple(Point* a, Point* b);
  virtual ~Couple() 
  {
  };
  virtual bool isCouple()
  {
    return true;
  }
  float4 distance2point(Point* idx, Metriek* metriek);
  virtual void determine_color(float4 hue_min, float4 hue_max,
			       int depth, int stopat);
  virtual unsigned4 elements_count()
  {
    return first->elements_count() + 
      second->elements_count();
  };
  float4 distance(Couple * other, Metriek * metriek);
};

typedef set<Point*> PointSet;
#endif // __loaded__cluster_h__
