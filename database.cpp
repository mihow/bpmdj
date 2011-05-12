/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "songselector.logic.h"
#include "process-manager.h"
#include "songtree.h"
#include "avltree.cpp"
#include "growing-array.cpp"
#include "tags.h"
#include "heap.h"

/**
 * The database cache contains items that are visible when only considereing the taglist
 */

DataBase::DataBase() : all(), cache()
{
  init();
}

DataBase::~DataBase()
{
  clear();
}

void DataBase::reset()
{
  clear();
  init();
}

void DataBase::init()
{
  all.reset();
  cache.reset();
  and_include = NULL;
  or_include = NULL;
  exclude = NULL;
  tag = NULL;
  tag_size = 0;
  file_tree = new AvlTree<QString>();
}

void DataBase::clear()
{
  for(int i = 0 ; i < all.count ; i ++)
    delete all.elements[i];
  all.reset();
  cache.reset();
  deallocate(and_include);
  deallocate(or_include);
  deallocate(exclude);
  delete[] tag;
  delete file_tree;
}

void DataBase::add(Song* song)
{
  all.add(song);
  if (file_tree->search(song->get_file()))
    {
      printf("Fatal: The song %s occurs at least two times in different index files\n",(const char*)song->get_file());
      printf("The first index file is %s\n",(const char*)song->get_storedin());
      song = ((SongSortedByFile*)(file_tree->search(song->get_file())))->song;
      printf("The second index file is %s\n",(const char*)song->get_storedin());
      exit(0);
    }
  file_tree->add(new SongSortedByFile(song));
}

Song * DataBase::find(QString song_filename)
{
  SongSortedByFile* ssbf = (SongSortedByFile*)file_tree->search(song_filename);
  if (!ssbf) return NULL;
  return ssbf->song;
}

bool DataBase::cacheValid(SongSelectorLogic * selector)
{
  if (selector->tagList->childCount() != tag_size)
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
  if (and_include) deallocate(and_include);
  if (or_include) deallocate(or_include);
  if (exclude) deallocate(exclude);
  if (tag) delete[](tag);
  tag_size    = selector->tagList->childCount();
  and_include = allocate(tag_size,bool);
  or_include  = allocate(tag_size,bool);
  exclude     = allocate(tag_size,bool);
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
  if (main!=NULL && main->get_author()==item->get_author())
    item->set_played_author_at_time(Played::songs_played);
  // song on disk ?
  if (Config::get_limit_ondisk() && !item->get_ondisk())
    return false;
  // song played ?
  if (Config::get_limit_nonplayed() && item->get_played())
    return false;
  // okay, no similar authors please..
  if (Config::get_limit_authornonplayed() && 
      Played::songs_played - item->get_played_author_at_time() < Config::get_authorDecay())
    return false;
  // now check the tempo stuff
  if (main && (Config::get_limit_uprange() || Config::get_limit_downrange()))
    if (!item->tempo_show(main,
			  Config::get_limit_uprange(),
			  Config::get_limit_downrange())) return false;
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
  if (Config::get_limit_indistance() && !indistance)
    return false;
  return true;
}

AvlTree<QString>* DataBase::getFileTreeCopy()
{
  AvlTree<QString> * file_tree = new AvlTree<QString>();
  for(int i = 0 ; i < all.count ; i ++)
    if (!file_tree->search(all.elements[i]->get_file()))
      file_tree->add(new SongSortedByFile(all.elements[i]));
  return file_tree;
}

AvlTree<QString>* DataBase::getFileTreeRef()
{
  return file_tree;
}

int DataBase::get_unheaped_selection(SongSelectorLogic* selector, Song* main, QVectorView* target)
{
  // to get an appropriate selection we allocate the nessary vector
  int itemcount=0;
  updateCache(selector);
  Song * * show = allocate(cache.count,Song*);
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
    show = reallocate(show, itemcount, Song*);
  else
    {
      show = NULL;
      deallocate(show);
    }
  QSong::setVector(show,itemcount);
  target->vectorChanged();
  return itemcount;
}

int DataBase::getSelection(SongSelectorLogic* selector, Song* main, QVectorView* target, int count)
{
  if (count==0 || main==NULL) return get_unheaped_selection(selector, main, target);
  
  // to get an appropriate selection we allocate the nessary vector
  updateCache(selector);
  Song * * show = allocate(count,Song*);
  SongHeap heap(count);
  if (Config::get_limit_indistance()) heap.maximum = 1.0;
  for(int i = 0; i < cache.count ; i ++)
    {
      Song *song = cache.elements[i];
      bool vis = filter(selector,song,main, heap.maximum);
      if (vis)
	heap.add(song);
    }
  int itemcount = heap.copy_to(show);
  assert(itemcount<=count);
  return set_answer(show,itemcount,target);
}

Song * * DataBase::closestSongs(SongSelectorLogic * selector, Song * target, SongMetriek * metriek, int maximum, int &count)
{
  int i, j;
  float * minima = allocate(maximum,float);
  Song * * entries = allocate(maximum,Song*);
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
      double d = song->distance(target,metriek,100000);
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
