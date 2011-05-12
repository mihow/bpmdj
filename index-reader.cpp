/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__index_reader_cpp__
#define __loaded__index_reader_cpp__
using namespace std;
#line 1 "index-reader.c++"
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qfile.h>
#include <qdatastream.h>
#include "bpmdj-event.h"
#include "index-reader.h"
#include "bpmdj.h"
#include "song.h"
#include "spectrum-type.h"
#include "database.h"
#include "qstring-factory.h"
#include "constants.h"
#include "dirscanner.h"
#include "vector-iterator.h"

class SongSelectorLogic;
class Song;

IndexReader indexReader;
bool reading_index = false;

class IndexScanner: public DirectoryScanner
{
protected:
  virtual void checkfile(const QString prefix, const QString filename);
public:
  vector<Song*> *songs_this_turn;
  IndexScanner() : DirectoryScanner(IndexDir,IdxExt)
  {
  };
};

void IndexScanner::checkfile(const QString prefix, const QString  filename)
{
  Index index(prefix + slash + filename);
  songs_this_turn->push_back(new Song(&index,true,false));
}

elementResult ActiveIndexReader::start(int expected)
{
  if (reader) delete reader;
  reading_index=true;
  // the chunk size is determined by  a maximum of 100 files per thunk and
  // and a minimum of 1 percent of the bar
  int chunk_size = expected / 100;
  expected_files = expected;
  if (chunk_size < 100) chunk_size = 100;
  reader = new IndexScanner();
  reader->set_file_per_turn(chunk_size);
  total_files = 0;
  queue_thunk();
  return Done;
}

elementResult ActiveIndexReader::terminate()
{
  reading_index=false;
  deactivate();
  return Done;
}

class CollectionOfSongs: public BpmDjEvent
{
private:
  vector<Song*> *collection;
  int total_files;
  int expected_files;
public:
  CollectionOfSongs(vector<Song*> *sc, int t, int e):
    collection(sc),
    total_files(t),
    expected_files(e)
  {
    if (app) app->postEvent(selector,this);
  }
  virtual void run()
  {
    unsigned4 progress= total_files*100/(expected_files ? expected_files : 1);
    ::status->showMessage("Reading index: "+QString::number(progress)+"%",5000);
    // printf("Passing along %x\n",(unsigned4)collection); fflush(stdout);
    if (collection->size())
      selector->step_reading_indices(collection);
    else
      selector->stop_reading_indices(total_files);
  }
  
  virtual ~CollectionOfSongs()
  {
    // printf("Deleting  %x\n",(unsigned4)collection); fflush(stdout);
    delete collection;
  }
};

elementResult ActiveIndexReader::thunk()
{
  assert(reader);
  reader->songs_this_turn=new vector<Song*>();
  int read = reader->scan();
  total_files+=read;
  if (total_files > expected_files) 
    expected_files = total_files*100/90;
  new CollectionOfSongs(reader->songs_this_turn,total_files,expected_files);
  if (read)
    return RevisitAfterIncoming;
  else
    {
      reading_index=false;
      delete reader;
      return Done;
    }
}
#endif // __loaded__index_reader_cpp__
