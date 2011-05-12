/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#include "embedded-files.h"

/**
 * Here we perofrm the linkage to the objects
 */
#define FILE(name) \
  extern const char _binary_##name## _start; \
  extern const char _binary_##name## _end; \
  int name## _size; \
  const char * name;
EMBEDDED_FILES
#undef FILE

void init_embedded_files()
{
/**
 * here we map the objectaddresses to proper defined variables
 */
#define FILE(name) \
  name## _size = & _binary_##name## _end - & _binary_##name## _start; \
  name         = & _binary_##name## _start;
EMBEDDED_FILES
#undef FILE
}
