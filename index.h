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

#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include "common.h"
#include "types.h"

/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/
class AlbumField
{
 public:
  char * name;    // name of the album
  int    nr;      // position of the song on the album (-1 if no album is known)
  AlbumField(int, const char*);
};

typedef AlbumField ** AlbumList;

class HistoryField
{
 public:
  char * file;    // the mp3 file on disk;
  int    count;   // how many time has this been played
  char * comment; // any comments on this ?
  HistoryField(char * primary);
  void increase() { count ++; };
  void write(const char*prefix, FILE * output);
};

class Index
{
  // the meta fields cover information with repsect to the index file itself
  // all other fields are normally information about the mp3 described.
 private:
  // meta information
  bool              meta_contains_tar; // true when title author and so on exist !
  char            * meta_filename;     // the .idx file on disk
  bool              meta_changed;      // true when data in memory has changed
  bool              meta_inbib;        // true when data is stored in a .bib file
  // song information
  period_type       index_period;      // the period of the song
  unsigned long     index_cue;         // cues...
  unsigned long     index_cue_z;
  unsigned long     index_cue_x;
  unsigned long     index_cue_c;
  unsigned long     index_cue_v;
  char            * index_file;           // the mp3 file in the music/ directory
  char            * index_remark;         // some small remark (without newlines)
  char            * index_tags;           // a alphabetically sorted tag list
  int               index_bpmcount_from;  // lastbpmcount was from ... 
  int               index_bpmcount_to;    // and to ...
  char            * index_md5sum;         // the md5 sum of the mp3
  char            * index_time;           // the normal playing time of the song
  char            * index_spectrum;       // 26 digits marking the spectrum
  // information only available from v2.2 on
  char            * title;
  char            * author;
  char            * remix;
  char            * version;
  HistoryField   ** prev;         // contains all the songs after which this song has been played
  HistoryField   ** next;         // contains all the songs before which this song has been played
  AlbumField     ** albums;       // list of albums containing this song
  // information only available from v2.6 on
 public:
  sample_type       index_min;   // min value (below zero line) of both channels
  sample_type       index_max;   // max value 
  sample_type       index_mean;  // mean value
  power_type        index_power; // rms
 private:
  // some conversion functions from old versions
  bool fix_tagline();
  bool fix_tempo_fields();
  bool split_field_value(char * start, char*&field, char*&value);
  void add_history(HistoryField **&, HistoryField *);
  void add_prev_history(HistoryField *);
  void add_next_history(HistoryField *);
  HistoryField * find_field(HistoryField **&, const char* mp3);
 protected:
  void init();
  void free();
  // reading an index (.idx)
  void read_idx(const char*);
  // reading an index stored in a .bib (.bib)
  void read_v23_field();
  void read_v261_field();
  void read_bib_field(const char* name);
  void write_v261_field(FILE * file);
 public:
  bool fix_tar_info();
  static void init_bib_batchread(const char* name);
  static void done_bib_batchread();
  long read_bib_field(long position, const char* meta_shortname = NULL);
  static bool batch_has_ended();
  Index();             // simply initalizes empty index
  Index(const char*);  // reads index from disk
  ~Index();            // deep destroy of object
  char*  get_storedin() {return meta_filename;};
  void   set_storedin(char* t) {meta_filename = strdup(t);};
  bool   valid_tar_info() {return meta_contains_tar;};
  char * encoded_tar();
  char * readable_description();
  char * get_display_author()  { return author ? strdup(author) : strdup(""); };
  char * get_display_title();
  char * get_display_version() { if (version) return *version ? strdup(version) : strdup("1"); else return strdup("");}
  static void read(const char*);
  void write_idx();
  void write_idx(const char* t) {meta_filename = strdup(t); write_idx();};
  void nolonger_inbib() {meta_inbib=false;};
  void write_bib_field(FILE * file);
  void executeInfoDialog();
  bool can_be_stored_in_bib() {return valid_tar_info();};
  // history accesors
  HistoryField   * add_prev_song  (const char* mp3);
  HistoryField   * add_next_song  (const char* mp3);
  HistoryField * * get_prev_songs ()        {return prev;};
  HistoryField * * get_next_songs ()        {return next;};
  // tempo accessors
  period_type      get_period()             { return index_period; };
  void             set_period(period_type t, bool update_on_disk = true);
  tempo_type       get_tempo()              { return period_to_tempo( index_period ); };
  // statistical accessors
  bool             fully_defined_energy();  
  void             clear_energy();
  sample_type      get_min()                { return index_min; };
  sample_type      get_max()                { return index_max; };
  sample_type      get_mean()               { return index_mean; };
  power_type       get_power()              { return index_power; };
  // album accessors
  void             add_album(AlbumField *);
  void             delete_album(const char * name);
  AlbumField     * find_album(const char* name);
  AlbumField   * * get_albums()  { return albums; };
  AlbumField   * * copy_albums();
  // cue accessors
  unsigned8        get_cue()                { return index_cue; };
  unsigned8        get_cue_z()              { return index_cue_z; };
  unsigned8        get_cue_x()              { return index_cue_x; };
  unsigned8        get_cue_c()              { return index_cue_c; };
  unsigned8        get_cue_v()              { return index_cue_v; };
  void             set_cue(unsigned8 c)     { if (c != index_cue) { index_cue = c; meta_changed=true; } };
  void             set_cue_z(unsigned8 c)   { if (c != index_cue_z) { index_cue_z = c; meta_changed=true; } };
  void             set_cue_x(unsigned8 c)   { if (c != index_cue_x) { index_cue_x = c; meta_changed=true; } };
  void             set_cue_c(unsigned8 c)   { if (c != index_cue_c) { index_cue_c = c; meta_changed=true; } };
  void             set_cue_v(unsigned8 c)   { if (c != index_cue_v) { index_cue_v = c; meta_changed=true; } };
  // time accessors
  void             set_time(const char * str);
  char           * get_time()               { return index_time; };
  // filename accessors
  char           * get_filename()           { return index_file; };
  void             set_filename(char* fn)   { index_file = fn; meta_changed = true; };
  // md5 accessors
  char           * get_md5sum()             { return index_md5sum; };
  void             set_md5sum(char*s)       { if (!index_md5sum || strcmp(index_md5sum,s)!=0) { index_md5sum = s; meta_changed=true; } };
  // tag accessors
  void             set_tags(char * str)     { index_tags = str; };
  char           * get_tags()               { return index_tags; };
  // meta accessors
  bool             changed()                { return meta_changed; };
  void             set_changed()            { meta_changed = true; };
  // scan boundary accessors
  void             set_bpmcount_from(int f) { index_bpmcount_from = f; };
  int              get_bpmcount_from()      { return index_bpmcount_from; };
  void             set_bpmcount_to(int f)   { index_bpmcount_to = f; };
  int              get_bpmcount_to()        { return index_bpmcount_to; };
  // spectrum accessors       
  void             set_spectrum(char*s)     { if (!index_spectrum || strcmp(index_spectrum,s)!=0) {index_spectrum = s; meta_changed=true; } };
  // char           * get_spectrum()           { return index_spectrum; };
  spectrum_type    get_spectrum_copy();
};

#endif
