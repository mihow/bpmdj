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
#ifndef __loaded__cluster_h__
#define __loaded__cluster_h__
using namespace std;
#line 1 "cluster.h++"
#include <assert.h>
#include "common.h"

/**
 * TODO: by taking all elements in realcontent immediatelly from
 * Qsong global if the index is below type_mark 
 */
class Metriek
{
 public:
  Metriek() {};
};

class Point
{
public:
  Point();
  virtual ~Point() {};
  virtual float4 distance(Point * other, Metriek * dp) 
  {
    return 0;
  };
  virtual Point* percentToward(Point * other, Metriek * dp, float4 percent) 
  {
    return NULL;
  };
  virtual void simpledump(unsigned2 d) 
  {
  };
  virtual void determine_color(float4 hue_min, float4 hue_max, 
			       int depth, int stopat) 
  {
  };
  virtual int    cluster_elements() 
  {
    return 1; 
  };
  virtual int clusters_with_size(int min_size, int max_size,
				 float4 &min_internal_distance, 
				 float4& max_internal_distance) 
  {
    assert(0); 
    return 0;
  };
  virtual int color_clusters_with_size(int min, int max, int nr, int count, 
				       float4 min_dist, float4 max_dist) 
  {
    assert(0);  
    return 0;
  };
  virtual void color_sub_elements(int, int, float4) {assert(0);};
  virtual void color_clusters_dw(float4 hue_min, float4 hue_max, 
				   float4 max_dist, float4 last_dist);
  virtual void color_clusters_dw2(float4 hue_min, float4 hue_max, 
				  int min_depth, int max_depth, int depth);
  virtual int get_min_maxdepth(int &min, int &max,int depth);
  virtual float4  intra_distance() 
  {
    return 0;
  };
  void prefix(unsigned2 d);
};

struct ClusterPosition
{
  float4 distance;
  signed2 x;
  signed2 y;
  bool operator == (ClusterPosition & other)
  {
    return x == other.x && y == other.y;
  }
};

class Couple: public Point
{
  int first;
  int second;
public:
  Couple(int a, int b);
  virtual ~Couple() {};
  float4 distance2point(int idx, Metriek* metriek);
  virtual void simpledump(unsigned2 d);
  virtual void determine_color(float4 hue_min, float4 hue_max, 
			       int depth, int stopat);
  virtual int  clusters_with_size(int min_size, int max_size,
				  float4 &min_internal_distance, 
				  float4& max_internal_distance);
  virtual int  cluster_elements();
  virtual void color_clusters_with_size(int min_size, int max_size);
  virtual int  color_clusters_with_size(int min, int max, int nr, int count, 
					float4 min_dist, float4 max_dist);
  virtual void color_sub_elements(int, int, float4);
  virtual void color_clusters_dw();
  virtual void color_clusters_dw(float4 hue_min, float4 hue_max, 
				 float4 max_dist, float4 last_dist);
  virtual float4 intra_distance();
  virtual int   get_min_maxdepth(int &min, int &max,int depth);
  virtual void color_clusters_dw2();
  virtual void color_clusters_dw2(float4 hue_min, float4 hue_max, 
				  int min_depth, int max_depth, int depth);
  float4 distance(Couple * other, Metriek * metriek);
};

class Cluster
{
public:
  // all points and clusters available
  static Point * * realcontent;
  static int       realcontentsize;
  static int       realcontenttotalsize;
  static int       type_mark;
  // the similarity matrix
  // an array of indices which are part of this cluster
  static int     * pointstocluster;
  static int       size;
  static int       totalsize;
  static inline bool isPoint(int xidx);
  
  static float4 * * similarity;
  static ClusterPosition * * prev;
  static ClusterPosition * * next;
  // the similarity matrix
  Cluster();
  static float4 distance(int x, int y, Metriek * metriek);
  static float4 distance_calculate(int x, int y, Metriek * metriek);
  static float4 distance_memory(int x, int y);
  // adds an index or a point to this cluster
  void add(int idx);
  void addPoint(Point* p);
  static void reset();
  // adds globally a new point, without inserting it in any cluster
  static int addcontent(Point * p);
  static int addpoint(Point * p);
  static int addcouple(Couple* c);
  Couple* agglomerate(Metriek * metriek);
  void dumpConnectionMatrix();
};
#endif // __loaded__cluster_h__
