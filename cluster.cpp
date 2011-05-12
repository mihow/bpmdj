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
#ifndef __loaded__cluster_cpp__
#define __loaded__cluster_cpp__
using namespace std;
#line 1 "cluster.c++"
// modify: move similarity out of distance...
// compare not based on index buyt on pointer value of this. This should help somewhat
// removal of elements from the list can be done through reverse mapping elements

// TODO -- memorycheck is toch wel eens nodig Vooral bij het releasen van couple's
// TODO -- static classe is niet zo fijn, zou ze liever wat meer lokaal hebben.
// TODO -- eens kijken hoeveel maal de memoized functionality effectief 
//         gebruikt wordt. Indien niet, ze der uit gooien.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cluster.h"
#include "memory.h"

float    ** Cluster::similarity = NULL;
ClusterPosition ** Cluster::next;
ClusterPosition ** Cluster::prev;
Point    ** Cluster::realcontent;
int         Cluster::type_mark;
int         Cluster::realcontentsize=0;
int         Cluster::realcontenttotalsize=0;
int         Cluster::totalsize=0;
int *       Cluster::pointstocluster;
int         Cluster::size;

// Abstract definition of points
Point::Point()
{
}

void Point::prefix(int d)
{
  while(d-->0)
    printf(" . ");
}

// Couples, which make up the cluster hierarchy
Couple::Couple(int a, int b) : Point()
{
  first = a;
  second = b;
}

float Couple::distance2point(int idx, Metriek * metriek)
{
  // the distance from this cluster to another point is determined
  float max = Cluster::distance(first,idx,metriek);
  float d = Cluster::distance(second,idx,metriek);
  if (d>max)
    return d;
  else
    return max;
}

float Couple::distance(Couple * other, Metriek * metriek)
{
  float max = Cluster::distance(first,other->first, metriek);
  float d = Cluster::distance(first,other->second, metriek);
  if (d>max) 
    max=d;
  d = Cluster::distance(second,other->first, metriek);
  if (d>max) 
    max=d;
  d = Cluster::distance(second,other->second, metriek);
  if (d>max) 
    return d;
  else
    return max;
}

void Couple::simpledump(int d)
{
  prefix(d);
  printf("%d\n",d);
  Cluster::realcontent[first]->simpledump(d+1);
  Cluster::realcontent[second]->simpledump(d+2);
}

void Couple::determine_color(float hue_min, float hue_max, int depth, int stopat)
{
  if (depth>stopat)
    {
      Cluster::realcontent[first]->determine_color(hue_min, hue_max, depth+1, stopat);
      Cluster::realcontent[second]->determine_color(hue_min, hue_max, depth+1, stopat);
    }
  else
    {
      Cluster::realcontent[first]->determine_color(hue_min + (hue_max-hue_min)*0.0/3.0,
						   hue_min + (hue_max-hue_min)*1.0/3.0,
						   depth+1, stopat);
      Cluster::realcontent[second]->determine_color(hue_min + (hue_max-hue_min)*2.0/3.0,
						    hue_min + (hue_max-hue_min)*3.0/3.0,
						    depth+1,
						    stopat);
    }
}

int Couple::cluster_elements()
{
  return Cluster::realcontent[first]->cluster_elements() + 
    Cluster::realcontent[second]->cluster_elements();
}

int Couple::clusters_with_size(int min_size, int max_size, float &min_internal_distance, float& max_internal_distance)
{
  int nr = cluster_elements();
  if (nr<min_size) return 0;
  if (nr>=min_size && nr<=max_size) 
    {
      float d = Cluster::distance_memory(first,second);
      if (d<min_internal_distance || min_internal_distance<0)
	min_internal_distance = d;
      if (d>max_internal_distance || max_internal_distance<0)
	max_internal_distance = d;
      return 1;
    }
  return 
    Cluster::realcontent[first]->clusters_with_size(min_size,max_size,min_internal_distance,max_internal_distance)+
    Cluster::realcontent[second]->clusters_with_size(min_size,max_size,min_internal_distance,max_internal_distance);
}

void Couple::color_sub_elements(int a, int b, float d)
{
  Cluster::realcontent[first]->color_sub_elements(a,b,d);
  Cluster::realcontent[second]->color_sub_elements(a,b,d);
}

void Couple::color_clusters_with_size(int min_size, int max_size)
{
  float min_distance = -1;
  float max_distance = -1;
  int nr_of_such_clusters = clusters_with_size(min_size,max_size,min_distance,max_distance);
  if (nr_of_such_clusters==0) 
    color_sub_elements(-1,-1,0);
  else
    color_clusters_with_size(min_size,max_size,0,nr_of_such_clusters,min_distance,max_distance);
}

int Couple::color_clusters_with_size(int min_size, int max_size, int cluster_nr, int nr_of_such_clusters, float min_dist, float max_dist)
{
  int nr = cluster_elements();
  // if this cluster falls in range then we simply color it entirily and return the next cluster number
  if (nr>=min_size && nr<=max_size)
    {
      color_sub_elements(cluster_nr,nr_of_such_clusters,
			 (Cluster::distance_memory(first,second) - min_dist)
			 / (max_dist-min_dist));
      return cluster_nr+1;
    }
  // if there are too few elements, then we also avoid coloring it
  if (nr<min_size)
    {
      color_sub_elements(-1,-1,0);
      return cluster_nr;
    }
  // if there are too many elements, then we go into the two sub trees
  cluster_nr = Cluster::realcontent[first]->color_clusters_with_size(min_size,max_size,cluster_nr,nr_of_such_clusters,min_dist,max_dist);
  return Cluster::realcontent[second]->color_clusters_with_size(min_size,max_size,cluster_nr,nr_of_such_clusters,min_dist,max_dist);
}

float Couple::intra_distance()
{
  return Cluster::distance_memory(first,second);
}

void Couple::color_clusters_dw()
{
  color_clusters_dw(0.0,240.0,Cluster::distance_memory(first,second),Cluster::distance_memory(first,second));
}

void Couple::color_clusters_dw(float hue_min, float hue_max, float max_dist, float last_dist)
{
  float dc = intra_distance(); // this detmines the hue afterward
  if (dc == 0) 
    {
      Point::color_clusters_dw(hue_min,hue_max,max_dist,last_dist);
      return;
    }
  float da = Cluster::realcontent[first]->intra_distance();
  float db = Cluster::realcontent[second]->intra_distance();
  // depending on the different distances, another color is given
  float tc;
  if (da+db>dc)
    tc = da+db;
  else 
    tc=dc;
  float hal = hue_min;
  float har = hue_min+(hue_max-hue_min)*da/tc;
  float hbl = hue_min+(hue_max-hue_min)*(tc-db)/tc;
  float hbr = hue_max;
  Cluster::realcontent[first]->color_clusters_dw(hal,har,max_dist,dc);
  Cluster::realcontent[second]->color_clusters_dw(hbl,hbr,max_dist,dc);
}

void Point::color_clusters_dw(float hue_min, float hue_max, float max_dist, float last_dist)
{
  color_sub_elements((int)hue_min,360,last_dist/max_dist);
}


int Couple::get_min_maxdepth(int &min, int &max, int depth)
{
  return 
    Cluster::realcontent[first]->get_min_maxdepth(min,max,depth+1)+
    Cluster::realcontent[second]->get_min_maxdepth(min,max,depth+1);
}

int Point::get_min_maxdepth(int &min, int &max,int depth)
{
  if (depth>max || max<0) max = depth;
  if (depth<min || min<0) min = depth;
  return 1;
}

void Couple::color_clusters_dw2()
{
  // get nr of elements
  // get minimum depth
  // get maximum depth
  int a  = -1;
  int b  = -1;
  get_min_maxdepth(a,b,0);
  printf("minimum depth = %d, maximum depth = %d\n",a,b);
  color_clusters_dw2(0.0,240.0,a,b,0);
}


void Couple::color_clusters_dw2(float hue_min, float hue_max, int min_depth, int max_depth, int depth)
{
  // int nr = cluster_elements();
  int a = Cluster::realcontent[first]->cluster_elements();
  int b = Cluster::realcontent[second]->cluster_elements();
  float h = hue_min+(hue_max-hue_min)*(float)a/(float)(a+b);
  Cluster::realcontent[first]->color_clusters_dw2(hue_min,h,min_depth,max_depth,depth+1);
  Cluster::realcontent[second]->color_clusters_dw2(h,hue_max,min_depth,max_depth,depth+1);
}

void Point::color_clusters_dw2(float hue_min, float hue_max, int min_depth, int max_depth, int depth)
{
  float d = ((float)(max_depth-depth)/(float)(1+max_depth-min_depth));
  assert(d>=0.0 && d <=1.0);
  color_sub_elements((int)hue_min,360,d);
}



// Cluster main class
Cluster::Cluster()
{
  totalsize=2;
  size=0;
  pointstocluster=bpmdj_allocate(2,int);
}

void Cluster::add(int t)
{
  size++;
  if(size>totalsize)
    {
      totalsize*=2;
      pointstocluster=bpmdj_reallocate(pointstocluster,totalsize,int);
    }
  pointstocluster[size-1]=t;
}

float Cluster::distance_calculate(int xidx, int yidx, Metriek * metriek)
{
  assert(xidx>yidx);
  Point* x = realcontent[xidx];
  Point* y = realcontent[yidx];
  float result = x->distance(y, metriek);
  similarity[xidx][yidx]=result;
  return result;
}

float Cluster::distance_memory(int x, int y)
{
  if (x<y)
    {
      int t = x;
      x = y;
      y = t;
    }
  float result = similarity[x][y];
  assert(result>=0);
  return result;
}

float Cluster::distance(int xidx, int yidx, Metriek * metriek)
{
  // assert(xidx!=yidx);
  // assert(xidx<realcontentsize);
  // assert(yidx<realcontentsize);
  // the xidx should be the largest
  if (xidx<yidx)
    {
      int tidx = xidx;
      xidx = yidx;
      yidx = tidx;
    }
  // is there a memory available 
  float result;
  result = similarity[xidx][yidx];
  if (result>=0)
    return result;
  // no memory found, so we need to calculate the distance
  // obtain the data
  bool tx = isPoint(xidx);
  bool ty = isPoint(yidx);
  Point* x = realcontent[xidx];
  Point* y = realcontent[yidx];
  // compare
  if (!tx && !ty)
    {
      Couple* X = (Couple*)(x);
      Couple* Y = (Couple*)(y);
      result = X->distance(Y, metriek);
    }
  else if (tx && !ty)
    {
      Couple* Y = (Couple*)(y);
      result = Y -> distance2point(xidx, metriek);
    }
  else if (!tx && ty)
    {
      Couple* X = (Couple*)(x);
      result = X->distance2point(yidx, metriek);
    }
  else if (tx && ty)
    {
      result = x->distance(y, metriek);
    }
  // store comparison
  similarity[xidx][yidx]=result;
  //  printf("Distance between %d and %d = %g\n",x,y,result);
  return result;
}

inline bool Cluster::isPoint(int idx)
{
  return idx<type_mark;
}

void Cluster::dumpConnectionMatrix()
{
  int **matrix = bpmdj_allocate(realcontentsize,int*);
  for(int i=0;i<realcontentsize;i++)
    {
      matrix[i]=bpmdj_allocate(realcontentsize,int);
      for (int j = 0 ; j < realcontentsize ; j++)
	matrix[i][j]=0;
    }
  
  ClusterPosition c = next[0][0];
  ClusterPosition n;
  while(true)
    {
      matrix[c.x][c.y]++;
      n = next[c.x][c.y];
      if (n.x==0 && n.y==0) 
	break;
      c=n;
    }
  
  while(true)
    {
      matrix[c.x][c.y]++;
      n=prev[c.x][c.y];
      if (n.x==0 && n.y==0) 
	break;
      c=n;
    }
  
  for(int i = 0 ; i <realcontentsize; i++)
    {
      printf("%d ",i);
      for(int j = 0 ; j <realcontentsize; j++)
	{
	  int d = matrix[i][j];
	  if (d==0)
	    printf(".");
	  else if (d==2)
	    printf("#");
	  else 
	    printf("*");
	}
      bpmdj_deallocate(matrix[i]);
      printf("\n");
    }
  printf("====================================\n");
}

// het probleem in de originele agglomerate function is dat telkens opnieuw het minimum gezocht moet worden
// om dit tegen te gaan, werken we nu met een lijst waar de bsetaande minima in staan, tesamen met hun posities
// 1. kreeer de similarity matrix...
// 2. sorteer deze in een eigen reeks
// 3. link ze aan elkaar...
// 4. wanneer een cluster gevormd wordt kijk welk van de twee maten genomen wordt en herlink 
//    de gelinkte data. Diegene die er uit gegooid wordt, kan maar beter gereset worden
// 5. zo doen we verder tot de ganse lijst gerelinked is

// dus als we nu distance and positie wegmoffelen in de positie..

int compareposition(const void* a, const void *b)
{
  ClusterPosition* A = (ClusterPosition*)a;
  ClusterPosition* B = (ClusterPosition*)b;
  if (A->distance<B->distance)
    return -1;
  else if (A->distance==B->distance)
    return 0;
  else 
    return 1;
}

Couple *Cluster::agglomerate(Metriek * metriek)
{
  int x, y;
  type_mark = realcontentsize;
  // 1. create initial set of comparisons.
  int entriestosort = ((size-1)*(size))/2;
  int entry=0;
  ClusterPosition *entries = bpmdj_allocate(entriestosort,ClusterPosition);
  assert(entries);
  printf("%d entries to create...\n",entriestosort);
  
  for (x = 1 ; x < size; x ++)
    for (y = 0; y < x ; y ++)
      {
	entries[entry].distance=distance_calculate(x,y,metriek);
	entries[entry].x=x;
	entries[entry].y=y;
	entry++;
      }
  
  printf("Similarity set created...(%d)\nBegin sorting\n",entry);
  // 2. sort the entries
  qsort(entries,entriestosort,sizeof(struct ClusterPosition),compareposition);
  printf("Entries sorted...\n");
  // 3. now we must link them all together... 
   ClusterPosition p;
   p.x=0;
   p.y=0;
   p.distance=0;
   ClusterPosition c;
   c.x=0;
   c.y=0;
   c.distance=0;
   for (int i = 0 ; i <entriestosort; i++)
     {
       c = entries[i];
       // make the current point to the previous
       prev[c.x][c.y]=p;
       // make the previous point to the current;
       next[p.x][p.y]=c;
       // the previous becomes the current
       p=c;
     }
   prev[0][0]=c;
   next[c.x][c.y].x=0;
   next[c.x][c.y].y=0;
   bpmdj_deallocate(entries);
   printf("Prev/next filled with correct data...\n");
   //dumpConnectionMatrix();
   printf("Begin agglomerating elements...\n");
   // 4. As long as there is data in the list, traverse it
   while(size>1)
    {
      // a. check stop condition
      ClusterPosition first;
      first = next[0][0];
      // first.x = next[0][0].x;
      // first.y = next[0][0].y;
      assert(first.x > 0 || first.y > 0);
      ClusterPosition second;
      second.x = next[first.x][first.y].x;
      second.y = next[first.x][first.y].y;
      second.distance = next[first.x][first.y].distance;
      //if (!first.distance<=second.distance)
      //{
      //printf("first.distance = %g\n second.distance = %g\n",first.distance,second.distance);
      //fflush(stdout);
      //assert(0);
      //}
      // b. merge the first
      Couple* n  = new Couple(x=first.x,y=first.y);
      // printf("Merging %d and %d, distance = %g\n",first.x,first.y,similarity[first.x][first.y]);
      int z =addcouple(n);
      
      // nu moeten we de beide elementen verwijderen uit de content set. 
      for( int i = 0 ; i < size ; i ++ )
	{
	  if (pointstocluster[i] == first.x)
	     {
	       pointstocluster[i] = z;
	       break;
	     }
	}
      for(int i = 0 ; i < size ; i ++)
	{
	  if ( pointstocluster [ i ] == first . y )
	    {
	      pointstocluster [ i ] = pointstocluster [ --size  ] ;
	      break;
	    }
	}
      // c. fix the linked list stuff...
      // remove first element
      next[0][0]=second;
      prev[second.x][second.y].x=0;
      prev[second.x][second.y].y=0;
      next[first.x][first.y].x=-1;
      next[first.x][first.y].y=-1;
      prev[first.x][first.y].x=-1;
      prev[first.x][first.y].y=-1;
      // create new distances and reuse existing positions
      for (int j = 0 ; j < size; j++)
	{
	  int i = pointstocluster[j];
	  if (i==z) continue;
	  float d = distance(z,i,metriek);
	  // nu bepalen we welke posities deruit gaat uit de gesorteerde lijst. 
	  // en welke verhuist naar een andere locatie (z,i)
	  ClusterPosition relocate;
	  ClusterPosition remove;
	  relocate.y=i;
	  remove.y=i;
	  // this is tricky, because x is not necessarily larger than i
	  if ( (x>i && d==similarity[x][i])
	    || (i>x && d==similarity[i][x]))
	    {
	      relocate.x=x;
	      remove.x=y;
	    }
	  else
	    {
	      relocate.x=y;
	      remove.x=x;
	    }
	  if (relocate.x<relocate.y)
	    {
	      int t=relocate.x;
	      relocate.x=relocate.y;
	      relocate.y=t;
	    }
	  if (remove.x<remove.y)
	    {
	      int t=remove.x;
	      remove.x=remove.y;
	      remove.y=t;
	    }
	  
	  // print out some info
	  //	  printf("  %d,%d  relocate(%d,%d)  remove(%d,%d)  z=%g\n",z,i,relocate.x,relocate.y,remove.x,remove.y,d);
	  
	  // some quick checks
	  // fix list to remove (remove.x,remove.y)
	  ClusterPosition p;
	  ClusterPosition n;
	  p=prev[remove.x][remove.y];
	  if (p.x==-1)
	    printf("removal of %d,%d",remove.x,remove.y);
	  assert(p.x>=0);
	  n=next[remove.x][remove.y];
	  assert(n.x>=0);
	  assert(n.y>=0);
	  next[p.x][p.y]=n;
	  prev[n.x][n.y]=p;
	  // bookkeeping
	  prev[remove.x][remove.y].x=-1;
	  prev[remove.x][remove.y].y=-1;
	  next[remove.x][remove.y].x=-1;
	  next[remove.x][remove.y].y=-1;
	  // fix list to relocate
	  p=prev[relocate.x][relocate.y];
	  if (p.x==-1)
	    printf("%d,%d\n",relocate.x,relocate.y);
	  assert(p.x>=0);
	  assert(p.y>=0);
	  n=next[relocate.x][relocate.y];
	  assert(n.x>=0);
	  assert(n.y>=0);
	  next[p.x][p.y].x=z;
	  next[p.x][p.y].y=i;
	  prev[n.x][n.y].x=z;
	  prev[n.x][n.y].y=i;
	  prev[z][i]=p;
	  next[z][i]=n;
	}
    }
  // return valuye
  return (Couple*)realcontent[pointstocluster[0]];
}

void Cluster::reset()
{
  realcontenttotalsize = 1;
  realcontent=bpmdj_allocate(realcontenttotalsize,Point*);
  realcontentsize=0;
  similarity=bpmdj_allocate(realcontenttotalsize,float*);
  // the previous and next matrices
  prev = bpmdj_allocate(realcontenttotalsize,ClusterPosition*);
  next = bpmdj_allocate(realcontenttotalsize,ClusterPosition*);
}

int Cluster::addcontent(Point* p)
{
  // update content
  if (realcontentsize>=realcontenttotalsize)
    {
      realcontenttotalsize*=2;
      realcontent = bpmdj_reallocate(realcontent,realcontenttotalsize, Point*);
      similarity  = bpmdj_reallocate(similarity, realcontenttotalsize, float*);
      prev        = bpmdj_reallocate(prev,       realcontenttotalsize, ClusterPosition *);
      next        = bpmdj_reallocate(next,       realcontenttotalsize, ClusterPosition *);
    }
  int id = realcontentsize++;
  realcontent[id] = p;
  // update similarity thingies
  if (id==0)
    {
      similarity[id]=NULL;
      prev[id]=bpmdj_allocate(1,ClusterPosition);
      next[id]=bpmdj_allocate(1,ClusterPosition);
      prev[id][0].x=-1;
      prev[id][0].y=-1;
      next[id][0].x=-1;
      next[id][0].y=-1;
    }
  else
    {
      similarity[id]=bpmdj_allocate(id,float);
      prev[id]=bpmdj_allocate(id,ClusterPosition);
      next[id]=bpmdj_allocate(id,ClusterPosition);
      for(int j = 0 ; j < id ; j++)
	{
	  similarity[id][j]=-1;
	  prev[id][j].x=-1;
	  next[id][j].x=-1;
	  prev[id][j].y=-1;
	  next[id][j].y=-1;
	}
    }
  return id;
}

int Cluster::addpoint(Point*p)
{
  return addcontent(p);
}

void Cluster::addPoint(Point*p)
{
  add(addpoint(p));
}

int Cluster::addcouple(Couple * c)
{
  return addcontent(c);
}

/**
 * A testing point to illustrate how points in the cluster should behave. See the old_main 
 * routine to test the clustering algorithm. At the moment a run with 1000 random points
 * takes 3.830 time to complete. 2000 points take 17.760 time. 3000 points take 44.060 time.
 * This is of course highly dependent on the time the distance metric takes and the processor
 * At the time of this measurement it was a pentium class machine.
 */
class TestPoint:
  public Point
{
  int x;
public:
  TestPoint(int p): Point()
  {
    x=p;
  }
  virtual float distance(Point* other)
  {
    int d = ((TestPoint*)other) -> x - x;
    if (d>=0) return d;
    else return -d;
  }
  virtual void determine_color(float hue_min, float hue_max, int depth, int stopat)
  {
    printf("%d   %g\n",x,hue_min);
  }
  virtual void simpledump(int d)
  {
    prefix(d);
    printf("INT %d\n",x);
  }
};

int old_main()
{
  Cluster *all = new Cluster();
  Cluster::reset();
  printf("Cluster Created\n");
  fflush(stdout);
  for(int i = 0; i<4000;i++)
    all->addPoint(new TestPoint(random()));
  printf("Data Created\n");
  fflush(stdout);
  all->agglomerate(NULL);
  printf("Agglomeration Done\n");
  fflush(stdout);
  // print out
  // all->simpledump(0);
  // all->determine_color(0,360,0,0);
  return 0;
}
#endif // __loaded__cluster_cpp__
