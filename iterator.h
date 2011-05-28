/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__iterator_h__
#define __loaded__iterator_h__
using namespace std;
#line 1 "iterator.h++"
#define ITERATE_OVER(IT) \
  while(IT.prepare_block()) \
    for(IT.start() ;IT.valid(); IT.step()) \
      { IT.need_fields(); 
#endif // __loaded__iterator_h__
