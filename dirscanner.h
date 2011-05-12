/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__dirscanner_h__
#define __loaded__dirscanner_h__
using namespace std;
#line 1 "dirscanner.h++"
#include <dirent.h>
#include <stdlib.h>
#include <qstring.h>
#include <stack>
#include "types.h"

/**
 * The directory scanner provides the possibility to scan a directory tree 
 * in specific chunk sizes. For each filename it will check its extension,
 * and if it matches it will call the checkFile virtual function.
 */
class DirectoryScanner
{
private:
  /**
   * A subclass used to track in which directory we currently are. The stack of 
   * open directories is maintained in dir_stack. Each frame e on this stack 
   * contains an opened directory entry (so the readdir function can be called)
   * and the full path to reach that directory.
   */
  class DirPos
  {
  public:
    DIR * dir;
    QString path;
  };
  /**
   * The stack of open directory entries. Since we can a number of files per 
   * turn and then continue later on we need this.
   */
  stack<DirPos> dir_stack; 
  /**
   * How many files will we scan per scan() call ?
   */
  unsigned4 files_per_turn;
private:
  /**
   * Called internally to check a specific filename. If the filename has the 
   * proper extension, checkFile will be called
   */
  void scanfile(const QString dir, const QString filename);
protected:
  /**
   * Should we recurse into sub directories
   */
  bool recurse;
  /**
   * The length of the initial directory. This can be used in check_file to 
   * strip the common root from the files that are being checked
   */
  int prefix_length;
  /**
   * The required extension before a file can be send to checkFile
   */
  QString required_extension;
  virtual void recursing(const QString dir);
  /**
   * Called for each file that has a proper extension.
   */
  virtual void checkfile(const QString dir, const QString filename) = 0;
  /**
   * The function used to check whether the extension matches
   * If the extension does not match the file is not sent to checkfile.
   * The standard function will check the required_extension.
   */
  virtual bool matchextension(const QString filename);
  virtual ~DirectoryScanner()
  {
  };
public:
  /**
   * Will create a directory scanner which will target the required extension.
   * To use it first reset it to a certain directory and then use scan()
   */
  DirectoryScanner(QString extension);
  /**
   * Creates a directory scanner that will target the given extension and start
   * searching for files under @arg dir. Normally recursion is enabled. Use the
   * last parameter to turn it off if necessary
   */
  DirectoryScanner(QString dir, QString extension, bool rec=true);
  /**
   * Will reset the scanner into a certain directory. Normally all files will
   * be scanned at once. If necessary to use smaller chunk sizes use the 
   * files_per_turn argument.
   */
  virtual void reset(const QString dirname, 
		     unsigned4 files_per_turn=0xffffffff);
  static bool goodName(QString name);
  void set_file_per_turn(unsigned4 fpt)
    {
      files_per_turn=fpt;
    }
  /**
   * Will scan a collection of files. It returns the number of files
   * found, which is not the same as the number of satisfactory files found.
   * If this function returns 0, then the process has ended.
   */
  virtual unsigned4 scan();
};
#endif // __loaded__dirscanner_h__
