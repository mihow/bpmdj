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

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "dirscanner.h"
#include "common.h"

bool DirectoryScanner::songExtension(QString ext)
{
  return goodExtension(ext.lower());
}

bool DirectoryScanner::goodExtension(QString ext)
{
  // important !!!
  // opgelet met extenties langer dan 4 tekens !!!
  return (ext.endsWith(".mp3") || 
	  ext.endsWith(".ogg") || 
	  ext.endsWith(".flac"));
}

bool DirectoryScanner::goodName(QString name)
{
  // must end on .idx or a song extension
  if (!goodExtension(name) && name.right(4)!=".idx")
    return false;
  // no spaces or any special characters
  // should occur anywhere in the filename
  if (name.contains(" ")) return false;
  if (name.contains("_")>1) return false;
  if (name.contains("_")==1)
    {
      int pos = name.findRev("_");
      int pos2 = name.find("[");
      if ((pos2-pos>3) || (pos<2))
	return false;
    }
  if (name.contains("'")) return false;
  if (name.contains("-")) return false;
  if (name.contains("[")!=1) return false;
  if (name.contains("]")!=1) return false;
  int leftbrace=name.find("[");
  if (leftbrace>name.find("]")) return false;
  if (name.at(leftbrace+1).isLetter())
    {
      char c = name.at(leftbrace+1).latin1();
      if (c<'A' || c>'Z')
	return false;
    }
  return true;
}


DirectoryScanner::DirectoryScanner(QString  extension)
{ 
  required_extension=extension; 
};

void DirectoryScanner::recursing(const QString dirname)
{
  printf("Recursing into %s\n",(const char*)dirname);
}

void DirectoryScanner::scandir(const QString dirname, const QString prefix)
{
  struct dirent * entry;
  DIR * dir=opendir(dirname);
  if (!dir) return;
  recursing(dirname);
  while ( (entry=readdir(dir)) )
    {
      if (strcmp(entry->d_name,".")==0 ||
	  strcmp(entry->d_name,"..")==0) continue;
      QString newprefix;
      if (prefix == QString::null)
	newprefix=entry->d_name;
      else
	newprefix=prefix+"/"+entry->d_name;
      QString newdir = dirname +"/"+entry->d_name;
      scandir(newdir,newprefix);
      scanfile(prefix,entry->d_name);
    }
  closedir(dir);
}

bool DirectoryScanner::matchextension(const QString filename)
{
  if (!required_extension)
    return true;
  if (strlen(filename)<strlen(required_extension))
    return false;
  return filename.right(required_extension.length()).contains(required_extension,0);
}

void DirectoryScanner::scanfile(const QString  fullname, const QString  filename)
{
  if (matchextension(filename))
    checkfile(fullname,filename);
}

