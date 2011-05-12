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

#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>
#include "common.h"

/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/

// A. (v2.2) the transition from index-filename encoded title/remix/author/version
// to storing this information within the index file itself should be handled very
// carfeuly because this extra redundancy makes it possible to have al kinds
// of conflicts
// - to avoid invalid use of the field index_readfrom we only allow access
//   through some new methods and rename index_readfrom to meta_filename;
// - to avoid problems in the renamer box we assume that the filename of the
//   indexfile is only relevant when the indexfile doesn't contain any
//   title/author/remiux information. If it does the filename is simply useless
//   (this information can be obtained through the validTarInformation())
// - when reading the index file and there is no valid tar information then
//   we try to obtain it from the filename. If this is possible the index reader
//   will automatically assign title and so on to the filename.
// - general dialog box added to change the song information.
// - de player aanpassen en de selector aanpassen om deze titels te ondersteunen.
// - uitbreiden van het index formaat om played-informatie te ondersteunen...

// B. (v2.3) add album tags and introduce merging
// - add album tag as album : nr : title
// - album tab
//   - expands album wanneer op geklikt, laat toe een song een nieuw nummer te geven
//   - songs selecteren en toevoegen door add-queue moet wel marcheren normaal gesproken
//   - hetzelfde geld voor dubbelklikken, begin de song te spelen
//   - delete from album
// - song tab
//   - insert in album
// - we moeten de mogelijkheid hebben de song info te editeren van al deze nummers
//   door snel een nieuw album nummer in te tikken... 
// / add reading and writing in .bib files (in index)
//   / keep track of version number
//   / read .bib file _only_ when correct version or when forced to do so..
//   / read index in bibfile based on position in the index file. Specified in hex...
//   / write to index file by appending to it... Make sure the lock is ours...
//   / when .idx file

// -- revise...
// B. (v2.3) the transition towards .bib files should be handled carefully
// - kbpm-play blijft met .idx files werken
// - selector kan index bundles (.bib) lezen en schrijven
// - when starting, first scan all .bib files
// - then import all .idx files and modify the necessary fields when available
//   a song is either described within the .bib file, or within an index file.
//   in case it is to be found in the index file, the index file is updated, in the
//   other situation an update is written to disk... The problem that might arise now 
//   is what will happen when 
// - afterward write out the .bib files and remove all .idx/.bib files
// - the index field of every song has a reference to either the bib or the bun file. 
//   in case it's a bib file that should be written out, select one and write it out..
//   because the .bib files are only read by the songselector this shouldn't form many
//   problems...
//   after reading the entire tree we should immediatelyl dump the updated tree to disk
//   so we should read 40M and write 40M every time we start up... Of course only when 
//   .idx files were encountered. The problems are located within the songs without title..
//   these cannot be squashed easily...

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
  int               index_period;      // the period of the song
  char            * index_tempo;       // the tempo of the song
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
 private:
  // some conversion functions from old versions
  bool fix_tagline();
  bool fix_tar_info();
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
  char* tohex(long);
  long  toint(const char*);
  void read_v23_field();
  void read_bib_field(const char* name);
  void write_v23_field(FILE * file);
 public:
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
  int              get_period()             { return index_period; };
  void             set_period(int t, bool update_on_disk = true);
  double           get_tempo()              { return index_period > 0 ? 4.0*11025.0*60.0/(double)index_period : -1; };
  char           * get_tempo_str()          { return index_tempo; };
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
  void             set_md5sum(char*s)       { if (!index_md5sum || strcmp(index_md5sum,s)!=0) {index_md5sum = s; meta_changed=true; } };
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
  char           * get_spectrum()           { return index_spectrum; };
};

#endif
