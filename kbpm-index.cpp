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


#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include "kbpm-index.h"
#include "kbpm-played.h"
#include "kbpm-md5.h"
extern "C"
{
#include "cbpm-index.h"
}

Song::Song()
{
   next=NULL;
   song_id=NULL;
   song_title=NULL;
   song_author=NULL;
   song_version=NULL;
   song_tempo=NULL;
   song_index=NULL;
   song_tags=NULL;
   song_file=NULL;
   song_md5sum=NULL;
   song_played=false;
   song_ondisk=false;
}

Song::Song(char*filename, char* currentpath, char* musicdir)
{
   static int songid=0;
   char * author, *fulltitle,*version,id[200],*tmp,pf[500];
   int len=strlen(filename);
   Song();
   /* NULL */
   assert(filename);
   /* too short */
   if (len<4) return;
   /* check for .idx at the end */
   assert(strcmp(filename+len-4,".idx")==0);
   /* retrieve fullname without .idx */
   fulltitle=strdup(filename);
   assert(fulltitle);
   fulltitle[len-4]=0;
   /* find '[' */
   author=fulltitle;
   while(*author && (*author!='[')) author++;
   if (!*author)
     {
	printf("filename: %s copy: %s\n",filename, fulltitle);
	fflush(stdout);
     }
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
//   printf("Reading \"%s\"\n",pf);
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
   song_file=strdup(index_file);
   if (index_tags) song_tags=strdup(index_tags);
   else song_tags=strdup("");
   if (index_md5sum) song_md5sum=strdup(index_md5sum);
   else 
     {
//	index_md5sum=strdup(song_md5sum=strdup(Sums::Md5(song_file)));
//	index_write();
		song_md5sum=strdup("");
     }
   /* set played flag */
   song_played=Played::IsPlayed(song_index);
   next=NULL;
   /* free all */
   index_free();
   /* try to open the song ! */
   char mp3filename[1024];
   int fd;
   sprintf(mp3filename,"%s/%s",musicdir,song_file);
   fd=open(mp3filename,O_RDONLY);
   if (fd!=-1)
     {
	close(fd);
	song_ondisk=true;
     }
   else
     {
	song_ondisk=false;
     }
}

void SongIndex::add(Song*t)
{
   assert(t);
   assert(!(t->next));
   t->next=list;
   list=t;
}

struct toReadEntry
{
   long inode;
   char* filename;
};

int inode_sorter(const void*a, const void*b)
{
   struct toReadEntry** aa=(struct toReadEntry**)a;
   struct toReadEntry** bb=(struct toReadEntry**)b;
   struct toReadEntry* aaa=*aa;
   struct toReadEntry* bbb=*bb;
   if (aaa->inode < bbb->inode)
     return -1;
   else if (aaa-> inode > bbb->inode)
     return 1;
   else 
     return 0;
}

SongIndex::SongIndex(char* filename, char*dirname, char* musicdir) : Song()
{
   struct toReadEntry **entriesToRead;
   int arraySize=1;
   int toRead=0;
   entriesToRead=(struct toReadEntry**)malloc(sizeof(struct toReadEntry)*arraySize);
   DIR *dir;
   list=NULL;
   struct dirent*entry;
   song_id=strdup(filename);
   // Scan directory 
   printf("reading directory %s\n",dirname);
   dir=opendir(dirname);
   assert(dir);
   while (entry=readdir(dir))
     {
	if (entry->d_type==DT_DIR)
	  {
	     if (strcmp(entry->d_name,".")==0 ||
		 strcmp(entry->d_name,"..")==0) continue;
	     char txt[500];
	     sprintf(txt,"%s/%s",dirname,entry->d_name);
	     add(new SongIndex(entry->d_name,txt,musicdir));
	  }
	else if (entry->d_type==DT_REG)
	  {
	     // put in arraytoread
	     struct toReadEntry * yeah;
	     yeah=(struct toReadEntry*)malloc(sizeof (struct toReadEntry) );
	     yeah->inode=entry->d_ino;
	     yeah->filename=strdup(entry->d_name);
	     entriesToRead[toRead]=yeah;
	     toRead++;
	     if (toRead>=arraySize)
	       {
		  arraySize*=2;
		  entriesToRead=(struct toReadEntry**)realloc(entriesToRead,arraySize*sizeof(struct toReadEntry));
	       }
	  }
     }
   closedir(dir);
   /* Sort all files for a fast read */
   qsort(entriesToRead,toRead,sizeof(struct toReadEntry*),inode_sorter);
   /* Read all files */
   for(int i=0;i<toRead;i++)
     {
	add(new Song(entriesToRead[i]->filename,dirname,musicdir));
     }
   /* Clear data */
   for(int i=0;i<toRead;i++)
     {
	free(entriesToRead[i]->filename);
	free(entriesToRead[i]);
     }
   free(entriesToRead);
}

bool Song::containsTag(const char* tag)
{
   if (!song_tags) return false;
   return strstr(song_tags,tag)!=NULL;
}
