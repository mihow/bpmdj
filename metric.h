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
#ifndef __loaded__metric_h__
#define __loaded__metric_h__
using namespace std;
#line 1 "metric.h++"
class Metriek
{
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
#endif // __loaded__metric_h__
