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

#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
#include "qstring-factory.h"

QString **QStringFactory::content = NULL;
int QStringFactory::next = 0;
int QStringFactory::size = 0;

static int winst = 0;
QString QStringFactory::create(char*  str)
{
  enlarge();
  if (str==NULL || !*str) 
    return QString::null;
  QString * newqstr = new QString(str);
  QString * answer = lookup(newqstr);
  if (answer == NULL)
    content[next++]=answer=newqstr;
  else
    {
      winst+=newqstr->length();
      printf("Winst = %d  %s\n",winst,(const char*)(*newqstr));
      delete newqstr;
    }
  return *answer;
}

QString *QStringFactory::lookup(QString *str)
{
  for(int i=0;i<next;i++)
    {
      if (*content[i]==*str)
	return content[i];
    }
  return NULL;
}

void QStringFactory::enlarge()
{
  if (content == NULL)
    {
      size = 1;
      next = 0;
      content=(QString**)malloc(sizeof(QString*)*size);
    }
  if (next>=size)
    {
      size*=2;
      content=(QString**)realloc(content,size*sizeof(QString*));
    }
}



