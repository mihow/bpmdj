/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__tempo_type_h__
#define __loaded__tempo_type_h__
using namespace std;
#line 1 "tempo-type.h++"
#include <qstring.h>
#include "constants.h"
#include "flowdb.h"

class tempo_type
{
public:
  float4 tempo;
  /**
   * no_tempo is the constant -1 (defined in types.c++)
   */
  static const float4 no_tempo;
  tempo_type(): tempo(no_tempo)
  {
  }
  tempo_type(float4 p): tempo(p)
  {
  }
  bool valid()
  {
    return !none();
  }
  bool none()
  {
    return tempo == no_tempo;
  }
  bool in_range()
  {
    return tempo > 20.0 && tempo < 400.0;
  }
  signed4 lower(signed4 delta)
  {
    return (signed4)tempo-delta;
  }
  signed4 higher(signed4 delta)
  {
    return (signed4)tempo+delta;
  }
  QString qstring()
  {
    if (none()) return slash;
    if (tempo>=100.0) return QString::number(tempo);
    return zero+QString::number(tempo);
  }
};
#endif // __loaded__tempo_type_h__
