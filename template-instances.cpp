/****
 Hierarchical Data Objects
 Copyright (C) 2005-2011 Werner Van Belle
 http://flow.yellowcouch.org/data/

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
#ifndef __loaded__template_instances_cpp__
#define __loaded__template_instances_cpp__
using namespace std;
#line 1 "template-instances.c++"
#include "om-data.h"

#define ZERO_TYPES \
ARRAY_TYPE(0,signed1)  \
ARRAY_TYPE(0,signed2)  \
ARRAY_TYPE(0,signed4)  \
ARRAY_TYPE(0,signed8)  \
ARRAY_TYPE(0,unsigned1)\
ARRAY_TYPE(0,unsigned2)\
ARRAY_TYPE(0,unsigned4)\
ARRAY_TYPE(0,unsigned8)\
ARRAY_TYPE(0,float4)   \
ARRAY_TYPE(0,float8)   \
ARRAY_TYPE(0,Data)     

#define ARRAY_TYPE(NR,TYPE) template class ArrayMeta<NR,TYPE>;
ARRAY_TYPES
ZERO_TYPES
#undef ARRAY_TYPE

#define ARRAY_TYPE(NR,TYPE) template class Array<NR,TYPE>;
ARRAY_TYPES
ZERO_TYPES
#undef ARRAY_TYPE

template class ArrayIteratorBacking<Data, false>;
template class ArrayIteratorBacking<Data, true>;

#endif // __loaded__template_instances_cpp__
