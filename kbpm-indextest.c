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

#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include "../cBpmDj/cbpm-index.h"

char *song_id;
char *song_title;
char *song_author;
char *song_version;
char *song_tempo;
char *song_index;
int song_played;

void InitSong()
{
   song_id=NULL;
   song_title=NULL;
   song_author=NULL;
   song_version=NULL;
   song_tempo=NULL;
   song_index=NULL;
   song_played=0;
}

void Song(char*filename, char* currentpath)
{
   static int songid=0;
   char * author, *fulltitle,*version,id[200],*tmp,pf[500];
   int len=strlen(filename);
   InitSong();
   /* NULL */
   assert(filename);
   /* too short */
   if (len<4) return;
   /* check for .idx at the end */
   assert(strcmp(filename+len-4,".idx")==0);
   /* retrieve fullname without .idx */
   fulltitle=strdup(filename);
   fulltitle[len-4]=0;
   /* find '[' */
   author=fulltitle;
   while(*author && (*author!='[')) author++;
   assert(*author);
   *author=0;
   author++;
   /* version is everything after the last ] */
   version=author;
   while(*version && (*version!=']')) version++;
   assert(*version);
   *version=0;
   version++;
   if (!*version) version=strdup("1");
   /* create id */
   sprintf(id,"%5d",songid++);
   /* full index file */
   sprintf(pf,"%s/%s",currentpath,filename);
   /* read the index file */
   printf("Reading \"%s\"\n",pf);
   index_read(pf);
   if (index_changed)
     index_write();
   /* copy everything to object */
   song_id=strdup(id);
   song_title=fulltitle;
   song_author=author;
   song_tempo=strdup(index_tempo);
   song_version=version;
   song_index=strdup(pf);
   song_played=0;
   /* free all */
   index_free();
     {
	static int thissize=0;
	thissize+=4+4;
	thissize+=strlen(song_id)+1+4;
	thissize+=strlen(song_title)+1+4;
	thissize+=strlen(song_author)+1+4;
	thissize+=strlen(song_version)+1+4;
	thissize+=strlen(song_tempo)+1+4;
	thissize+=strlen(song_index)+1+4;
	printf("  %d ",thissize/1024);
     }
}

void SongIndex(char* filename, char*dirname)
{
   DIR *dir;
   struct dirent*entry;
   song_id=strdup(filename);
   printf("reading directory %s\n",dirname);
   dir=opendir(dirname);
   assert(dir);
   while (entry=readdir(dir))
     {
	if (entry->d_type==DT_DIR)
	  {
	     char txt[500];
	     if (strcmp(entry->d_name,".")==0 ||
		 strcmp(entry->d_name,"..")==0) continue;
	     sprintf(txt,"%s/%s",dirname,entry->d_name);
	     SongIndex(entry->d_name,txt);
	  }
	else if (entry->d_type==DT_REG)
	  Song(entry->d_name,dirname);
     }
   closedir(dir);
}

int main(int argc, char* argv[])
{
   SongIndex("/","../cBpmDj/index");
}
