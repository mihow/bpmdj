/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__index_h__
#define __loaded__index_h__
using namespace std;
#line 1 "index.h++"
#include <stdio.h>
#include "common.h"
#include "type-operations.h"
#include "spectrum-type.h"
#include "echo-property.h"
#include "composition-property.h"
#include "rhythm-property.h"
#include "capacity.h"
#include "Data/om-data.h"

/*-------------------------------------------
 *          Album Objects
 *-------------------------------------------*/
class DataError
{
public:
  string msg;
  DataError(string s): msg(s)
  {
  };
};

/*-------------------------------------------
 *          Album Objects
 *-------------------------------------------*/
class AlbumField
{
 public:
  QString name;    // name of the album

  /**
   * Position of the song on the album (-1 if no album is known)
   */
  int nr;

  AlbumField(int, QString);
  AlbumField(Data &data)
    {
      set_data(data);
    }
  Data get_data(int versionnr) const;
  void set_data(Data &data); 
};

typedef AlbumField ** AlbumList;

/*-------------------------------------------
 *           History Fields
 *-------------------------------------------*/
class HistoryField
{
 public:
  QString file;    // the mp3 file on disk;
  int     count;   // how many time has this been played
  QString comment; // any comments on this ?
  HistoryField() : count(0)
    {
    }
  HistoryField(QString data);
  HistoryField(Data & data)
    {
      set_data(data);
    }
  void increase() { count ++; };
  Data get_data(int version);
  void set_data(Data &data);
};

/*-------------------------------------------
 *            Indices
 *-------------------------------------------*/
class SongInformation;
class Index
{
  friend class SongInformation;
  // the meta fields cover information with respect to the index file itself
  // all other fields are normally information about the mp3 described.
 private:
  QString           meta_filename;     // the .idx file on disk
  bool              meta_changed;      // true when data in memory has changed
  // song information
  period_type       index_period;      // the period of the song
  unsigned4         index_cue;         // cues...
  unsigned4         index_cue_z;
  unsigned4         index_cue_x;
  unsigned4         index_cue_c;
  unsigned4         index_cue_v;
  /**
   * The mp3 file in the music/ directory
   */
  QString           index_file;      
  /**
   * An alphabetically sorted tag list
   */
  QString           index_tags;      
  /**
   * MD5 sum of the mp3
   */
  QString           index_md5sum; 
  /**
   * Normal playing time of the song
   */
  QString           index_time;      
  /**
   * The bark frequencies
   */
  spectrum_type   * index_spectrum;  
  /**
   * The histogram of the SFFT at the different bark bands
   */
  echo_property     index_histogram; 
  /**
   * the slice rotated SFFT of the average measure
   */
  rhythm_property   index_rhythm;    
  /**
   * The autocorrelation of the composition of the song
   */
  composition_property index_composition; 
  // information only available from v2.2 on
  QString           title;
  QString           author;
  QString           remix;
  QString           version;
  /**
   * Contains all the songs after which this song has been played
   */
  HistoryField   ** prev;         
  /**
   * Contains all the songs before which this song has been played
   */
  HistoryField   ** next;         
  AlbumField     ** albums;       // list of albums containing this song
  // information only available from v2.6 on
 public:
  sample4_type      index_min;   // min value (below zero line) of both channels
  sample4_type      index_max;   // max value 
  sample4_type      index_mean;  // mean value
  power_type        index_power; // rms
  // information available from v2.9 and above
  /**
   * Disabled capacities for this song
   */
  capacity_type     index_disabled_capacities;  

  //----------------------------------
  // creation, loading and storing
  //----------------------------------
 public:
  /** 
   * Initializes empty index
   */
  Index();             
  /**
   * Reads index from disk
   */
  Index(QString);  
  /**
   * Deep destroy of object
   */
  ~Index();          
  /**
   * Initializes the index by reading the data from the passed filename
   */
  void read_idx(QString);
  /**
   * Writes the index to the file 'meta_filename'. Returns true
   * if the file was completely written. False otherwise.
   */
  bool write_idx();
  /**
   * Writes the index to a new filename, replaces meta_filename first
   * Returns true if the file was completely written. False otherwise.
   */
  bool write_idx(QString t) 
    {
      meta_filename = t; 
      return write_idx();
    };
  /**
   * Returns the place where this index is stored in
   */
  QString get_storedin()
    {
      return meta_filename;
    };
  /**
   * Sets the place where this index is stored in
   */
  void set_storedin(QString t)
    {
      meta_filename = t;
    };
  /**
   * Returns whether the data has changed
   */
  bool changed()                
    {
      return meta_changed; 
    };
  /**
   * Marks the index as 'changed'
   */
  void set_changed()            
    {
      meta_changed = true; 
    };
 private:
  /**
   * Initializes the object with empty values
   */
  void init();
  /**
   * Removes all the self allocated objects
   */
  void free();
  
  //--------------------------
  // song filename accessors
  //--------------------------
 public:
  QString get_filename()           
    {
      return index_file; 
    };
  void set_filename(QString fn)   
    {
      index_file = fn; 
      meta_changed = true; 
    };

  //-----------------------
  // period / tempo field
  //-----------------------
 public:
  period_type      get_period()             
    {
      return index_period; 
    };
  void             set_period(period_type t, bool update_on_disk = true);
  tempo_type       get_tempo()
    {
      return period_to_tempo(index_period); 
    };
 private:
  // some old conversion functions useful to verify the range
  bool fix_tempo_fields();
  // returns the data for a period 
  Data get_data(const period_type &p);
  
  //------------------
  // history fields
  //------------------
 public:
  HistoryField   * add_prev_song  (QString mp3);
  HistoryField   * add_next_song  (QString mp3);
  // returns a zero terminated array of prev HistoryFields
  HistoryField * * get_prev_songs ()        
    {
      return prev;
    };
  // returns a zero terminated array of next HistoryFields
  HistoryField * * get_next_songs ()        
    {
      return next;
    };
  // adds a new previous history field
  void add_prev_history(HistoryField *);
  // adds a new next history field
  void add_next_history(HistoryField *);
 private:
  // returns the data for a zero terminated list of historyfields
  Data get_data(HistoryField ** fields);
  void add_history(HistoryField **&, HistoryField *);
  HistoryField * find_field(HistoryField **&, QString mp3);

  //------------------
  // title accessors
  //------------------
 public:
  QString get_title()
    {
      return title;
    }
  void set_title(QString t)
    {
      if (meta_changed || title != t)
	{
	  title = t;
	  meta_changed = true;
	}
    }
  // will attach the remix when possible
  QString get_display_title();
  // will append remix author and version if possible
  QString readable_description();
  /**
   * Tries to figure out the name, remix, author and version from the song 
   * filename
   */
  bool set_title_author_remix(QString filename);

  //-------------------
  // author accessors
  //-------------------
  QString get_author()
    {
      return author;
    }
  void set_author(QString new_author)
    {
      if (meta_changed || author != new_author)
	{
	  author = new_author;
	  meta_changed = true;
	}
    }

  //-------------------
  // version accessors
  //-------------------
  QString get_version()
    {
      return version;
    }
  void set_version(QString new_version)
    {
      if (meta_changed || version != new_version)
	{
	  version = new_version;
	  meta_changed = true;
	}
    }

  //-------------------
  // remix accessors
  //-------------------
  QString get_remix()
    {
      return remix;
    }
  void set_remix(QString t)
    {
      if (meta_changed || remix != t)
	{
	  remix = t;
	  meta_changed = true;
	}
    }

  //-------------------
  // tagline accessors
  //-------------------
 public:
  void set_tags(QString str)
    {
      index_tags = str;
      meta_changed = true;
    };
  QString get_tags()               
    {
      return index_tags; 
    };
 private:
  bool fix_tagline();

  //--------------------------
  // energy/amplitude fields
  //--------------------------
 public:
  bool             fully_defined_energy();  
  void             clear_energy();
  sample4_type     get_min()                
    {
      return index_min; 
    };
  sample4_type     get_max()                
    {
      return index_max; 
    };
  sample4_type     get_mean()               
    {
      return index_mean; 
    };
  power_type       get_power()              
    {
      return index_power; 
    };

  //--------------------------
  // album fields
  //--------------------------
 public:
  void             add_album(AlbumField *);
  void             delete_album(QString name);
  AlbumField     * find_album(QString name);
  AlbumField   * * get_albums()  { return albums; };
  AlbumField   * * copy_albums();

  //--------------------------
  // cue fields
  //--------------------------
 public:
  unsigned8        get_cue()                
    {
      return index_cue; 
    };
  unsigned8        get_cue_z()              
    { 
      return index_cue_z; 
    };
  unsigned8        get_cue_x()              
    { 
      return index_cue_x; 
    };
  unsigned8        get_cue_c()              
    { 
      return index_cue_c; 
    };
  unsigned8        get_cue_v()              
    { 
      return index_cue_v; 
    };
  void             set_cue(unsigned8 c)     
    { 
      if (c != index_cue)
      { 
	index_cue = c; meta_changed=true; 
      } 
    };
  void             set_cue_z(unsigned8 c)   
    { if (c != index_cue_z) 
      { 
	index_cue_z = c; meta_changed=true; 
      } 
    };
  void             set_cue_x(unsigned8 c)   
    {
      if (c != index_cue_x)
      { 
	index_cue_x = c; meta_changed=true; 
      } 
    };
  void             set_cue_c(unsigned8 c)  
    { 
      if (c != index_cue_c) 
      {
	index_cue_c = c; meta_changed=true; 
      } 
    };
  void             set_cue_v(unsigned8 c)  
    {
      if (c != index_cue_v)
      {
	index_cue_v = c; meta_changed=true; 
      } 
    };
  
  //--------------------------
  // time fields
  //--------------------------
 public:
  void             set_time(QString str);
  QString          get_time()               
    {
      return index_time; 
    };
  unsigned2        get_time_in_seconds();

  //--------------------------
  // md5sum 
  //--------------------------
 public:
  QString get_md5sum()             
    {
      return index_md5sum; 
    };
  void set_md5sum(QString s)
    { 
      if (meta_changed || index_md5sum!=s)
	{
	  index_md5sum = s; 
	  meta_changed=true;
	}
    };
  
  //--------------------------
  // spectrum
  //--------------------------
 public:
  void             set_spectrum(spectrum_type*s) 
    {
      index_spectrum = s; 
      meta_changed=true; 
    };
  spectrum_type *  get_spectrum();

  //--------------------------
  // echo
  //--------------------------
 public:
  void             set_histogram(echo_property h) 
    {
      index_histogram = h; 
      meta_changed=true; 
    };
  echo_property    get_histogram()          
    {
      return index_histogram; 
    };

  //--------------------------
  // rhythm
  //--------------------------
 public:
  rhythm_property get_rhythm() 
  {
    return index_rhythm; 
  };
  void set_rhythm(rhythm_property r) 
  {
    index_rhythm = r ; 
    meta_changed = true; 
  } ;
  
  //--------------------------
  // composition
  //--------------------------
 public:
  void             set_composition(composition_property r)
    {
      index_composition = r ; 
      meta_changed = true; 
    } ;
  composition_property get_composition() 
    {
      return index_composition; 
    };

  //--------------------------
  // disabled capacities
  //--------------------------
 public:
  capacity_type get_disabled_capacities() 
  {
    return index_disabled_capacities; 
  };
  void set_disabled_capacities(capacity_type c) 
  {
    index_disabled_capacities = c; 
    meta_changed = true; 
  };
  
  //--------------------------
  // other stuff
  //--------------------------
public:
  void executeInfoDialog();
  // total play count of this song (before or after a mix)
  unsigned4 get_playcount();
};


/**
 * Creates a new unique index filename and refer to the song, thereby tagging
 * it as New
 */ 
Index * createNewIndexFor(QString song, QString directory);
/**
 * Prefixes the extension of filename with a number until a unique filename is
 * found
 */
QString findUniqueName(QString directory, QString filename);
#endif // __loaded__index_h__
