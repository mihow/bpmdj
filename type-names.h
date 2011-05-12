/****
 Borg4 Data Library
 Copyright (C) 2005-2009 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__data.html

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
#ifndef __loaded__type_names_h__
#define __loaded__type_names_h__
using namespace std;
#line 1 "type-names.h++"
#include "numbers.h"
#include "data-string.h"

//---------------------------------------------------------------
//                      Exported
//---------------------------------------------------------------
#define BASIC_TYPE(WHAT, NAME, CLASS) \
inline QString type_name(WHAT ) { return #NAME;}; \
inline Data to_data(WHAT input)     { return CLASS(input);}; \
inline void convertTo(const Data& data, NAME& target) {target = (CLASS)data;};
BASIC_TYPES
#undef BASIC_TYPE
inline QString type_name(Data ) { return "Data";};
inline Data    to_data(Data blah) {return blah;};
inline void    convertTo(const Data& data, Data& target) {target = data;};
#endif // __loaded__type_names_h__
