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

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "dirscanner.h"

DirectoryScanner::DirectoryScanner(QString  extension)
{ 
  required_extension=extension; 
};

bool DirectoryScanner::exists(const QString  fn)
{
   FILE * f = fopen(fn,"rb");
   if (f)
     {
	fclose(f);
	return true;
     }
   return false;
}

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
  while (entry=readdir(dir))
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

