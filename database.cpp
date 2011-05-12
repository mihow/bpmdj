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

#include <qlabel.h>
#include <pthread.h>
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "songselector.logic.h"
#include "process-manager.h"
#include "songtree.h"
#include "avltree.h"
#include "growing-array.h"
#include "tags.h"
#include "heap.h"
#include "song-statistics.h"
#include "kbpm-dj.h"
#include "existence-scanner.h"

/**
 * The database cache contains items that are visible when only considereing the taglist
 */
DataBase::DataBase() : cache()
{
  init();
}

DataBase::~DataBase()
{
}

void DataBase::init()
{
  BasicDataBase::init();
  cache.reset();
  and_include = NULL;
  or_include = NULL;
  exclude = NULL;
  tag = NULL;
  tag_size = 0;
  rebuild_cache = false;
}

void DataBase::clear()
{
  BasicDataBase::clear();
  cache.reset();
  bpmdj_deallocate(and_include);
  bpmdj_deallocate(or_include);
  bpmdj_deallocate(exclude);
  delete[] tag;
}

void DataBase::start_existence_check()
{
  (new ExistenceScanner(all))->startAnalyzer();
}

bool DataBase::cacheValid(SongSelectorLogic * selector)
{
  if (rebuild_cache || selector->tagList->childCount() != tag_size)
    return false;
  QListViewItemIterator it(selector->tagList);
  int i = 0;
  while(it.current())
    {
      QListViewItem *t = it.current();
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
  QListViewItemIterator it(selector->tagList);
  int i = 0;
  while(it.current())
    {
      QListViewItem * t = it.current();
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
  cache.reset();
  for(int i = 0 ; i < all.count ; i ++)
    {
      Song * song = all.elements[i];
      bool vis = tagFilter(song);
      if (vis) cache.add(song);
    }
}

/* WVB -- the thing below can be further optimized
 * 1 - get all flags and pass them in one flag
 */
bool DataBase::filter(SongSelectorLogic* selector, Song *item, Song* main, float limit)
{
  if (main!=NULL && main->get_author()==item->get_author() && !main->get_author().isEmpty())
    item->set_played_author_at_time(History::get_songs_played());
  // song on disk ?
  if (Config::limit_ondisk && !item->get_ondisk())
    return false;
  // song played ?
  if (Config::limit_nonplayed && item->get_played())
    return false;
  // okay, no similar authors please..
  if (Config::limit_authornonplayed && 
      History::get_songs_played() - item->get_played_author_at_time() < Config::get_authorDecay())
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
      if (title.contains(lookingfor)+author.contains(lookingfor)==0)
	return false;
    }
  // obtain the distance
  bool indistance = item->get_distance_to_main(limit);
  if (Config::limit_indistance && !indistance)
    return false;
  return true;
}

int DataBase::get_unheaped_selection(SongSelectorLogic* selector, Song* main, QVectorView* target)
{
  // to get an appropriate selection we allocate the nessary vector
  int itemcount=0;
  updateCache(selector);
  Song * * show = bpmdj_allocate(cache.count,Song*);
  for(int i = 0; i < cache.count ; i ++)
    {
      Song *song = cache.elements[i];
      bool vis = filter(selector,song,main,1.0);
      if (vis)
	show[itemcount++] = song;
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
  // store old current item if available
  Song * before = NULL;
  if(ibefore>=0 && ibefore < QSong::get_song_count())
    before = QSong::songEssence(ibefore);
  // set the vector
  QSong::setVector(show,itemcount);
  // find the old current item and make it the current again
  //  if (before)
  //    printf("Song before was %s\n",(const char*)before->getDisplayTitle());
  //  else
  //    printf("There was no song before\n");
  int item_to_select = 0;
  if (before && show)
    for(int i = 0 ; i < itemcount ; i ++)
      if (show[i]==before)
	{
	  item_to_select = i;
	  break;
	}
  // set the focus
  //  printf("The new item to select is %d\n",item_to_select);
  target->setCurrentItem(item_to_select);
  if (item_to_select < itemcount && item_to_select >=0)
    QSong::set_selected(item_to_select,true);
  target->ensureItemVisible(item_to_select);
  //  printf("The new first item is selected ? %d\n",QSong::get_selected(0));
  // inform the target that the vector has changed
  target->vectorChanged();
  //  printf("The new focus is at %d\n\n",target->currentItem());
  //  printf("The vectorview thinks that first item isSelected: %d\n\n",target->isSelected(0));
  return itemcount;
}

static int itemUpdateingCount = 0;
int DataBase::getSelection(SongSelectorLogic* selector, Song* main, QVectorView* target, int count)
{
  // only when we have a dcolor limitation can we use the amount limitation
  if (!Config::limit_indistance || count==0 || main==NULL) return get_unheaped_selection(selector, main, target);
  assert(count>0);
  
  // to get an appropriate selection we allocate the nessary vector
  itemUpdateingCount++;
  updateCache(selector);
  Song * * show = bpmdj_allocate(count,Song*);
  SongHeap heap(count);
  if (Config::limit_indistance) heap.maximum = 1.0;
  for(int i = 0; i < cache.count ; i ++)
    {
      Song *song = cache.elements[i];
      bool vis = filter(selector,song,main, heap.maximum);
      if (vis)
	heap.add(song);
    }
  int itemcount = heap.copy_to(show);
  assert(itemcount<=count);
  itemUpdateingCount--;
  return set_answer(show,itemcount,target);
}

Song * * DataBase::closestSongs(SongSelectorLogic * selector,
				Song * target1, float weight1,
				Song * target2, float weight2,
				SongMetriek * metriek, int maximum, int &count)
{
  int i, j;
  float * minima = bpmdj_allocate(maximum,float);
  Song * * entries = bpmdj_allocate(maximum,Song*);
  count = 0;
  for(i = 0 ; i < maximum ; i++)
    {
      entries[i]=NULL;
      minima[i]=-1;
    }
  updateCache(selector);
  for(i = 0 ; i < cache.count; i ++)
    {
      Song * song = cache.elements[i];
      // standard checks: tags completed and ondisk 
      if (!song->get_ondisk()) continue;
      if (!tagFilter(song)) continue;
      // measure distance, gegeven de metriek
      double d = song->distance(target1,weight1,target2,weight2,metriek);
      // find the best position to insert the item..
      for (j = 0 ; j < count ; j++)
	if (minima[j]>d) 
	  break;
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
	  entries[j]=song;
	}
    }
  return entries;
}
