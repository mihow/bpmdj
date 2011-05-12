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

#include <qstring.h>
#include "qstring-factory.h"

class tempo_type
{
 public:
  float tempo;
  static const float no_tempo = -1;
  tempo_type()
    {
      tempo = no_tempo;
    }
  tempo_type(float p)
    {
      tempo = p;
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
  void write_idx(FILE * f)
    {
      fprintf(f,"tempo    : %g\n",tempo);
    }
  char * get_charstr()
    {
      char T[500];
      if (none())
	return strdup("/");
      if (tempo>=100.0)
	sprintf(T,"%g",tempo);
      else 
	sprintf(T,"0%g",tempo);
      return strdup(T);
    }
  long lower(float delta)
    {
      return (long)(tempo-delta);
    }
  long higher(float delta)
    {
      return (long)(tempo+delta);
    }
  QString qstring()
    {
      // we don't use the factory here because these are only numbers and creating
      // them would simply fill up the factory for nothing: after their usage
      // in listviewitems these are destroyed again
      if (none()) return slash;
      if (tempo>=100.0) return QString::number(tempo);
      return zero+QString::number(tempo);
    }
};
