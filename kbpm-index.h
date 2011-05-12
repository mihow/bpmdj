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


#define MUSIC_DIR "./music"
#define INDEX_DIR "./index"

#include <qlistview.h>
class Song
{
 public:
   Song* next;
   char *song_title;
   char *song_author;
   char *song_version;
   char *song_tempo;
   char *song_index;
   char *song_tags;
   char *song_file;
   char *song_md5sum;
   bool song_played;
   bool song_ondisk;
 public:
   Song();
   Song(char*filename,char* currentpath);
   bool obtainTitleAuthor(char * fulltitle);
   virtual void reread();
   virtual bool isIndex() {return false;};
   bool containsTag(const char* which);
};

class SongIndex: public Song
{
 public:
   char *song_id;
   Song *list;
   void add(Song*t);
   SongIndex() : Song() { scanDir("/", INDEX_DIR); };
   SongIndex(char * filename, char * dirname) : Song() {scanDir(filename, dirname);};
   void scanDir(char * filename, char * dirname);
   virtual void reread() {return;};
   virtual bool isIndex() {return true;};
};
