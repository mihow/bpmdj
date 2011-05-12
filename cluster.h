/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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

extern "C" {
#include "common.h"
}

class Metriek
{
 public:
  Metriek() {};
};

class Point
{
  public:
    Point();
    virtual float distance(Point * other, Metriek * dp) {return 0;};
    virtual Point* percentToward(Point * other, Metriek * dp, float percent) {return NULL;};
    virtual void simpledump(int d) {};
    virtual void determine_color(float hue_min, float hue_max, int depth, int stopat) {}
    void prefix(int d);
};

struct Position
{
  float distance;
  signed2 x;
  signed2 y;
  bool operator == (Position & other)
  {
    return x == other.x &&
      y == other.y;
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
