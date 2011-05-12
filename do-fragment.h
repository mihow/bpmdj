/****
Data Object compiled file
Copyright (C) 2006-2010 Werner Van Belle
Do not change. Changes might be lost
------------------------------------------

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
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
****/

#ifndef __DO_FRAGMENT_H
#define __DO_FRAGMENT_H
#include <typeinfo>
#include "data-reference-count.h"
#include "song.h"
#include "stereo-sample2.h"
#include "memory.h"
class FragmentInMemory;
class FragmentInMemoryData;
class FragmentFile;
class FragmentFileData;

//-------------------------------------
// Data Meta Definition 
//-------------------------------------
class FragmentInMemory: public DataSmart<FragmentInMemoryData>
{
  public: template <class SmartRefChild> FragmentInMemory(SmartRefChild& other)
    {
    other.incref();
    FragmentInMemoryData* target = dynamic_cast<FragmentInMemoryData*>(other.ptr);
    assert(target);
    ptr=target;
    }
  public: FragmentInMemory(FragmentInMemoryData * other):
    DataSmart<FragmentInMemoryData>(other)
    {
    }
  public: stereo_sample2* get_samples();
  public: void set_samples(stereo_sample2*);
  public: unsigned4 get_size();
  public: void set_size(unsigned4);
  public: FragmentInMemory();
  public: FragmentInMemory(QString filename);
};

class FragmentFile: public DataSmart<FragmentFileData>
{
  public: template <class SmartRefChild> FragmentFile(SmartRefChild& other)
    {
    other.incref();
    FragmentFileData* target = dynamic_cast<FragmentFileData*>(other.ptr);
    assert(target);
    ptr=target;
    }
  public: FragmentFile(FragmentFileData * other):
    DataSmart<FragmentFileData>(other)
    {
    }
  public: QString get_f();
  public: void set_f(QString);
  public: Song* get_song();
  public: void set_song(Song*);
  public: bool isEmpty();
  public: FragmentFile();
  public: FragmentFile(Song* s, QString f);
  public: FragmentInMemory getFragment();
};


//-------------------------------------
// Data Object Definition
//-------------------------------------
class FragmentInMemoryData: public DataReferenceCount
{
  friend class FragmentInMemory;
  public: virtual ~FragmentInMemoryData();
  public: virtual DataReferenceCount* clone();
  protected: stereo_sample2* samples;
  public: stereo_sample2* get_samples();
  public: void set_samples(stereo_sample2*);
  protected: unsigned4 size;
  public: unsigned4 get_size();
  public: void set_size(unsigned4);
  protected: FragmentInMemoryData();
  protected: FragmentInMemoryData(QString filename);
};

class FragmentFileData: public DataReferenceCount
{
  friend class FragmentFile;
  public: virtual ~FragmentFileData();
  public: virtual DataReferenceCount* clone();
  protected: QString f;
  public: QString get_f();
  public: void set_f(QString);
  protected: Song* song;
  public: Song* get_song();
  public: void set_song(Song*);
  public: bool isEmpty();
  protected: FragmentFileData();
  protected: FragmentFileData(Song* s, QString f);
  public: FragmentInMemory getFragment();
};


//-------------------------------------
// Data Methods
//-------------------------------------
inline DataReferenceCount* FragmentInMemoryData::clone()
  {
  return new FragmentInMemoryData(*this);
  }

inline stereo_sample2* FragmentInMemoryData::get_samples()
  {
  return samples;
  }

inline void FragmentInMemoryData::set_samples(stereo_sample2* __arg)
  {
  samples=__arg;
  }

inline stereo_sample2* FragmentInMemory::get_samples()
  {
  return ptr->get_samples();
  }

inline void FragmentInMemory::set_samples(stereo_sample2* __arg)
  {
  ptr->set_samples(__arg);
  }

inline unsigned4 FragmentInMemoryData::get_size()
  {
  return size;
  }

inline void FragmentInMemoryData::set_size(unsigned4 __arg)
  {
  size=__arg;
  }

inline unsigned4 FragmentInMemory::get_size()
  {
  return ptr->get_size();
  }

inline void FragmentInMemory::set_size(unsigned4 __arg)
  {
  ptr->set_size(__arg);
  }

inline FragmentInMemory::FragmentInMemory():
  DataSmart<FragmentInMemoryData>(new FragmentInMemoryData())
  {
  }

inline FragmentInMemory::FragmentInMemory(QString filename):
  DataSmart<FragmentInMemoryData>(new FragmentInMemoryData(filename))
  {
  }

inline DataReferenceCount* FragmentFileData::clone()
  {
  return new FragmentFileData(*this);
  }

inline QString FragmentFileData::get_f()
  {
  return f;
  }

inline void FragmentFileData::set_f(QString __arg)
  {
  f=__arg;
  }

inline QString FragmentFile::get_f()
  {
  return ptr->get_f();
  }

inline void FragmentFile::set_f(QString __arg)
  {
  ptr->set_f(__arg);
  }

inline Song* FragmentFileData::get_song()
  {
  return song;
  }

inline void FragmentFileData::set_song(Song* __arg)
  {
  song=__arg;
  }

inline Song* FragmentFile::get_song()
  {
  return ptr->get_song();
  }

inline void FragmentFile::set_song(Song* __arg)
  {
  ptr->set_song(__arg);
  }

inline bool FragmentFile::isEmpty()
  {
  return ptr->isEmpty();
  }

inline FragmentFile::FragmentFile():
  DataSmart<FragmentFileData>(new FragmentFileData())
  {
  }

inline FragmentFile::FragmentFile(Song* s, QString f):
  DataSmart<FragmentFileData>(new FragmentFileData(s, f))
  {
  }

inline FragmentInMemory FragmentFile::getFragment()
  {
  return ptr->getFragment();
  }

#endif // __DO_FRAGMENT_H
