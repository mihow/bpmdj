/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

/*-------------------------------------------
 *         Headers
 *-------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <libgen.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>
#include <qlabel.h>
#include <sys/mman.h>
#include <qlineedit.h>
#include <unistd.h>
#include <time.h>
#include "common.h"
#include "index.h"
#include "version.h"
#include "song-information.h"
#include "memory.h"

/*-------------------------------------------
 *         Performance operations
 *-------------------------------------------*/
static char* buffer = NULL;
static long int buffer_size;
static long int buffer_ptr;
static FILE* buffer_file;

static void buffer_init(FILE * stream)
{
  // read new buffer
  fseek(stream,0,SEEK_END);
  buffer_size=ftell(stream);
  buffer = allocate(buffer_size+1,char);
  // read file
  fseek(stream,0,SEEK_SET);
  fread(buffer,buffer_size,1,stream);
  // sentinel.. so nextchar can work faster
  buffer[buffer_size]='\n';
  buffer_ptr=0;
  buffer_file=stream;
}

static void buffer_map(int fd)
{
  // get size
  buffer_size = lseek(fd,0,SEEK_END);
  // map into memory
  buffer = (char *)mmap(0,buffer_size,PROT_READ,MAP_SHARED,fd,0);
  assert(buffer);
  // set initial buffer_ptr
  buffer_ptr=0;
}

static void buffer_unmap()
{
  munmap(buffer,buffer_size);
}

static int nextchar()
{
  // no end-of-file check because 
  // this is done in the beginning in getline
  return buffer[buffer_ptr++];
}

static char* strldup(char* str, int l)
{
  char * result = allocate(l+1,char);
  strncpy(result,str,l+1);
  return result;
}
	      
static long int buffer_long()
{
  long p = *(long*)(buffer+buffer_ptr);
  buffer_ptr+=4;
  return p;
}

static float8 buffer_double()
{
  float8 p = *(float8*)(buffer+buffer_ptr);
  buffer_ptr+=8;
  return p;
}

static char* buffer_strdup()
{
  int l = strlen(buffer+buffer_ptr);
  char * result =  strldup(buffer+buffer_ptr,l);
  buffer_ptr+=l+1;
  return result;
}

static void buffer_done()
{
  deallocate(buffer);
  buffer=NULL;
  buffer_size=0;
  buffer_file=NULL;
}

ssize_t fast_getline(char **lineptr)
     // warning !!!! -- semantics differ seriously from getline.
{
  int c;
  int l=0;
  char * result;
  // check end of line !
  if (buffer_ptr>=buffer_size)
    return -1;
  // now, we are sure we can read till the end of this line !
  result = buffer+buffer_ptr;
  while((c=nextchar())!='\n') l++;
  // give a pointer back
  *lineptr = result;
  // zero terminate the thing
  result[l]=0;
  return l;
}

/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/
void Index::init()
{
  meta_contains_tar = false;
  meta_inbib = false;
  index_period = -1;
  index_file = 0;
  index_tags = NULL;
  index_md5sum = NULL;
  index_remark = 0;
  meta_changed = 0;
  index_bpmcount_from = -1;
  index_bpmcount_to = -1;
  index_time = NULL;
  index_cue_z = 0;
  index_cue_x = 0;
  index_cue_c = 0;
  index_cue_v = 0;
  index_cue = 0;
  index_spectrum = NULL;
  title = NULL;
  author = NULL;
  remix = NULL;
  version = NULL;
  prev = allocate(1, HistoryField*);
  prev[0]=NULL;
  next = allocate(1,HistoryField*);
  next[0]=NULL;
  albums = allocate(1, AlbumField*);
  albums[0]=NULL;
  clear_energy();
}

void Index::free()
{
  if (meta_filename) ::deallocate(meta_filename);
  meta_filename=NULL;
  if (index_remark)   ::deallocate(index_remark);
  if (index_file)     ::deallocate(index_file);
  if (index_tags)     ::deallocate(index_tags);
  if (index_md5sum)   ::deallocate(index_md5sum);
  if (index_time)     ::deallocate(index_time);
  if (index_spectrum) ::deallocate(index_spectrum);
  // if (index_pattern) deallocate(index_pattern);
  if (title)   ::deallocate(title);
  if (author)  ::deallocate(author);
  if (remix)   ::deallocate(remix);
  if (version) ::deallocate(version);
  assert(prev && next);
  while (*prev) delete(*prev++);
  while (*next) delete(*next++);
  while (*albums) delete(*albums++);
  prev = NULL;
  next = NULL;
  albums = NULL;
}

void Index::write_idx()
{
   FILE*f;
   assert(!meta_inbib);
   if (!meta_filename)
     {
       printf("Error: no name given to write index file to\n");
       return;
     }
   f=fopen(meta_filename,"wb");
   if (!f)
     {
       printf("Warning: cannot open %s for writing\n",meta_filename);
       return;
     }
   fprintf(f,"version  : BpmDj v"VERSION"\n");
   fprintf(f,"file     : %s\n",index_file);
   if (meta_contains_tar)
     {
       if (title && *title) fprintf(f,"title   : %s\n",title);
       if (author && *author) fprintf(f,"author  : %s\n",author);
       if (remix && *remix) fprintf(f,"remix   : %s\n",remix);
       if (version && *version) fprintf(f,"number  : %s\n",version);
     }
   index_period.write_idx(f);
   get_tempo().write_idx(f);
   if (index_tags) fprintf(f,"tag      : %s\n",index_tags);
   if (index_cue>0) fprintf(f,"cue      : %ld\n",index_cue);
   if (index_cue_z>0) fprintf(f,"cue-z    : %ld\n",index_cue_z);
   if (index_cue_x>0) fprintf(f,"cue-x    : %ld\n",index_cue_x);
   if (index_cue_c>0) fprintf(f,"cue-c    : %ld\n",index_cue_c);
   if (index_cue_v>0) fprintf(f,"cue-v    : %ld\n",index_cue_v);
   if (index_bpmcount_from>=0) fprintf(f,"bpmcount-from : %d\n",index_bpmcount_from);
   if (index_bpmcount_to>=0) fprintf(f,"bpmcount-to   : %d\n",index_bpmcount_to);
   if (index_md5sum) fprintf(f,"md5sum : %s\n",index_md5sum);
   if (index_remark) fprintf(f,"remark   : %s\n",index_remark);
   if (index_time) fprintf(f,"time : %s\n",index_time);
   if (index_spectrum) fprintf(f,"spectrum : %s\n",index_spectrum);
   index_min.write("min : ",f);
   index_max.write("max : ",f);
   index_mean.write("mean : ",f);
   index_power.write("power : ",f);
   // dumping of history fields..
   HistoryField **tmp = prev;
   while (*tmp) 
     {
       (*tmp)->write("prev",f);
       tmp++;
     }
   tmp = next;
   while (*tmp) 
     {
       (*tmp)->write("next",f);
       tmp++;
     }
   AlbumField **amp = albums;
   while (*amp)
     {
       fprintf(f,"album : %d : %s\n",(*amp)->nr,(*amp)->name);
       amp++;
     }
   fclose(f);
}

void HistoryField::write(const char* prefix, FILE * f)
{
  fprintf(f,"%s : file  : %s\n", prefix, file);
  fprintf(f,"%s : count : %d\n", prefix, count);
  fprintf(f,"%s : info  : %s\n", prefix, comment ? comment : "");
}

HistoryField::HistoryField(char* primary)
{
  file = strdup(primary);
  count = 0;
  comment = NULL;
}

AlbumField::AlbumField(int r, const char * n)
{
  name = strdup(n);
  nr = r;
}

char* strip_begin(char*tostrip)
{
  while(*tostrip && (*tostrip==' ' || *tostrip=='\t' || *tostrip=='\n')) tostrip++;
  return tostrip;
}

char* strip_end(char*tostrip,char*theend)
{
  int c;
  while(theend>=tostrip && (c=*theend) && (c==' ' || c=='\t' || c=='\n')) theend--;
  *(theend+1)=0;
  return tostrip;
}

char* strip(char*tostrip, char* theend)
{
   tostrip=strip_begin(tostrip);
   return strip_end(tostrip,theend);
}

int strcompare(const void *a, const void * b)
{
  return strcmp(*(const char**)a,*(const char**)b);
}

bool Index::fix_tagline()
{
  // hoe kunnen we de tagline fixed ?
  // we creeeren een array die refereert naar de gevraagde words,
  int length;
  int nextword = 0;
  char* lastword;
  char* words[100];
  char* runner;
  char* temp;
  char * new_tags;
  int i = 0;
  if (index_tags==NULL)
    return false;
  // find all tags
  temp=strdup(index_tags);
  runner=temp;
  length = strlen(runner);
  while(1)
    {
      words[nextword++]=runner;
      while(*runner!=' ' && *runner!=0) runner++;
      if (!*runner) 
	break;
      *runner=0;
      runner++;
    }
  // now sort them 
  qsort(words,nextword,sizeof(char*),strcompare);
  // remove duplicates and create result
  new_tags = allocate(length+1,char);
  runner = new_tags;
  lastword = "";
  for(i = 0 ; i <nextword ; i ++)
    {
      if (strcmp(words[i],lastword)!=0)
	{
	  strcpy(runner,lastword=words[i]);
	  runner+=strlen(words[i]);
	  *runner=' ';
	  runner++;
	}
    }
  if (runner>new_tags)
    *(runner-1)=0;
  else
    *(runner)=0;
  if (strcmp(index_tags,new_tags)==0)
    {
      ::deallocate(new_tags);
      ::deallocate(temp);
      return false;
    }
  else
    {
      printf("Index: tags '%s' to '%s'\n",index_tags,new_tags);
      ::deallocate(index_tags);
      ::deallocate(temp);
      index_tags=new_tags;
      return true;
    }
}

void Index::add_history(HistoryField **& history, HistoryField * field)
{
  // we assume that the historyfield itself is not yet available !
  int c=0;
  HistoryField* *tmp = history;
  while(*tmp) {c++; tmp++;};
  c++;
  history=reallocate(history,c+1,HistoryField*);
  history[c-1]=field;
  history[c]=NULL;
}

void Index::add_album(AlbumField * a)
{
  int c=1;
  AlbumField* *tmp = albums;
  while(*tmp) {c++; tmp++;};
  albums=reallocate(albums,c+1,AlbumField*);
  albums[c-1]=a;
  albums[c]=NULL;
}

AlbumField ** Index::copy_albums()
{
  AlbumField ** tmp = albums;
  int c=1;
  while ( *tmp ) { c++; tmp++; } ;
  AlbumField ** result = allocate(c,AlbumField*);
  result[ --c ] = NULL ;
  while( --c >= 0 ) 
    result[c] = new AlbumField(albums[c]->nr, albums[c]->name);
  return result;
}

AlbumField * Index::find_album(const char* n)
{
  AlbumField* *tmp = albums;
  while(*tmp && (strcmp(n,(*tmp)->name)!=0)) 
    tmp++;
  return *tmp;
}

void Index::delete_album(const char * n)
{
  AlbumField* *tmp = albums;
  while(*tmp && (strcmp(n,(*tmp)->name)!=0)) 
    tmp++;
  while(tmp[0])
    {
      tmp[0]=tmp[1];
      tmp++;
    }
}

void Index::add_prev_history(HistoryField * h)
{
  add_history(prev,h);
}

void Index::add_next_history(HistoryField * h)
{
  add_history(next,h);
}

bool Index::split_field_value(char * c, char * & field, char * & value)
{
  char * line = c;
  while(*c!=':' && *c!=0)
    {
      *c=tolower(*c);
      c++;
    }
  if (!*c) return false;
  *c=0;
  field = strip_end(line,c-1);
  value = strip_begin(c+1);
  return true;
}

bool Index::fix_tempo_fields()
{ 
  if (index_period.valid())
    {
      tempo_type T=get_tempo();
      if (!T.in_range())
	index_period = period_type();
    }
  else if (!(index_period.none()))
    {
      index_period = period_type();
      return true;
    }
  return false;
}

void Index::read_idx(const char* indexn)
{
  char * meta_version = NULL;      // the version format of the index file
  char * line=NULL;
  HistoryField * current = NULL;
  int read;
  // open file
  FILE* index;
  meta_filename = strdup(indexn);
  index=fopen(meta_filename,"rb");
  if (!index) 
    {
      printf("Couldn't read %s\n",indexn);
      exit(5);
    }
  // clear all fields
  init();
  // inititalize buffer
  buffer_init(index);
  // read fields
  while((read=fast_getline(&line))!=-1)
    {
      char * field, * value;
      char * end = line + read;
      if (!split_field_value(strip(line, end - 1), field, value)) continue;
      if (strcmp(field,"version")==0) meta_version=strldup(value,end-value);
      else if(strcmp(field,"period")==0) index_period=atoi(value);
      else if(strcmp(field,"file")==0) index_file=strldup(value,end-value);
      else if(strcmp(field,"tempo")==0) ;
      else if(strcmp(field,"bpmcount-to")==0) index_bpmcount_to=atoi(value);
      else if(strcmp(field,"bpmcount-from")==0) index_bpmcount_from=atoi(value);
      else if(strcmp(field,"comment")==0) index_remark=strldup(value,end-value);
      else if(strcmp(field,"time")==0) index_time=strldup(value,end-value);
      else if(strcmp(field,"cue")==0) index_cue=atol(value);
      else if(strcmp(field,"cue-z")==0) index_cue_z=atol(value);
      else if(strcmp(field,"cue-x")==0) index_cue_x=atol(value);
      else if(strcmp(field,"cue-c")==0) index_cue_c=atol(value);
      else if(strcmp(field,"cue-v")==0) index_cue_v=atol(value);
      else if(strcmp(field,"md5sum")==0) index_md5sum=strldup(value,end-value);
      else if(strcmp(field,"min")==0) index_min.read(value);
      else if(strcmp(field,"max")==0) index_max.read(value);
      else if(strcmp(field,"mean")==0) index_mean.read(value);
      else if(strcmp(field,"power")==0) index_power.read(value);
      else if(strcmp(field,"spectrum")==0) index_spectrum=strldup(value,end-value);
      else if(strcmp(field,"prev")==0 || strcmp(field,"next")==0)
	{
	  char * subfield, * subvalue;
	  if (!split_field_value(value,subfield,subvalue)) continue;
	  if (strcmp(subfield,"file")==0) current = new HistoryField(subvalue);
	  else if (strcmp(subfield,"count")==0)
	    {
	      assert(current);
	      current->count = atol(subvalue);
	    }
	  else if (strcmp(subfield,"info")==0)
	    {
	      assert(current);
	      current->comment = strdup(subvalue);
	      if (*field=='p') add_prev_history(current);
	      else add_next_history(current);
	      current = NULL;
	    }
	}
      else if(strcmp(field,"album")==0)
	{
	  char * subfield,* subvalue;
	  if (!split_field_value(value,subfield,subvalue)) continue;
	  int nr = atoi(subfield);
	  add_album(new AlbumField(nr,subvalue));
	}
      else if(strcmp(field,"title")==0) 
	{
	  title=strldup(value,end-value);
	  meta_contains_tar=true;
	}
      else if(strcmp(field,"author")==0) 
	{
	  author=strldup(value,end-value);
	  meta_contains_tar=true;
	}
      else if(strcmp(field,"remix")==0) 
	{
	  remix=strldup(value,end-value);
	  meta_contains_tar=true;
	}
      else if(strcmp(field,"number")==0) 
	{
	  version=strldup(value,end-value);
	  meta_contains_tar=true;
	}
      else if(strcmp(field,"pattern")==0) 
	{
	  int index_pattern_size=atol(value);
	  assert(index_pattern_size>0);
	  // index_pattern=allocate(index_pattern_size,unsigned char);
	  for(read = 0 ; read < index_pattern_size ; read++)
	    // index_pattern[read] =      
	      nextchar();
	  meta_changed = 1;
	}
      else if(strcmp(field,"tag")==0) 
	{
	  if (!index_tags) index_tags=strldup(value,end-value);
	  else 
	    {
	      char tmp[1000];
	      sprintf(tmp,"%s %s",index_tags,value);
	      ::deallocate(index_tags);
	      index_tags=strdup(tmp);
	    }
	}
      else 
	{
	  printf("Warning: Unknown field '%s' in %s \n",field,meta_filename);
	  continue;
	}
    }
  // finish file access
  buffer_done();
  fclose(index);
  // check for old non-versioned files
  if (!meta_version)
    {
      printf("Error: too old index file %s\n",basename(meta_filename));
      exit(40);
    }
  
  // check for old versions running 22050 samplerate
  if ((strcmp(meta_version,"BpmDj v1.5")==0)
      ||(strcmp(meta_version,"BpmDj v1.4")==0)
      ||(strcmp(meta_version,"BpmDj v1.3")==0)
      ||(strcmp(meta_version,"BpmDj v1.2")==0)
      ||(strcmp(meta_version,"BpmDj v1.1")==0)
      ||(strcmp(meta_version,"BpmDj v1.0")==0)
      ||(strcmp(meta_version,"BpmDj v0.9")==0)
      ||(strcmp(meta_version,"BpmDj v0.8")==0)
      ||(strcmp(meta_version,"BpmDj v0.7")==0)
      ||(strcmp(meta_version,"BpmDj v0.6")==0)
      ||(strcmp(meta_version,"BpmDj v0.5")==0))
    {
      index_cue_z *= 2;
      index_cue_x *= 2;
      index_cue_c *= 2;
      index_cue_v *= 2;
      index_cue   *= 2; 
      meta_changed = 1;
    }

  if (fix_tempo_fields())
    meta_changed=true;
  // the problem of duplicate tags..
  if (fix_tagline())
    meta_changed=1;

  // the problem of no-tar information...
  if (fix_tar_info())
    meta_changed=1;
  
  // release the meta_version, this is no longer necessary because
  // we know which version we currently are
  ::deallocate(meta_version);
}

char* zeroable(char* in)
{
  if (!in) return NULL;
  if (!*in) 
    {
      deallocate(in);
      return NULL;
    }
  return in;
}
void Index::read_v23_field()
{
  HistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  title = buffer_strdup();
  author = buffer_strdup();
  remix = zeroable(buffer_strdup());
  version = buffer_strdup();
  index_file= buffer_strdup();
  buffer_strdup();  // this was the tempo string in previous versions
  index_remark= zeroable(buffer_strdup());
  index_time= buffer_strdup();
  index_md5sum= buffer_strdup();
  index_spectrum= zeroable(buffer_strdup());
  index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  index_period = buffer_long();
  index_bpmcount_from = buffer_long();
  index_bpmcount_to = buffer_long();
  index_cue = buffer_long();
  index_cue_z = buffer_long();
  index_cue_x = buffer_long();
  index_cue_c = buffer_long();
  index_cue_v = buffer_long();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_long();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new HistoryField(buffer_strdup());
      current->count = buffer_long();
      current->comment = buffer_strdup();
      add_prev_history(current);
    }
  count = buffer_long();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new HistoryField(buffer_strdup());
      current->count = buffer_long();
      current->comment = buffer_strdup();
      add_next_history(current);
    }
  count = buffer_long();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_long();
      char * n = buffer_strdup();
      add_album(new AlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void Index::read_v261_field()
{
  HistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  title = buffer_strdup();
  author = buffer_strdup();
  remix = zeroable(buffer_strdup());
  version = buffer_strdup();
  index_file= buffer_strdup();
  buffer_strdup();  // was index_tempo
  index_remark= zeroable(buffer_strdup());
  index_time= buffer_strdup();
  index_md5sum= buffer_strdup();
  index_spectrum= zeroable(buffer_strdup());
  index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  index_period = buffer_long();
  index_bpmcount_from = buffer_long();
  index_bpmcount_to = buffer_long();
  index_cue = buffer_long();
  index_cue_z = buffer_long();
  index_cue_x = buffer_long();
  index_cue_c = buffer_long();
  index_cue_v = buffer_long();
  index_min.left = buffer_long();
  index_min.right = buffer_long();
  index_max.left = buffer_long();
  index_max.right = buffer_long();
  index_mean.left = buffer_long();
  index_mean.right = buffer_long();
  index_power.left = buffer_double();
  index_power.right = buffer_double();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_long();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new HistoryField(buffer_strdup());
      current->count = buffer_long();
      current->comment = buffer_strdup();
      add_prev_history(current);
    }
  count = buffer_long();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new HistoryField(buffer_strdup());
      current->count = buffer_long();
      current->comment = buffer_strdup();
      add_next_history(current);
    }
  count = buffer_long();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_long();
      char * n = buffer_strdup();
      add_album(new AlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

static void file_string(char*str, FILE * f)
{
  int written;
  if (str)
    {
      int len = strlen(str)+1;
      written = fwrite(str,len,1,f);
    }
  else
    {
      int n = 0;
      written = fwrite(&n,1,1,f);
    }
  assert(written==1);
}

static void file_double(float8 i, FILE * f)
{
  int written = fwrite(&i,8,1,f);
  assert(written==1);
}

void Index::write_v261_field(FILE * index)
{
  HistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. write all kinds of strings
  assert(meta_contains_tar);   // otherwise it should not be placed into the .bib file
  file_string(title,index);
  file_string(author,index);
  file_string(remix,index);
  file_string(version,index);
  file_string(index_file,index);
  file_string(get_tempo().get_charstr(),index);
  file_string(index_remark,index);
  file_string(index_time,index);
  file_string(index_md5sum,index);
  file_string(index_spectrum,index);
  file_string(index_tags,index);
  // 2. write all kinds of numbers
  index_period.write_bib_v261(index);
  file_long(index_bpmcount_from,index);
  file_long(index_bpmcount_to,index);
  file_long(index_cue,index);
  file_long(index_cue_z,index);
  file_long(index_cue_x,index);
  file_long(index_cue_c,index);
  file_long(index_cue_v,index);
  file_long(index_min.left,index);
  file_long(index_min.right,index);
  file_long(index_max.left,index);
  file_long(index_max.right,index);
  file_long(index_mean.left,index);
  file_long(index_mean.right,index);
  file_double(index_power.left,index);
  file_double(index_power.right,index);

  // 3. write the list structures... (prev, next, album)
  HistoryField ** tmp = prev;
  int count = 0;
  while(*(tmp++)) count++;
  file_long(count,index);
  tmp = prev;
  while(*tmp)
    {
      current = *tmp;
      file_string(current->file,index);
      file_long(current->count,index);
      file_string(current->comment,index);
      tmp++;
    }

  tmp = next;
  count = 0;
  while(*(tmp++)) count++;
  file_long(count,index);
  tmp = next;
  while(*tmp)
    {
      current = *tmp;
      file_string(current->file,index);
      file_long(current->count,index);
      file_string(current->comment,index);
      tmp++;
    }
  
  count = 0;
  AlbumField**album = albums;
  while(*(album++)) count++;
  file_long(count,index);
  album=albums;
  while(*album)
    {
      
      file_long((*album)->nr,index);
      file_string((*album)->name,index);
      album++;
    }
}

void Index::write_bib_field(FILE * index)
{
  file_long(261,index);
  write_v261_field(index);
}

long Index::read_bib_field(long position, const char* meta_shortname)
{
  static int printed_meta_version = false;
  int meta_version;
  // open file and jump to position
  assert(buffer);
  buffer_ptr=position;
  // clear all fields
  init();
  // version describing the format of this field
  meta_version = buffer_long();
  if (!printed_meta_version)
    {
      printed_meta_version = true;
      printf("Index v%g\n",(float)meta_version/10.0); fflush(stdout);
    }

  // depending on the field we can call different routines
  if (meta_version==23) read_v23_field();
  else if (meta_version==261) read_v261_field();
  else assert(0);
  // fix fields when necessary
  if (fix_tempo_fields()) meta_changed=true;
  if (fix_tagline()) meta_changed=true;
  meta_inbib=true;
  if (meta_shortname)
    {
      char d[500];
      sprintf(d,"%s#%s.bib",meta_shortname,tohex(position));
      meta_filename=strdup(d);
    }
  // return the current position, (= the start of the next field)
  return buffer_ptr;
}

int batch_read_fd;
void Index::init_bib_batchread(const char* name)
{
  batch_read_fd = open(name,O_RDONLY);
  if (batch_read_fd == -1)
    {
      printf("Could not open \"%s\"",name);
      assert(batch_read_fd!=-1);
    }
  buffer_map(batch_read_fd);
}

void Index::done_bib_batchread()
{
  buffer_unmap();
  close(batch_read_fd);
}

bool Index::batch_has_ended()
{
  //printf("buffer_ptr = %ld\n",buffer_ptr);
  //printf("buffer_size = %ld\n",buffer_size);
  return buffer_ptr == buffer_size;
}

void Index::read_bib_field(const char * name)
{
  long field_position = 0;
  assert(name);
  int l = strlen(name);
  int i ;
  assert(l>4);
  assert(strcmp(name+l-4,".bib")==0);
  for (i = 0 ; i < l ; i++)
    if (name[i]=='#')
      {
	assert(i+8<=l-4);
	field_position=toint(name+i+1);
	break;
      }
  // printf("name %s gives position %ld\n",name,field_position);
  assert(i!=l);
  char correctname[l-9];
  strncpy(correctname,name,i);
  strncpy(correctname+i,name+i+9,l-i-8);
  // open file
  init_bib_batchread(correctname);
  read_bib_field(field_position);
  done_bib_batchread();
  meta_filename = strdup(name);
}

bool Index::fix_tar_info()
{
  // WVB -- dat leakt hier nogal wat memory denk ik.. nu who cares, dit is enkel tijdens transitie
  // if there already is title/autor/remix/version info then we skip this
  if (meta_contains_tar) 
    return false;
  assert(meta_filename);
  char * original = strdup(meta_filename);
  int l = strlen(original);
  // if the filename does not end on .idx then we skip
  if (l>4)
    if (strcmp(original+l-4,".idx")!=0)
      return false;
  // ditch the last three characters (.idx)
  original[l-4]=0;
  char * fulltitle = basename(original);
  // obtain title and author
  char * tmp = fulltitle;
  int busy=0;
  assert(!title);
  assert(!version);
  assert(!author);
  assert(!remix);
  title = fulltitle;
  // busy = 0, begonnen aan title
  // busy = 1, begonnen aan remix
  // busy = 2, geeindigt met remix
  // busy = 3, begonnen met author
  // busy = 4, geeindigt met author, begonnen met version
  // busy >= 6, geeindigt met crash
  while(*tmp)
    {
      if (*tmp == '{')
	{
	  if (busy!=0) { busy=6; break; }
	  remix=tmp+1;
	  *tmp=0;
	  busy=1;
	}
      if (*tmp == '}')
	{
	  if (busy!=1) { busy=7; break; }
	  *tmp=0;
	  busy=2;
	  tmp++;
	  if (*tmp!='[') { busy=8; break; }
	}
      if (*tmp == '[')
	{
	  if (busy!=2 && busy!=0) { busy=9; break; }
	  author = tmp+1;
	  *tmp=0;
	  busy=3;
	}
      if (*tmp == ']')
	{
	  if (busy!=3) { busy=10; break; }
	  *tmp=0;
	  busy=4;
	  version = tmp+1;
	}
      tmp++;
    }
  if (busy!=4)
    {
      // printf("failed because of %d\n",busy);
      title=NULL;
      version=NULL;
      remix=NULL;
      author=NULL;
    }
  else
    {
      // printf("Index: fixed %s { %s } [ %s ] %s\n",title,remix, author, version);
      if (title) 
	title = strdup(title);
      if (version)
	version = strdup(version);
      if (author)
	author=strdup(author);
      if (remix)
	remix=strdup(remix);
    }
  ::deallocate(original);
  return meta_contains_tar=busy==4;
}

// constructors and destructors
Index::Index()
{
  init();
}

Index::Index(const char* from)
{
  int l = strlen(from);
  assert(l>4);
  if (strcmp(from+l-4,".idx")==0)
    read_idx(from);
  else 
    read_bib_field(from);
}

Index::~Index()
{
  free();
}

// accessors
char * Index::encoded_tar()
{
  assert(meta_contains_tar);
  char *res;
  char r[200];
  char a[200];
  res=allocate(2000,char);
  if (remix && *remix)
    sprintf(r,"{%s}",remix);
  else
    r[0]=0;
  if (author && *author)
    sprintf(a,"[%s]", author);
  else
    a[0]=0;
  sprintf(res,"%s%s%s%s",
	  (title ? title : ""), 
	  r, 
	  a, 
	  (version ? version : " "));
  return res;
}

char * Index::readable_description()
{ 
  if (meta_contains_tar) 
    return encoded_tar();
  else
    return strdup(meta_filename);
}

char* Index::get_display_title()
{
  if (!title)
    return strdup(meta_filename);
  else if (remix)
    {
      char tmp[2000];
      sprintf(tmp,"%s{%s}",title,remix);
      return strdup(tmp);
    }
  return strdup(title);
}

void Index::set_period(period_type t, bool update_on_disk)
{
  index_period = t; 
  meta_changed = 1;
  if (update_on_disk)
    write_idx();
}; 

// dialog boxes to update the state of an index file
#define field2this(namea,nameb) if (!strxeq(info.namea##Edit->text(),nameb)) \
  { const char* t = info.namea##Edit->text(); \
    nameb = t ? strdup(t) : NULL; \
    meta_contains_tar = ( meta_changed = true ); }
void Index::executeInfoDialog()
{
  SongInformation info(NULL,NULL,TRUE);
  info.idxLabel->setText(meta_filename);
  info.md5Label->setText(index_md5sum);
  info.titleEdit->setText(title);
  info.authorEdit->setText(author);
  info.versionEdit->setText(version);
  info.remixEdit->setText(remix);
  info.tagEdit->setText(index_tags);
  if (info.exec()==QDialog::Accepted)
    {
      field2this(title, title);
      field2this(author, author);
      field2this(remix, remix);
      field2this(version, version);
      field2this(tag, index_tags);
    }
}

HistoryField *Index::add_prev_song(const char* mp3)
{
  HistoryField * f = find_field(prev,mp3);
  f->count++;
  meta_changed=true;
  return f;
}

HistoryField *Index::add_next_song(const char* mp3)
{
  HistoryField * f = find_field(next,mp3);
  f->count++;
  meta_changed=true;
  return f;
}

HistoryField* Index::find_field(HistoryField **&ar, const char * mp3)
{
  HistoryField ** cur = ar;
  while(*cur)
    {
      if (strcmp((*cur)->file,mp3)==0)
	return *cur;
      cur++;
    }
  HistoryField *g = new HistoryField(strdup(mp3));
  add_history(ar,g);
  return g;
}

void Index::set_time(const char* str)
{
  if (!index_time || (strcmp(str,index_time)!=0))
    {
      index_time = strdup(str);
      meta_changed = 1;
    }
};

spectrum_type Index::get_spectrum_copy()
{
  if (!index_spectrum) 
    return no_spectrum;
  spectrum_type result = allocate_spectrum();
  for(int i = 0 ; i < spectrum_size && index_spectrum[i]; i ++)
    result[i]=((spectrum_freq)(index_spectrum[i]-'a'))/24.0;
  return result;
}

bool Index::fully_defined_energy()
{
  return index_min.fully_defined()
    && index_max.fully_defined()
    && index_mean.fully_defined()
    && index_power.fully_defined();
}

void Index::clear_energy()
{
  index_min=sample_type();
  index_max=sample_type();
  index_mean=sample_type();
  index_power=power_type();
}
