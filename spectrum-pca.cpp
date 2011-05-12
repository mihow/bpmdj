/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__spectrum_pca_cpp__
#define __loaded__spectrum_pca_cpp__
using namespace std;
#line 1 "spectrum-pca.c++"
#include <cstdio>
#include "vector-iterator.h"
#include "pca.h"
#include "spectrum-pca.h"
#include "user-notification.h"

elementResult ActiveSpectrumPca::pcaThis(vector<Song*> *songs)
{
  UserNotification * un = new UserNotification();
  float4 minx=0,miny=0,minz=0;
  float4 maxx=0,maxy=0,maxz=0;
  int count = 0;
  float4 ** data;
  vector<Song*>&all=*songs;
  vectorIterator<Song*> song(all);
ITERATE_OVER(song)

    if (song.val()->get_spectrum() != no_spectrum) count++;
  }
  if ( count == 0 ) return Done;
  data = matrix(count,spectrum_size);
  int written = 0;
  vectorIterator<Song*> svi(all);
ITERATE_OVER(svi)

    if (svi.val()->get_spectrum() != no_spectrum)
      {
	assert(written<count);
	++ written; 
	for (int j = 0 ; j < spectrum_size ; j++)
	  data [ written ] [ j + 1 ] = svi.val() -> get_spectrum() -> band ( j ) ;
      }
  }
  // 2. do principal component analysis
  const char * error_msg = NULL;
  do_pca(count,spectrum_size,data,error_msg);
  if (error_msg)
    {
      char text[2000];
      sprintf(text,"Error message is %s",error_msg);
      un->title="Principal Component Analysis";
      un->error="An error occurred, aborting pca\n";
      un->error+=text;
      goto clean_up;
    }
  float4 dx,dy,dz;
  for(int i = 1 ; i <= count; i ++)
    {
#define MIN(A,B) if (B<A) A=B;
#define MAX(A,B) if (B>A) A=B;
      MIN(minx,data[i][1]);
      MIN(miny,data[i][2]);
      MIN(minz,data[i][3]);
      MAX(maxx,data[i][1]);
      MAX(maxy,data[i][2]);
      MAX(maxz,data[i][3]);
    }
  dx=maxx-minx;
  dy=maxy-miny;
  dz=maxz-minz;
  if (dx==0) dx=1;
  if (dy==0) dy=1;
  if (dz==0) dz=1;
  dx/=128.0;
  dy/=128.0;
  dz/=128.0;
  // 3. modify colors of the selected items
  written = 0;
  {
    constVectorIterator<Song*> svi2(all);
ITERATE_OVER(svi2)

      if (svi2.val()->get_spectrum() != no_spectrum)
	{
	  written++;
	  float4 x = (data[written][1] - minx) / dx;
	  float4 y = (data[written][2] - miny) / dy;
	  float4 z = (data[written][3] - minz) / dz;
	  QColor transfer;
	  transfer.setRgb(127+(int)x,127+(int)y,127+(int)z);
	  svi2.val()->setColor(transfer);
	}
    }
  }
 clean_up:
  free_matrix(data, count, spectrum_size);
  delete songs;
  un->status="Spectrum PCA finished";
  un->updateProcessView(true);
  un->send();
  return Done;
}

elementResult ActiveSpectrumPca::terminate()
{
  return deactivate();
}

SpectrumPca spectrumPca;
#endif // __loaded__spectrum_pca_cpp__
