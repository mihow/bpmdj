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

#include <qlabel.h>
#include "database.h"
#include "qsong.h"
#include "songselector.logic.h"
#include "kbpm-played.h"
#include "process-manager.h"
#include "songtree.h"
#include "avltree.cpp"

DataBase::DataBase()
{
  all_size = 1;
  all_count = 0;
  all = allocate(all_size,Song*);
  cache = NULL;
  clearCache();
  and_include = NULL;
  or_include = NULL;
  exclude = NULL;
  tag = NULL;
  tag_size = 0;
}

void DataBase::add(Song* song)
{
  if (all_count==all_size)
    {
      all_size *= 2;
      all = reallocate(all,all_size,Song*);
    }
  all[all_count++]=song;
}

void DataBase::addToCache(Song * song)
{
  if (cache_count==cache_size)
    {
      cache_size*=2;
      cache = reallocate(cache,cache_size,Song*);
    }
  cache[cache_count++]=song;
}

void DataBase::clearCache()
{
  if (cache)
    free(cache);
  cache_size = 1;
  cache_count = 0;
  cache = allocate(cache_size,Song*);
}

bool DataBase::cacheValid(SongSelectorLogic * selector)
{
  if (selector->nextTagLine != tag_size)
    return false;
  for (int i=0; i < tag_size; i++)
    if (selector->tagLines[i]->text()!=*(tag[i]) ||
	selector->tagAndInclude[i]->isChecked()!=and_include[i] ||
	selector->tagOrInclude[i]->isChecked()!=or_include[i] ||
	selector->tagExclude[i]->isChecked()!=exclude[i])
      return false;
  return true;
}

void DataBase::copyTags(SongSelectorLogic * selector)
{
  if (and_include) free(and_include);
  if (or_include) free(or_include);
  if (exclude) free(exclude);
  if (tag) delete[](tag);
  tag_size = selector->nextTagLine;
  and_include = allocate(tag_size,bool);
  or_include = allocate(tag_size,bool);
  exclude = allocate(tag_size,bool);
  tag = new QString[tag_size]();
  and_includes_checked = false;
  excludes_checked = false;
  for (int i=0; i < tag_size; i++)
    {
      tag[i] = selector->tagLines[i]->text();
      or_include[i] = selector->tagOrInclude[i]->isChecked();
      and_includes_checked |= and_include[i] = selector->tagAndInclude[i]->isChecked();
      excludes_checked     |= exclude[i] = selector->tagExclude[i]->isChecked();
    }
}

bool DataBase::tagFilter(Song* item)
{
  bool matched = false;
  // first we check the or_include tags
  for (int i = 0; !matched && i < tag_size; i++)
    if (or_include[i])
      if (item->containsTag(tag[i]))
	matched=true;
  if (!matched) return false;
  // now we check the and_include tags
  if (and_includes_checked)
    for(int i = 0; i < tag_size ; i ++)
      if(and_include[i])
	if (!item->containsTag(tag[i]))
	  return false;
  // now we check the exclusion tags
  if (excludes_checked)
    for (int i=0; i < tag_size; i++)
      if (exclude[i])
	if (item->containsTag(tag[i]))
	  return false;
  return matched;
}

void DataBase::updateCache(SongSelectorLogic* selector)
{
  if (cacheValid(selector))
    return;
  copyTags(selector);
  clearCache();
  for(int i = 0 ; i < all_count ; i ++)
    {
      Song * song = all[i];
      bool vis = tagFilter(song);
      if (vis) addToCache(song);
    }
}

/* WVB -- the thing below can be seriously optimized
 * 2 - get all flags and pass them in one flag
 * 3 - move the getDistance to a later moment
 * 4 - make the song to work internally with floats/doubles instead of strings
 * 5 - remove the maintempo flag.. I don't think its usefull
 */
bool DataBase::filter(SongSelectorLogic* selector, Song *item, Song* main)
{
  if (main!=NULL && main->author==item->author)
    item->played_author_at_time=Played::songs_played;
  // we must obtain the distance first because this value is updated on the fly
  bool indistance = item->getDistance();
  if (Config::limit_indistance && !indistance)
    return false;
  // song on disk ?
  if (Config::limit_ondisk && !item->ondisk)
    return false;
  // song played ?
  if (Config::limit_nonplayed && item->played)
    return false;
  // okay, no similar authors please..
  if (Config::limit_authornonplayed && 
      Played::songs_played - item->played_author_at_time < Config::authorDecay)
    return false;
  // tempo 
  if (item->tempo && (Config::limit_uprange || Config::limit_downrange))
    {
      double t=atof(item->tempo);
      double mt=0;
      if (main) mt=atof(main->tempo);
      if (!Config::limit_downrange)
	if (t<mt) return false;
      if (!Config::limit_uprange)
	if (t>mt) return false;
      if (main && main->tempo_distance(item)>1)
	return false;
    }
  // search
  const QString lookingfor = selector -> searchLine -> text();
  if (strcmp(lookingfor,"")!=0)
    {
      QString title=item->title.upper();
      QString author=item->author.upper();
      if (title.contains(lookingfor)+author.contains(lookingfor)==0)
	return false;
    }
  return true;
}

AvlTree<QString>* DataBase::getFileTree()
{
  AvlTree<QString> * tree = new AvlTree<QString>();
  for(int i = 0 ; i < all_count ; i ++)
    if (!tree->search(all[i]->file))
      tree->add(new SongSortedByFile(all[i]));
  return tree;
}

int DataBase::getSelection(SongSelectorLogic* selector, Song* main, QListView* target)
{
  int itemcount=0;
  updateCache(selector);
  for(int i = 0; i < cache_count ; i ++)
    {
      Song *song = cache[i];
      bool vis = filter(selector,song,main);
      if (vis)
	{
	  new QSong(song,target);
	  itemcount++;
	}
    }
  return itemcount;
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
  for(i = 0 ; i < cache_count; i ++)
    {
      Song * song = cache[i];
      // standard checks: tags completed and ondisk 
      if (!song->ondisk) continue;
      if (!tagFilter(song)) continue;
      // measure distance, gegeven de metriek
      double d = song->distance(target,metriek);
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

void DataBase::del(Song* which)
{
  for (int i = 0 ; i < all_count ; i ++)
    {
      if (all[i]==which)
	{
	  while(i<all_count - 1)
	    {
	      all[i]=all[i+1];
	      i++;
	    }
	  all_count -- ;
	  break;
	}
    }
  for (int i = 0 ; i < cache_count ; i ++)
    {
      if (cache[i]==which)
	{
	  while(i<cache_count - 1)
	    {
	      cache[i]=cache[i+1];
	      i++;
	    }
	  cache_count -- ;
	  break;
	}
    }
}
