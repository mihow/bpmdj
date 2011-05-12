/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include <qapplication.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <stdlib.h>
#include <qptrvector.h>
#include <qprogressbar.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <libgen.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <time.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qcstring.h>
#include <map>
#include <qintcache.h>
#include "kbpm-dj.h"
#include "version.h"
#include "dirscanner.h"
#include "scripts.h"
#include "config.h"
#include "tags.h"
#include "magic.h"
#include "smallhistogram-type.h"
#include "signals-template.cpp"
#include "common.h"
#include "index.h"
#include "version.h"
#include "memory.h"
#include "spectrum-type.h"
#include "kbpm-dj.h"
#include "smallhistogram-type.h"
#include "histogram-property.h"
#include "types.h"
#include "echo-property.h"
#include "composition-property.h"
#include "rythm-property.h"
#include "capacity.h"

using namespace std;

/*-------------------------------------------
 *         OldIndex Album fields
 *-------------------------------------------*/
class OldAlbumField
{
 public:
  char * name;    // name of the album
  int    nr;      // position of the song on the album (-1 if no album is known)
  OldAlbumField(int, const char*);
};

char*   strldup(char* str, int l);
typedef OldAlbumField ** OldAlbumList;

/*-------------------------------------------
 *         OldIndex History fields
 *-------------------------------------------*/
class OldHistoryField
{
 public:
  char * file;    // the mp3 file on disk;
  int    count;   // how many time has this been played
  char * comment; // any comments on this ?
  OldHistoryField(char * primary);
  void increase() { count ++; };
  void write(const char*prefix, FILE * output);
};

/*-------------------------------------------
 *         OldIndex file reader
 *-------------------------------------------*/
class OldIndex
{
 public:
  // meta information
  bool              meta_contains_tar; // true when title author and so on exist !
  char            * meta_filename;     // the .idx file on disk
  bool              meta_inbib;        // true when data is stored in a .bib file
  // song information
  period_type       index_period;      // the period of the song
  unsigned long     index_cue;         // cues...
  unsigned long     index_cue_z;
  unsigned long     index_cue_x;
  unsigned long     index_cue_c;
  unsigned long     index_cue_v;
  char            * index_file;           // the mp3 file in the music/ directory
  char            * index_tags;           // a alphabetically sorted tag list
  char            * index_md5sum;         // the md5 sum of the mp3
  char            * index_time;           // the normal playing time of the song
  spectrum_type   * index_spectrum;       // the bark frequencies
  echo_property     index_histogram;      // the histogram of the SFFT at the different bark bands
  rythm_property    index_rythm;          // the slice rotated SFFT of measures
  composition_property index_composition; // the autocorrelation of the composition of the song
  char            * title;
  char            * author;
  char            * remix;
  char            * version;
  OldHistoryField   ** prev;                     // contains all the songs after which this song has been played
  OldHistoryField   ** next;                     // contains all the songs before which this song has been played
  OldAlbumField     ** albums;                   // list of albums containing this song
  sample4_type       index_min;                  // min value (below zero line) of both channels
  sample4_type       index_max;                  // max value 
  sample4_type       index_mean;                 // mean value
  power_type         index_power;                // rms
  capacity_type      index_disabled_capacities;  // disabled capacities for this song

  void mergeIn(OldIndex& other);
  void mergeIn(OldHistoryField ** &target, OldHistoryField ** source);
  // some conversion functions from old versions
  bool fix_tagline();
  bool fix_tempo_fields();
  bool split_field_value(char * start, char*&field, char*&value);
  void add_history(OldHistoryField **&, OldHistoryField *);
  void add_prev_history(OldHistoryField *);
  void add_next_history(OldHistoryField *);
  void make_valid_tar();
  void init();
  void read_idx(QString);
  spectrum_type *new_spectrum_type(const char* str);
  void spectrum_type_pre27init(spectrum_type & SELF, const char * txt);
  void spectrum_type_post27init(spectrum_type & SELF, const char * txt);
  
  bool fix_tar_info();
  OldIndex();         
  OldIndex(QString);  
  char * encoded_tar();
  char * readable_description();
  void clear_energy();
  void             add_album(OldAlbumField *);
  // cue accessors
  unsigned8        get_cue()                { return index_cue; };
  unsigned8        get_cue_z()              { return index_cue_z; };
  unsigned8        get_cue_x()              { return index_cue_x; };
  unsigned8        get_cue_c()              { return index_cue_c; };
  unsigned8        get_cue_v()              { return index_cue_v; };
  void             set_cue(unsigned8 c)     { if (c != index_cue) { index_cue = c; } };
  void             set_cue_z(unsigned8 c)   { if (c != index_cue_z) { index_cue_z = c;  } };
  void             set_cue_x(unsigned8 c)   { if (c != index_cue_x) { index_cue_x = c;  } };
  void             set_cue_c(unsigned8 c)   { if (c != index_cue_c) { index_cue_c = c;  } };
  void             set_cue_v(unsigned8 c)   { if (c != index_cue_v) { index_cue_v = c;  } };
  void             set_spectrum(spectrum_type*s) { index_spectrum = s;  };
  spectrum_type *  get_spectrum();
  int              get_playcount();
};

/*-------------------------------------------
 *         Merge functionality
 *-------------------------------------------*/
char * catOr(char* a, char *b)
{
  if (a==0) return b;
  if (b==0) return a;
  if (strcmp(a,b)==0) return a;
  char blah[10000];
  sprintf(blah,"%s -or- %s",a,b);
  return strdup(blah);
}

void OldIndex::mergeIn(OldIndex& other)
{
  title   = catOr(title,other.title);
  author  = catOr(author,other.author);
  remix   = catOr(remix, other.remix);
  version = catOr(version, other.version);
  meta_contains_tar |= other.meta_contains_tar;

  if (index_period.none()) index_period=other.index_period;
  if (index_cue_z<=0) index_cue_z=other.index_cue_z;
  if (index_cue_x<=0) index_cue_x=other.index_cue_x;
  if (index_cue_c<=0) index_cue_c=other.index_cue_c;
  if (index_cue_v<=0) index_cue_v=other.index_cue_v;
  
  char new_tags[10000];
  sprintf(new_tags,"%s %s Merged",index_tags,other.index_tags);
  index_tags=strdup(new_tags);
  fix_tagline();
  
  if (!index_md5sum || !*index_md5sum)
    index_md5sum = other.index_md5sum;

  if (!index_time || !*index_time)
    index_time = other.index_time;

  if (!index_spectrum)
    index_spectrum = other.index_spectrum;
  
  if (index_histogram.empty())
    index_histogram=other.index_histogram;
  
  if (index_rythm.empty())
    index_rythm=other.index_rythm;
  
  if (index_composition.empty())
    index_composition=other.index_composition;

  if (!index_min.fully_defined())   index_min=other.index_min;
  if (!index_max.fully_defined())   index_max=other.index_max;
  if (!index_mean.fully_defined())  index_mean=other.index_mean;
  if (!index_power.fully_defined()) index_power=other.index_power;
  
  index_disabled_capacities |= other.index_disabled_capacities;

  // the historyfields are tricky to merge;
  mergeIn(prev,other.prev);
  mergeIn(next,other.next);
  
  OldAlbumField ** album_iterator = other.albums;
  while(*album_iterator)
    {
      OldAlbumField * other_album = *album_iterator;
      OldAlbumField **tmp = albums;
      while(*tmp && strcmp((*tmp)->name,other_album->name)!=0)
	tmp++;
      if (!*tmp)
	add_album(other_album);
      album_iterator++;
    }
}

void OldIndex::mergeIn(OldHistoryField ** &target, OldHistoryField ** source)
{
  for(OldHistoryField ** X = source ; *X ; X++)
    {
      OldHistoryField *x = *X;
      OldHistoryField ** Y = target;
      for( ; *Y ; Y++)
	{
	  OldHistoryField *y = *Y;
	  if(strcmp(x->file,y->file)==0)
	    {
	      y->count=max(y->count,x->count);
	      if (!y->comment || !*y->comment)
		y->comment = x->comment;
	      break;
	    }
	}
      if (!*Y)
	add_history(target,x);
    }
}

/*-------------------------------------------
 *         Forward declarations
 *-------------------------------------------*/
void read_bib_field(OldIndex & SELF, const char* name);
long read_bib_field(OldIndex & SELF, long position, const char* meta_shortname = NULL);
static bool batch_has_ended();
void read_v23_field(OldIndex & SELF);
void read_v261_field(OldIndex & SELF);
void read_v27_field(OldIndex & SELF);
void read_v271_field(OldIndex & SELF);
void read_v272_field(OldIndex & SELF);
void read_v273_field(OldIndex & SELF);
void read_v274_field(OldIndex & SELF);
void read_v291_field(OldIndex & SELF);
static void init_bib_batchread(const char* name);
static void done_bib_batchread();

// also works when a or b or NULL
bool strxeq(const char* a, const char* b)
{
  if (a==NULL || !*a)
    return (b==NULL || !*b);
  if (b==NULL || !*b)
    return false;
  return strcmp(a,b)==0;
}


char* strldup(char* str, int l)
{
  char * result = bpmdj_allocate(l+1,char);
  strncpy(result,str,l+1);
  return result;
}

static char* buffer = NULL;
static long int buffer_size;
static long int buffer_ptr;
static FILE* buffer_file;

static void buffer_init(FILE * stream)
{
  // read new buffer
  fseek(stream,0,SEEK_END);
  buffer_size=ftell(stream);
  buffer = bpmdj_allocate(buffer_size+1,char);
  // read file
  fseek(stream,0,SEEK_SET);
  fread(buffer,buffer_size,1,stream);
  // sentinel.. so nextchar can work faster
  buffer[buffer_size]='\n';
  buffer_ptr=0;
  buffer_file=stream;
}

static void buffer_done()
{
  bpmdj_deallocate(buffer);
  buffer=NULL;
  buffer_size=0;
  buffer_file=NULL;
}

static int nextchar()
{
  // no end-of-file check because 
  // this is done in the beginning in getline
  return buffer[buffer_ptr++];
}

// warning !!!! -- semantics differ seriously from getline.
ssize_t fast_getline(char **lineptr)
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
 *         OldIndex Histogram Type loader
 * - used for rythm, composition and echo 
 *   information
 *-------------------------------------------*/
template <int smallhistogram_size> 
void smallhistogram_type_reader(smallhistogram_type<smallhistogram_size>& SELF,const char* str)
{
  char * cur;
  char * nxt;
  SELF.scale = strtof(str,&cur);
  SELF.validate_scale();
  int count = strtol(cur,&nxt,10);
  cur=nxt;
  if (count>smallhistogram_size) 
    count=smallhistogram_size;
  assert(count<=smallhistogram_size);
  if (count<0) count = 0;
  int i =0;
  for( ; i < count ; i++)
    {
      SELF.bin[i]=strtol(cur,&nxt,16);
      cur = nxt;
    }
  while(i<smallhistogram_size) 
    SELF.bin[i++]=0;
}

template <int bandsize> 
void histogram_reader(histogram_property<bandsize> * whatever, const char * str)
{
  if (whatever->empty())
    whatever->init();
  char *rest;
  int band = strtol(str,&rest,10);
  assert(band>=0 && band<bandsize);
  smallhistogram_type_reader(whatever->bark[band],rest);
}

/*-------------------------------------------
 *         OldIndex Spectrum Type Fixes
 *-------------------------------------------*/
spectrum_type *OldIndex::new_spectrum_type(const char* str)
{
  spectrum_type * result = new spectrum_type();
  while(str[0]==' ' || str[0]=='\t') str++;
  if (strlen(str)==24)
    spectrum_type_pre27init(*result,str);
  else
    spectrum_type_post27init(*result,str);
  return result;
}

void OldIndex::spectrum_type_pre27init(spectrum_type & SELF, const char * txt)
{
  for(int i = 0 ; i < spectrum_size; i++)
    {
      spectrum_freq d = txt[i]-'a';
      if (i==0) d*=2;
      d/=barkbounds[i+1]-barkbounds[i];
      SELF.bark[i]=30+10*log(d)/log(10);
      if (isnan(SELF.bark[i])) SELF.bark[i]=0;
      else if (SELF.bark[i]<0) SELF.bark[i]=0;
    }
  translate_mean(SELF.bark,spectrum_size);
};

void OldIndex::spectrum_type_post27init(spectrum_type & SELF, const char * txt)
{
  const char *cur = txt;
  for(int i = 0 ; i < spectrum_size; i++)
    {
      char* nxt;
      SELF.bark[i]=strtof(cur,&nxt);
      if (isnan(SELF.bark[i])) SELF.bark[i]=0;
      cur=nxt;
    }
};

/*-------------------------------------------
 *         OldIndex operations
 *-------------------------------------------*/
void OldIndex::init()
{
  meta_contains_tar = false;
  meta_inbib = false;
  index_period = -1;
  index_file = 0;
  index_tags = NULL;
  index_md5sum = NULL;
  index_time = NULL;
  index_cue_z = 0;
  index_cue_x = 0;
  index_cue_c = 0;
  index_cue_v = 0;
  index_cue = 0;
  index_spectrum = NULL;
  index_histogram.clear();
  index_rythm.clear();
  index_composition.clear();
  title = NULL;
  author = NULL;
  remix = NULL;
  version = NULL;
  prev = bpmdj_allocate(1, OldHistoryField*);
  prev[0]=NULL;
  next = bpmdj_allocate(1,OldHistoryField*);
  next[0]=NULL;
  albums = bpmdj_allocate(1, OldAlbumField*);
  albums[0]=NULL;
  clear_energy();
  index_disabled_capacities = no_disabled_capacities;
}

void OldHistoryField::write(const char* prefix, FILE * f)
{
  fprintf(f,"%s : file  : %s\n", prefix, file);
  fprintf(f,"%s : count : %d\n", prefix, count);
  fprintf(f,"%s : info  : %s\n", prefix, comment ? comment : "");
}

OldHistoryField::OldHistoryField(char* primary)
{
  file = strdup(primary);
  count = 0;
  comment = NULL;
}

OldAlbumField::OldAlbumField(int r, const char * n)
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

int strcompare(const void *a, const void * b);

bool OldIndex::fix_tagline()
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
  new_tags = bpmdj_allocate(length+1,char);
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
      ::bpmdj_deallocate(new_tags);
      ::bpmdj_deallocate(temp);
      return false;
    }
  else
    {
      printf("OldIndex: tags '%s' to '%s'\n",index_tags,new_tags);
      ::bpmdj_deallocate(index_tags);
      ::bpmdj_deallocate(temp);
      index_tags=new_tags;
      return true;
    }
}

void OldIndex::add_history(OldHistoryField **& history, OldHistoryField * field)
{
  // we assume that the historyfield itself is not yet available !
  int c=0;
  OldHistoryField* *tmp = history;
  while(*tmp) {c++; tmp++;};
  c++;
  history=bpmdj_reallocate(history,c+1,OldHistoryField*);
  history[c-1]=field;
  history[c]=NULL;
}

void OldIndex::add_album(OldAlbumField * a)
{
  int c=1;
  OldAlbumField* *tmp = albums;
  while(*tmp) {c++; tmp++;};
  albums=bpmdj_reallocate(albums,c+1,OldAlbumField*);
  albums[c-1]=a;
  albums[c]=NULL;
}

void OldIndex::add_prev_history(OldHistoryField * h)
{
  add_history(prev,h);
}

void OldIndex::add_next_history(OldHistoryField * h)
{
  add_history(next,h);
}

bool OldIndex::split_field_value(char * c, char * & field, char * & value)
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

bool OldIndex::fix_tempo_fields()
{ 
  if (index_period.valid())
    {
      tempo_type T=period_to_tempo( index_period );
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

void OldIndex::read_idx(QString indexn)
{
  char * meta_version = NULL;
  char * line=NULL;
  OldHistoryField * current = NULL;
  int read;
  FILE* index;
  meta_filename = strdup(indexn);
  index=fopen(meta_filename,"rb");
  if (!index) 
    {
      printf("Couldn't read %s\n",(const char*)indexn);
      exit(5);
    }
  init();
  buffer_init(index);
  while((read=fast_getline(&line))!=-1)
    {
      char * field, * value;
      char * end = line + read;
      if (!split_field_value(strip(line, end - 1), field, value)) continue;
      if (strcmp(field,"version")==0) meta_version=strldup(value,end-value);
      else if(strcmp(field,"period")==0) index_period=atoi(value);
      else if(strcmp(field,"file")==0) index_file=strldup(value,end-value);
      else if(strcmp(field,"tempo")==0) ;
      else if(strcmp(field,"bpmcount-to")==0) ;
      else if(strcmp(field,"bpmcount-from")==0) ;
      else if(strcmp(field,"comment")==0) ;
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
      else if(strcmp(field,"spectrum")==0) index_spectrum=new_spectrum_type(value);
      else if(strcmp(field,"histogram")==0) histogram_reader<96>(&index_histogram,value);
      else if(strcmp(field,"rythm")==0) histogram_reader(&index_rythm,value);
      else if(strcmp(field,"composition")==0) histogram_reader<32>(&index_composition,value);
      else if(strcmp(field,"disabled-capacities")==0) index_disabled_capacities=atol(value);
      else if(strcmp(field,"prev")==0 || strcmp(field,"next")==0)
	{
	  char * subfield, * subvalue;
	  if (!split_field_value(value,subfield,subvalue)) continue;
	  if (strcmp(subfield,"file")==0) current = new OldHistoryField(subvalue);
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
	  add_album(new OldAlbumField(nr,subvalue));
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
	  // index_pattern=bpmdj_allocate(index_pattern_size,unsigned char);
	  for(read = 0 ; read < index_pattern_size ; read++)
	    // index_pattern[read] =      
	      nextchar();
	}
      else if(strcmp(field,"tag")==0) 
	{
	  if (!index_tags) index_tags=strldup(value,end-value);
	  else 
	    {
	      char tmp[1000];
	      sprintf(tmp,"%s %s",index_tags,value);
	      ::bpmdj_deallocate(index_tags);
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
    }
  
  fix_tempo_fields();
  fix_tagline();
  fix_tar_info();
  bpmdj_deallocate(meta_version);
}

bool OldIndex::fix_tar_info()
{
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
      // printf("OldIndex: fixed %s { %s } [ %s ] %s\n",title,remix, author, version);
      if (title) 
	title = strdup(title);
      if (version)
	version = strdup(version);
      if (author)
	author=strdup(author);
      if (remix)
	remix=strdup(remix);
    }
  ::bpmdj_deallocate(original);
  return meta_contains_tar=busy==4;
}

OldIndex::OldIndex()
{
  init();
}

OldIndex::OldIndex(QString from)
{
  read_idx(from);
}

char * OldIndex::encoded_tar()
{
  assert(meta_contains_tar);
  char *res;
  char r[200];
  char a[200];
  res=bpmdj_allocate(2000,char);
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

char * OldIndex::readable_description()
{ 
  if (meta_contains_tar) 
    return encoded_tar();
  else
    return strdup(meta_filename);
}

spectrum_type *OldIndex::get_spectrum()
{
  if (!index_spectrum) return no_spectrum;
  return new spectrum_type(index_spectrum);
}

void OldIndex::clear_energy()
{
  index_min=sample4_type();
  index_max=sample4_type();
  index_mean=sample4_type();
  index_power=power_type();
}

void OldIndex::make_valid_tar()
{
  if (meta_contains_tar) return;
  title = strdup(meta_filename);
  meta_contains_tar = true;
}

int OldIndex::get_playcount()
{
  int total = 0;
  if (prev)
    {
      int i = 0;
      while(prev[i])
	total+=prev[i++]->count;
    }
  if(next)
    {
      int i = 0;
      while(next[i])
	total+=next[i++]->count;
    }
  return total;
}

/*-------------------------------------------
 *         Buffer reading
 *-------------------------------------------*/
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

static long int buffer_signed4()
{
  signed4 p = *(long*)(buffer+buffer_ptr);
  buffer_ptr+=4;
  return p;
}

static unsigned4 buffer_unsigned4()
{
  unsigned4 p = *(unsigned4*)(buffer+buffer_ptr);
  buffer_ptr+=4;
  return p;
}

static void buffer_unmap()
{
  munmap(buffer,buffer_size);
}

static char* buffer_strdup()
{
  int l = strlen(buffer+buffer_ptr);
  char * result =  strldup(buffer+buffer_ptr,l);
  buffer_ptr+=l+1;
  return result;
}

float4 buffer_float4()
{
  float p = *(float*)(buffer+buffer_ptr);
  buffer_ptr+=4;
  return p;
}

static unsigned1 buffer_unsigned1()
{
  unsigned1 p = *(unsigned1*)(buffer+buffer_ptr);
  buffer_ptr+=1;
  return p;
}

float8 buffer_float8()
{
  float8 p = *(float8*)(buffer+buffer_ptr);
  buffer_ptr+=8;
  return p;
}

void buffer_sequence(unsigned1 *a, int l)
{
  memcpy(a,buffer+buffer_ptr,1*l);
  buffer_ptr+=l;
}

/*-------------------------------------------
 *         Bib file reading 
 *-------------------------------------------*/
template <int smallhistogram_size> 
void SH_read_bib_v271(smallhistogram_type<smallhistogram_size>& SELF)
{
  SELF.scale = buffer_float4();
  SELF.validate_scale();
  buffer_sequence(SELF.bin,smallhistogram_size);
}

template <int smallhistogram_size> 
void SH_read_bib_v272(smallhistogram_type<smallhistogram_size>& SELF)
{
  SELF.scale = buffer_float4();
  SELF.validate_scale();
  int count = buffer_unsigned1();
  assert(count>=0);
  buffer_sequence(SELF.bin,count);
  while(count<smallhistogram_size)
    SELF.bin[count++]=0;
}

template <int bandsize> 
void HP_read_bib_v271(histogram_property<bandsize> &SELF)
{
  unsigned1 l = buffer_unsigned1();
  if (l==0) return;
  assert(l==spectrum_size);
  SELF.init();
  for(int i = 0 ; i < spectrum_size ; i++)
    SH_read_bib_v271(SELF.bark[i]);
}

template <int bandsize> 
void HP_read_bib_v272(histogram_property<bandsize> &SELF)
{
  unsigned1 l = buffer_unsigned1();
  if (l==0) return;
  assert(l==spectrum_size);
  SELF.init();
  for(int i = 0 ; i < spectrum_size ; i++)
    SH_read_bib_v272(SELF.bark[i]);
}

void S_read_bib_v27(spectrum_type * SELF)
{
  for(int i = 0 ; i < spectrum_size ; i ++)
    SELF->bark[i]=buffer_float4();
}

int batch_read_fd;
void init_bib_batchread(const char* name)
{
  batch_read_fd = open(name,O_RDONLY);
  if (batch_read_fd == -1)
    {
      printf("Could not open \"%s\"",name);
      assert(batch_read_fd!=-1);
    }
  buffer_map(batch_read_fd);
}

void done_bib_batchread()
{
  buffer_unmap();
  close(batch_read_fd);
}

bool batch_has_ended()
{
  return buffer_ptr == buffer_size;
}

long read_bib_field(OldIndex & SELF, long position, const char* meta_shortname)
{
  int meta_version;
  // open file and jump to position
  assert(buffer);
  buffer_ptr=position;
  // clear all fields
  SELF.init();
  // version describing the format of this field
  meta_version = buffer_signed4();
  // depending on the field we can call different routines
  if (meta_version==23) read_v23_field(SELF);
  else if (meta_version==261) read_v261_field(SELF);
  else if (meta_version==27) read_v27_field(SELF);
  else if (meta_version==271) read_v271_field(SELF);
  else if (meta_version==272) read_v272_field(SELF);
  else if (meta_version==273) read_v273_field(SELF);
  else if (meta_version==274) read_v274_field(SELF);
  else if (meta_version==291) read_v291_field(SELF);
  else assert(0);
  // fix fields when necessary
  SELF.fix_tempo_fields();
  SELF.fix_tagline();
  SELF.meta_inbib=true;
  if (meta_shortname)
    {
      char d[500];
      sprintf(d,"%s#%s.bib",meta_shortname,tohex(position));
      SELF.meta_filename=strdup(d);
    }
  // return the current position, (= the start of the next field)
  return buffer_ptr;
}

void read_bib_field(OldIndex & SELF, const char * name)
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
  read_bib_field(SELF,field_position);
  done_bib_batchread();
  SELF.meta_filename = strdup(name);
}

void read_v23_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // this was the tempo string in previous versions
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  char * tmp = zeroable(buffer_strdup());
  SELF.index_spectrum = tmp ? SELF.new_spectrum_type(tmp) : no_spectrum;
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from =
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v261_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was SELF.index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  char * tmp = zeroable(buffer_strdup());
  SELF.index_spectrum = tmp ? SELF.new_spectrum_type(tmp) : no_spectrum;
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); //SELF.index_bpmcount_from = 
  buffer_signed4(); //SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v27_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); //SELF.index_bpmcount_from = 
  buffer_signed4(); //SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v271_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  HP_read_bib_v271(SELF.index_histogram);
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from = 
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v272_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was SELF.index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  HP_read_bib_v272(SELF.index_histogram);
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from = 
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v273_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  HP_read_bib_v272(SELF.index_histogram);
  HP_read_bib_v272(SELF.index_rythm);
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from = 
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v274_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was SELF.index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  HP_read_bib_v272(SELF.index_histogram);
  HP_read_bib_v272(SELF.index_rythm);
  HP_read_bib_v272(SELF.index_composition);
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from = 
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

void read_v291_field(OldIndex & SELF)
{
  OldHistoryField * current = NULL;
  // initialize the buffer to the correct size
  // 1. read all kinds of strings
  SELF.meta_contains_tar = true;   // otherwise it should not be placed into the .bib file
  SELF.title = buffer_strdup();
  SELF.author = buffer_strdup();
  SELF.remix = zeroable(buffer_strdup());
  SELF.version = buffer_strdup();
  SELF.index_file= buffer_strdup();
  buffer_strdup();  // was SELF.index_tempo
  zeroable(buffer_strdup()); // SELF.index_remark= 
  SELF.index_time= buffer_strdup();
  SELF.index_md5sum= buffer_strdup();
  if (buffer_signed4())
    {
      SELF.index_spectrum = new spectrum_type();
      S_read_bib_v27(SELF.index_spectrum);
    }
  else
    SELF.index_spectrum = no_spectrum;
  HP_read_bib_v272(SELF.index_histogram);
  HP_read_bib_v272(SELF.index_rythm);
  HP_read_bib_v272(SELF.index_composition);
  SELF.index_tags=buffer_strdup();
  // 2. read all kinds of numbers
  SELF.index_period = buffer_signed4();
  buffer_signed4(); // SELF.index_bpmcount_from = 
  buffer_signed4(); // SELF.index_bpmcount_to = 
  SELF.index_cue = buffer_signed4();
  SELF.index_cue_z = buffer_signed4();
  SELF.index_cue_x = buffer_signed4();
  SELF.index_cue_c = buffer_signed4();
  SELF.index_cue_v = buffer_signed4();
  SELF.index_min.left = buffer_signed4();
  SELF.index_min.right = buffer_signed4();
  SELF.index_max.left = buffer_signed4();
  SELF.index_max.right = buffer_signed4();
  SELF.index_mean.left = buffer_signed4();
  SELF.index_mean.right = buffer_signed4();
  SELF.index_power.left = buffer_float8();
  SELF.index_power.right = buffer_float8();
  SELF.index_disabled_capacities = buffer_unsigned4();

  // 3. read the list structures... (prev, next, album)
  int count;
  count = buffer_signed4();
  // printf("prev_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_prev_history(current);
    }
  count = buffer_signed4();
  // printf("next_count = %ld\n",count);
  while(count-->0)
    {
      current = new OldHistoryField(buffer_strdup());
      current->count = buffer_signed4();
      current->comment = buffer_strdup();
      SELF.add_next_history(current);
    }
  count = buffer_signed4();
  // printf("album_count = %ld\n",count);
  while(count-->0)
    {
      int   nr = buffer_signed4();
      char * n = buffer_strdup();
      SELF.add_album(new OldAlbumField(nr,n));
      // printf("Located in album %d : %s\n",nr,n);
    }
}

/*-------------------------------------------
 *         The index of known songs
 *-------------------------------------------*/
struct qstring_cmp
{
  bool operator()(const QString *s1, const QString *s2) const
  {
    return (*s1)<(*s2);
  }
};

static map<QString *, OldIndex*, qstring_cmp> known_songs;

/*-------------------------------------------
 *         The old index reader
 *-------------------------------------------*/
class OldIndexReader: public DirectoryScanner
{
protected:
  virtual void checkfile(const QString prefix, const QString filename);
public:
  OldIndexReader();
};

OldIndexReader::OldIndexReader() : DirectoryScanner(".idx")
{
  scan(IndexDir,IndexDir);
}

void OldIndexReader::checkfile(const QString prefix, const QString  filename)
{
  QString fullname = prefix + "/" + filename;
  OldIndex * index = new OldIndex(fullname);
  //  printf("Reading song %s from index %s\n",index->index_file,index->meta_filename);
  QString * key=new QString(index->index_file);
  if (known_songs[key])
    {
      printf("Duplicate song %s\n",index->index_file);
      printf("  a: from index %s\n",index->meta_filename);
      printf("  b: from index %s\n",known_songs[key]->meta_filename);
      known_songs[key]->mergeIn(*index);
    }
  else
    known_songs[key]=index;
}

/*-------------------------------------------
 *         The Bib file reader
 *-------------------------------------------*/
class BibReader: public DirectoryScanner
{
  protected:
    virtual void checkfile(const QString prefix, const QString filename);
  public:
    BibReader();
};

BibReader::BibReader() : DirectoryScanner(".bib")
{
  scan(IndexDir,IndexDir);
}

/**
 * When we read a bib we read the index and add
 * it into a big map of song-filenames
 */
void BibReader::checkfile(const QString prefix, const QString  filename)
{
  QString fullname = prefix + "/" + filename;
  init_bib_batchread(fullname);
  QString shortname = fullname.left(fullname.findRev(".bib"));
  long position = 0;
  while(!batch_has_ended())
    {
      OldIndex * index = new OldIndex();
      position = read_bib_field(*index,position,shortname);
      known_songs[new QString(index->index_file)]=index;
    }
  done_bib_batchread();
}

/*-------------------------------------------
 *         Main Program
 *-------------------------------------------*/
QApplication *app;
QMutex bpmdj_busy(true);

Song* main_song = NULL;
const char* programname;
QStatusBar* status = NULL;

float4 normalize_freq(float4 in, int band)
{
  assert(0);
  return band;
}

int main(int argc, char* argv[])
{
  programname = argv[0];

  QApplication application(argc,argv);
  app = &application;

  DIR * idir = opendir(IndexDir);
  if (idir == NULL)
    {
      printf("Please start this upgrade program in the directory\n");
      printf("from which bpmdj is started\n");
      exit(0);
    }
  
  // list all the files we already know
  printf("Reading all index files\n");
  OldIndexReader * indexReader = new OldIndexReader();
  delete indexReader;
  // find and extract all bib files
  printf("Reading all bib files\n");
  BibReader * bibReader = new BibReader();
  delete bibReader;
  printf("Converting files\n");
  mkdir("./new-index",0750);
  map<QString*, OldIndex*, qstring_cmp>::const_iterator cur = known_songs.begin();
  while(cur!=known_songs.end())
    {
      pair<QString*, OldIndex*> keyval=*cur;
      OldIndex * index = keyval.second;
      // find a unique name for the file if it is in a .bib 
      if (index->meta_inbib)
	{
	  char * proposal = index->readable_description();
	  char fullprop[500];
	  sprintf(fullprop,"%s.idx",proposal);
	  char * uniquename = findUniqueName("./new-index/",fullprop);
	  index->meta_filename = strdup(uniquename);
	}
      else
	{
	  char * uniquename = findUniqueName("./new-index/",index->meta_filename);
	  index->meta_filename = strdup(uniquename);
	}
      //      printf("Converting song %s in index %s\n",index->index_file,index->meta_filename);
      // transfer all data to a new index
      Index newindex;
      newindex . set_storedin(   index->meta_filename);
      newindex . set_period(     index->index_period);
      newindex . set_cue(        index->get_cue());
      newindex . set_cue_z(      index->get_cue_z());
      newindex . set_cue_x(      index->get_cue_x());
      newindex . set_cue_c(      index->get_cue_c());
      newindex . set_cue_v(      index->get_cue_v());
      newindex . set_filename(   index->index_file);
      newindex . set_tags(       index->index_tags);
      newindex . set_md5sum(     index->index_md5sum);
      newindex . set_time(       index->index_time);
      newindex . set_spectrum(   index->get_spectrum());
      newindex . set_histogram(  index->index_histogram);
      newindex . set_rythm(      index->index_rythm);
      newindex . set_composition(index->index_composition);
      newindex . set_title(      index->title);
      newindex . set_author(     index->author);
      newindex . set_remix(      index->remix);
      newindex . set_version(    index->version);
      if (!index->meta_contains_tar)
	newindex . set_title_author_remix(index->index_file);
      
      OldHistoryField ** fields = index->prev;
      while(*fields)
	{
	  OldHistoryField * hf = *fields;
	  HistoryField * nhf = new HistoryField();
	  nhf->file  = hf->file;
	  nhf->count = hf->count;
	  nhf->comment = hf->comment;
	  newindex.add_prev_history(nhf);
	  fields++;
	}

      fields = index->next;
      while(*fields)
	{
	  OldHistoryField * hf = *fields;
	  HistoryField * nhf = new HistoryField();
	  nhf->file  = hf->file;
	  nhf->count = hf->count;
	  nhf->comment = hf->comment;
	  newindex.add_next_history(nhf);
	  fields++;
	}
      
      OldAlbumField ** albums = index->albums;
      while(*albums)
	{
	  OldAlbumField * af = *albums;
	  AlbumField * naf = new AlbumField(af->nr,af->name);
	  newindex.add_album(naf);
	  albums++;
	}
      
      newindex . index_min   = index -> index_min;
      newindex . index_max   = index -> index_max;
      newindex . index_mean  = index -> index_mean;
      newindex . index_power = index -> index_power;
      newindex . set_disabled_capacities(index->index_disabled_capacities);
      newindex . set_changed();
      newindex . write_idx();

      ++ cur;
    }
}
