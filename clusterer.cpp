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
#ifndef __loaded__clusterer_cpp__
#define __loaded__clusterer_cpp__
using namespace std;
#line 1 "clusterer.c++"
#include "cluster.h"
#include "clusterer.h"
#include "user-notification.h"
elementResult ActiveClusterer::reset()
{
  cluster.reset();
  return Done;
}

elementResult ActiveClusterer::addPoint(Point* p)
{
  cluster.addPoint(p);
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
  Couple *result = cluster.agglomerate(&metriek);
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
  return deactivate();
}

Clusterer clusterer;
#endif // __loaded__clusterer_cpp__
