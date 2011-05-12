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

#include <qstring.h>
#include <stdio.h>

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
 public:
  static Index* index;
 public:
  bool     meta_contains_tar; // true when title author and so on exist !
  char   * meta_filename;     // the .idx file on disk
 private:
  int      index_period;       // the period of the song
  char   * index_tempo;        // the tempo of the song
 public:
  char   * index_file;         // the mp3 file in the music/ directory
  char   * index_remark;       // some small remark (without newlines)
  char   * index_tags;           // a alphabetically sorted tag list
  int      index_changed;        // true when data in memory has changed
  int      index_bpmcount_from;  // lastbpmcount was from ... 
  int      index_bpmcount_to;    // and to ...
  char   * index_md5sum;       // the md5 sum of the mp3
  char   * index_time;         // the normal playing time of the song
  // char * index_playedafter[]; 
  // char * index_album;
  unsigned long     index_cue;   // cues...
  unsigned long     index_cue_z; 
  unsigned long     index_cue_x; 
  unsigned long     index_cue_c; 
  unsigned long     index_cue_v;
  char   * index_spectrum;    /// 26 digits marking the spectrum
 private:
  // information only available from v2.2 on
  char *   title;
  char *   author;
  char *   remix;
  char *   version;
  HistoryField ** prev;         // contains all the songs after which this song has been played
  HistoryField ** next;         // contains all the songs before which this song has been played
  // some conversion functions from old versions
  bool fix_tagline();
  bool fix_tar_info();
  bool split_field_value(char * start, char*&field, char*&value);
 protected:
  void init();
  void free();
  void read_idx(const char*);
  void add_history(HistoryField **&, HistoryField *);
  void add_prev_history(HistoryField *);
  void add_next_history(HistoryField *);
  HistoryField * find_field(HistoryField **&, const char* mp3);
 public:
  Index();             // simply initalizes empty index
  Index(const char*);  // reads index from disk
  ~Index();            // deep destroy of object
  char*  meta_readfrom() {return meta_filename;};
  void   meta_writeto(char* t) {meta_filename = strdup(t);};
  bool   valid_tar_info() {return meta_contains_tar;};
  char * encoded_tar();
  char * readable_description();
  QString get_display_author()  { return author ? author : ""; };
  QString get_display_title();
  QString get_display_version() { if (version) return *version ? version : "1"; else return "";}
  static void read(const char*);
  void write_idx();
  void write_idx(char* t) {meta_filename = strdup(t); write_idx();};
  void executeInfoDialog();
  // history accesors
  HistoryField *add_prev_song(const char* mp3);
  HistoryField *add_next_song(const char* mp3);
  HistoryField **get_prev_songs() {return prev;};
  HistoryField **get_next_songs() {return next;};
  // tempo accessors
  int get_period() {return index_period;};
  void set_period(int t, bool update_on_disk = true);
  double get_tempo() {return index_period > 0 ? 4.0*11025.0*60.0/(double)index_period : -1 ;};
  char * get_tempo_str() {return index_tempo;};
};
