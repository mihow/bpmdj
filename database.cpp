/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__database_cpp__
#define __loaded__database_cpp__
using namespace std;
#line 1 "database.c++"
#include <qlabel.h>
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "selector.h"
#include "players-manager.h"
#include "vector-iterator.h"
#include "tags.h"
#include "heap.h"
#include "statistics.h"
#include "bpmdj.h"
#include "existence-scanner.h"
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "scripts.h"

void DataBase::reset()
{
  clear();
  init();
}

DataBase::DataBase() : all()
{
  init();
}

Song * DataBase::find(QString song_filename)
{
  map<QString,Song*>::iterator ssbf;
  ssbf = file2song.find(song_filename);
  if (ssbf==file2song.end())
    return NULL;
  return ssbf->second;
}

DataBase::~DataBase()
{
  clear();
}

void DataBase::init()
{
  all.clear();
  file2song = map<QString,Song*>();
  cache.clear();
  and_include = NULL;
  or_include = NULL;
  exclude = NULL;
  tag = NULL;
  tag_size = 0;
  rebuild_cache = false;
}

void DataBase::clear()
{
  vectorIterator<Song*> i(all);
ITERATE_OVER(i)
 delete i.val(); }
  all.clear();
  file2song = map<QString,Song*>();
  cache.clear();
  bpmdj_deallocate(and_include);
  bpmdj_deallocate(or_include);
  bpmdj_deallocate(exclude);
  delete[] tag;
}

bool DataBase::cacheValid(SongSelectorLogic * selector)
{
  if (rebuild_cache || selector->tagList->childCount() != tag_size)
    return false;
  Q3ListViewItemIterator it(selector->tagList);
  int i = 0;
  while(it.current())
    {
      Q3ListViewItem *t = it.current();
      tag_type ta = Tags::find_tag(t->text(TAGS_TEXT));
      if ( (ta != tag[i]) ||
	   (t->text(TAGS_AND) == TAG_TRUE) != and_include[i] ||
	   (t->text(TAGS_OR)  == TAG_TRUE) != or_include[i] ||
	   (t->text(TAGS_NOT) == TAG_TRUE) != exclude[i])
	return false;
      i++;
      it++;
    }
  return true;
}

void DataBase::copyTags(SongSelectorLogic * selector)
{
  if (and_include) bpmdj_deallocate(and_include);
  if (or_include) bpmdj_deallocate(or_include);
  if (exclude) bpmdj_deallocate(exclude);
  if (tag) delete[](tag);
  tag_size    = selector->tagList->childCount();
  and_include = bpmdj_allocate(tag_size,bool);
  or_include  = bpmdj_allocate(tag_size,bool);
  exclude     = bpmdj_allocate(tag_size,bool);
  tag         = new tag_type[tag_size];
  and_includes_checked = false;
  excludes_checked = false;
  Q3ListViewItemIterator it(selector->tagList);
  int i = 0;
  while(it.current())
    {
      Q3ListViewItem * t = it.current();
      tag[i] = Tags::find_tag(t->text(TAGS_TEXT));
      // printf("%s = %d\n",(const char*) t->text(TAGS_TEXT),tag[i]);
      or_include[i] = t->text(TAGS_OR) == TAG_TRUE;
      and_includes_checked |= and_include[i] = t->text(TAGS_AND) == TAG_TRUE;
      excludes_checked     |= exclude[i] = t->text(TAGS_NOT) == TAG_TRUE;
      i++;
      it++;
    }
}

bool DataBase::tagFilter(Song* item)
{
  bool matched = false;
  // first we check the or_include tags
  for (int i = 0; !matched && i < tag_size; i++)
    if (or_include[i])
      if (item->contains_tag(tag[i]))
	matched=true;
  if (!matched) return false;
  // now we check the and_include tags
  if (and_includes_checked)
    for(int i = 0; i < tag_size ; i ++)
      if(and_include[i])
	if (!item->contains_tag(tag[i]))
	  return false;
  // now we check the exclusion tags
  if (excludes_checked)
    for (int i=0; i < tag_size; i++)
      if (exclude[i])
	if (item->contains_tag(tag[i]))
	  return false;
  return matched;
}

void DataBase::updateCache(SongSelectorLogic* selector)
{
  if (cacheValid(selector)) return;
  rebuild_cache = false;
  copyTags(selector);
  cache.clear();
  vectorIterator<Song*> song(all);
ITERATE_OVER(song)

    bool vis = tagFilter(song.val());
    if (vis) cache.push_back(song.val());
  }
}

/* WVB -- the thing below can be further optimized
 * 1 - get all flags and pass them in one flag
 */
bool DataBase::filter(SongSelectorLogic* selector, Song *item, Song* main, 
		      float4 limit)
{
  if (main!=NULL && main->get_author()==item->get_author() && 
      !main->get_author().isEmpty())
    item->set_played_author_at_time(History::get_songs_played());
  // song on disk ?
  if (Config::limit_ondisk && !item->get_ondisk())
    return false;
  // song played ?
  if (Config::limit_nonplayed && item->get_played())
    return false;
  // okay, no similar authors please..
  if (Config::limit_authornonplayed && 
      History::get_songs_played() - item->get_played_author_at_time() < 
      Config::get_authorDecay())
    return false;
  // now check the tempo stuff
  if (main && (Config::limit_uprange || Config::limit_downrange))
    if (!item->tempo_show(main,
			  Config::limit_uprange,
			  Config::limit_downrange)) return false;
  // search
  const QString lookingfor = selector -> searchLine -> text();
  if (!lookingfor.isEmpty())
    {
      QString title=item->get_title().upper();
      QString author=item->get_author().upper();
      if (!title.contains(lookingfor) && !author.contains(lookingfor))
	return false;
    }
  // obtain the distance
  bool indistance = item->get_distance_to_main(limit);
  if (Config::limit_indistance && !indistance)
    return false;
  return true;
}

int DataBase::get_unheaped_selection(SongSelectorLogic* selector, Song* main, 
				     QVectorView* target)
{
  // to get an appropriate selection we allocate the necessary vector
  int itemcount=0;
  updateCache(selector);
  Song ** show = bpmdj_allocate(cache.size(),Song*);
  vectorIterator<Song*> song(cache);
ITERATE_OVER(song)

    bool vis = filter(selector,song.val(),main,1.0);
    if (vis) show[itemcount++] = song.val();
  }
  return set_answer(show,itemcount,target);
}

int DataBase::set_answer(Song ** show, int itemcount, QVectorView* target)
{
  if (itemcount)
    show = bpmdj_reallocate(show, itemcount, Song*);
  else
    {
      show = NULL;
      bpmdj_deallocate(show);
    }
  int ibefore = target->currentItem();
  Song * before = NULL;
  if(ibefore>=0 && ibefore < QSong::get_song_count())
    before = QSong::songEssence(ibefore);
  QSong::setVector(show,itemcount);
  int item_to_select = 0;
  if (before && show)
    for(int i = 0 ; i < itemcount ; i ++)
      if (show[i]==before)
	{
	  item_to_select = i;
	  break;
	}
  target->setCurrentItem(item_to_select);
  if (item_to_select < itemcount && item_to_select >=0)
    QSong::set_selected(item_to_select,true);
  target->ensureItemVisible(item_to_select);
  target->vectorChanged();
  return itemcount;
}
 
int DataBase::getSelection(SongSelectorLogic* selector, QVectorView* target, 
			   int count)
{
  Song* main=::main_song;
  // only when we have a dcolor limitation can we use the amount limitation
  if (!Config::limit_indistance || count==0 || main==NULL) 
    return get_unheaped_selection(selector, main, target);
  assert(count>0);
  
  updateCache(selector);
  SongHeap heap(count);
  if (Config::limit_indistance) heap.maximum = 1.0;
  vectorIterator<Song*> song(cache);
ITERATE_OVER(song)

    bool vis = filter(selector,song.val(),main, heap.maximum);
    if (vis) heap.add(song.val());
  }
  Song ** show = bpmdj_allocate(count,Song*);
  int itemcount = heap.copy_to(show);
  assert(itemcount<=count);
  return set_answer(show,itemcount,target);
}

void DataBase::addNewSongs(SongSelectorLogic* selector, QVectorView* target, 
			   vector<Song*> *newsongs)
{
  /**
   * Since these come from the index reader we do not want to limit the 
   * selection to a heap since that would slow everything down. Instead we 
   * simply extend the current selection if the song satisfies all other 
   * criteria (in tempo, proper tag etc) we also do not check cache updates 
   * or the likes, this means that the filter must be applied in 2 steps. 
   * First to check the tag, then to check its distance to the main song
   */
  assert(newsongs);
  if (!newsongs->size()) return;
  Song* show[newsongs->size()];
  unsigned int count=0;
  vectorIterator<Song*> song(newsongs);
ITERATE_OVER(song)

    add(song.val());  // add it to the total list
    bool vis = tagFilter(song.val());
    if (vis) 
    {
      cache.push_back(song.val()); // put it in the cache if necessary
      if (filter(selector,song.val(),::main_song,1.0))
	show[count++]=song.val();  // put it in the current selection
    }
  }
  assert(count<=newsongs->size());
  QSong::addVector(show,count);
  // printf("Accepting %x\n",(unsigned4)newsongs); fflush(stdout);
  target->vectorChanged();
}

void DataBase::add(Song* song)
{
  all.push_back(song);
  if (file2song.find(song->get_file())!=file2song.end())
    {
      Song *song2 = file2song[song->get_file()];
      Fatal("The song \n    %s\n"
	    "occurs at least two times in different index files\n"
	    "The first index file is \n    %s\n"
	    "The second index file is \n    %s",
	    (const char*)song->get_file(),
	    (const char*)song->get_storedin(),
	    (const char*)song2->get_storedin());
    }
  file2song[song->get_file()]=song;
  flush_cache();
};

Song * * DataBase::closestSongs(SongSelectorLogic * selector,
				Song * target1, float4 weight1,
				Song * target2, float4 weight2,
				SongMetriek * metriek, int maximum, int &count)
{
  /*
  cerr << "Looking for a song between " << target1->getDisplayTitle().
  toStdString()
  << " and " << target2->getDisplayTitle().toStdString() 
  << " weight1 = " << weight1 
  << " weight2 = " << weight2
  << " maximum weight = " << maximum 
  << "\n";
  */

  int i, j;
  float4 * minima = bpmdj_allocate(maximum,float4);
  Song * * entries = bpmdj_allocate(maximum,Song*);
  count = 0;
  for(i = 0 ; i < maximum ; i++)
    {
      entries[i]=NULL;
      minima[i]=-1;
    }
  updateCache(selector);
  vectorIterator<Song*> song(cache);
ITERATE_OVER(song)

    // standard checks: tags completed and ondisk 
    if (!song.val()->get_ondisk()) continue;
    if (!tagFilter(song.val())) continue;
    // measure distance, gegeven de metriek
    float8 d = song.val()->distance(target1,weight1,target2,weight2,metriek);
    // find the best position to insert the item..
    for (j = 0 ; j < count ; j++)
      if (minima[j]>d) break;
    if (j==count && count < maximum)
      count++;
    // insert the item
    if (j<count)
      {
	for(int k = maximum - 1 ; k > j ; k --)
	  {
	    minima[k]=minima[k-1];
	    entries[k]=entries[k-1];
	  }
	minima[j]=d;
	entries[j]=song.val();
      }
  }
  return entries;
}
#endif // __loaded__database_cpp__
