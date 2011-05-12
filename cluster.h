/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#ifndef CLUSTER_H
#define CLUSTER_H

#include <assert.h>
#include "common.h"
#include "basic-types.h"

// TODO: by taking all elements in realcontent immediatelly from
// Qsong global if the index is below type_mark 
class Metriek
{
 public:
  Metriek() {};
};

class Point
{
  public:
    Point();
    virtual float  distance(Point * other, Metriek * dp) {return 0;};
    virtual Point* percentToward(Point * other, Metriek * dp, float percent) {return NULL;};
    virtual void   simpledump(int d) {};
    virtual void   determine_color(float hue_min, float hue_max, int depth, int stopat) {};
    virtual int    cluster_elements() { return 1; };
    virtual int    clusters_with_size(int min_size, int max_size,float &min_internal_distance, float& max_internal_distance) {assert(0);};
    virtual int    color_clusters_with_size(int min, int max, int nr, int count, float min_dist, float max_dist) {assert(0);};
    virtual void   color_sub_elements(int, int, float) {assert(0);};
    virtual void   color_clusters_dw(float hue_min, float hue_max, float max_dist, float last_dist);
    virtual void   color_clusters_dw2(float hue_min, float hue_max, int min_depth, int max_depth, int depth);
    virtual int    get_min_maxdepth(int &min, int &max,int depth);
    virtual float  intra_distance() {return 0;};
    void prefix(int d);
};

struct Position
{
  float distance;
  signed2 x;
  signed2 y;
  bool operator == (Position & other)
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
  float distance2point(int idx, Metriek* metriek);
  virtual void simpledump(int d);
  virtual void determine_color(float hue_min, float hue_max, int depth, int stopat);
  virtual int  clusters_with_size(int min_size, int max_size,float &min_internal_distance, float& max_internal_distance);
  virtual int  cluster_elements();
  virtual void color_clusters_with_size(int min_size, int max_size);
  virtual int  color_clusters_with_size(int min, int max, int nr, int count, float min_dist, float max_dist);
  virtual void color_sub_elements(int, int, float);
  virtual void color_clusters_dw();
  virtual void color_clusters_dw(float hue_min, float hue_max, float max_dist, float last_dist);
  virtual float intra_distance();
  virtual int   get_min_maxdepth(int &min, int &max,int depth);
  virtual void color_clusters_dw2();
  virtual void color_clusters_dw2(float hue_min, float hue_max, int min_depth, int max_depth, int depth);
  float distance(Couple * other, Metriek * metriek);
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
    
    static float * * similarity;
    static Position * * prev;
    static Position * * next;
    // the similarity matrix
    Cluster();
    static float distance(int x, int y, Metriek * metriek);
    static float distance_calculate(int x, int y, Metriek * metriek);
    static float distance_memory(int x, int y);
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
#endif
