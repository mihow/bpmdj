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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include "kbpm-played.h"

int Played::songs_played=0;
int Played::size=1;
int Played::next=1;
FILE *Played::f=NULL;
QString** Played::names=NULL;

void Played::Add(QString *filename)
{
  /* put line in memory */
  names[next++]=filename;
  if (next>=size)
    {
      size*=2;
      names=(QString**)realloc(names,size*sizeof(QString*));
    }
}

Played::Played(const QString filename)
{
   /* initialise shit */
   size=100;
   next=0;
   names=(QString**)malloc(sizeof(QString*)*size);
   assert(names);
   /* read the file or create it */
   f=fopen(filename,"rb");
   /* read all lines in memory and sort them */
   if (f!=NULL)
     {
	char  *line = NULL;
	size_t blen = 0;
	ssize_t len = 0;
	while((len=getline(&line,&blen,f))!=-1)
	  {
	    line[len-1]=0;
	    Add(new QString(line));
	  }
	if (line)
	  free(line);
     }
   /* open file for appending */
   f=fopen(filename,"ab");
}

bool Played::IsPlayed(const QString indexname)
{
  for(int i = 0 ; i < next ; i++)
    if (*(names[i])==indexname) 
      return true;
  return false;
}

void Played::Play(const QString indexname)
{
  fprintf(f,"%s\n",(const char*)indexname);
  fflush(f);
  songs_played++;
}
