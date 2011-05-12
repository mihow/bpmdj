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
#ifndef __loaded__dirscanner_cpp__
#define __loaded__dirscanner_cpp__
using namespace std;
#line 1 "dirscanner.c++"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "dirscanner.h"
#include "common.h"
#include "capacity.h"
#include "constants.h"
#include "info.h"

bool DirectoryScanner::goodName(QString name)
{
  // must end on .idx or a song extension
  if (!goodExtension(name) && !name.endsWith(IdxExt))
    return false;
  // no spaces or any special characters
  // should occur anywhere in the filename
  if (name.contains(OneSpace)) return false;
  if (name.count("_")>1) return false;
  if (name.contains("_"))
    {
      int pos = name.lastIndexOf("_");
      int pos2 = name.indexOf("[");
      if ((pos2-pos>3) || (pos<2))
	return false;
    }
  if (name.contains("'")) return false;
  if (name.contains("-")) return false;
  if (name.contains("[")!=1) return false;
  if (name.contains("]")!=1) return false;
  int leftbrace=name.indexOf("[");
  if (leftbrace>name.indexOf("]")) return false;
  if (name.at(leftbrace+1).isLetter())
    {
      QChar c = name.at(leftbrace+1);
      if (c<'A' || c>'Z')
	return false;
    }
  return true;
}

DirectoryScanner::DirectoryScanner(QString extension):
  recurse(true),
  required_extension(extension)
{ 
};

DirectoryScanner::DirectoryScanner(QString dir, QString extension, bool rec):
  recurse(rec),
  required_extension(extension)
{
  reset(dir);
}

void DirectoryScanner::recursing(const QString dirname)
{
  Info("Recursing into %s",dirname.toAscii().data());
}

void DirectoryScanner::reset(const QString dirname, unsigned4 fpt)
{
  DirPos pos;
  pos.dir = opendir(dirname.toAscii().data());
  pos.path = dirname;
  prefix_length=dirname.length();
  dir_stack.push(pos);
  files_per_turn=fpt;
}

unsigned4 DirectoryScanner::scan()
{
  unsigned4 real_files=0;
  while(real_files < files_per_turn)
    {
      if (dir_stack.size()==0) 
	return real_files;
      DirPos dirpos = dir_stack.top();
      if (!dirpos.dir) dir_stack.pop();
      else
	{
	  struct dirent * entry=NULL;
	  while ( real_files<files_per_turn && (entry=readdir(dirpos.dir)))
	    {
	      if (strcmp(entry->d_name,".")==0 ||
		  strcmp(entry->d_name,"..")==0) continue;
	      QString d_name(entry->d_name);
	      QString newdir = dirpos.path+slash+d_name;
	      // is it a directory
	      DIR * nd = opendir(newdir.toAscii().data());
	      if (nd)
		{
		  if (!recurse) continue;
		  DirPos pos;
		  pos.dir = nd;
		  pos.path = newdir;
		  dir_stack.push(pos);
		  recursing(newdir);
		  break;
		}
	      else
		{
		  real_files++;
		  scanfile(dirpos.path,d_name);
		}
	    }
	  if (!entry)
	    {
	      closedir(dirpos.dir);
	      dir_stack.pop();
	    }
	}
    }
  return real_files;
}

bool DirectoryScanner::matchextension(const QString filename)
{
  if (required_extension.isEmpty()) return true;
  if (filename.length()<required_extension.length()) return false;
  return filename.right(required_extension.length()).
    contains(required_extension,Qt::CaseInsensitive);
}

void DirectoryScanner::scanfile(const QString dir, const QString filename)
{
  if (matchextension(filename))
    checkfile(dir,filename);
}
#endif // __loaded__dirscanner_cpp__
