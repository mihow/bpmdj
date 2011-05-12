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

#include <dirent.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <stdlib.h>
#include "setupwizard.h"
#include "songselector.logic.h"
#include "kbpm-played.h"
#include "kbpm-index.h"

QApplication *app;

int main(int argc, char* argv[])
{
  SongIndex *songIndex;
  app = new QApplication(argc,argv);
  SongSelectorLogic test;
  app->setMainWidget(&test);
  // are the two necessary directories available ?
  DIR * mdir;
  DIR * idir;
  do
    {
      mdir = opendir(MusicDir);
      idir = opendir(IndexDir);
      if (mdir == NULL || idir == NULL)
	{
	  SetupWizard *sw = new SetupWizard(0,0,true);
	  sw->setFinishEnabled(sw->lastpage,true);
	  int res = sw->exec();
	  if (res == QDialog::Rejected)
	    exit(0);
	}
    }
  while (mdir==NULL || idir==NULL);
  // read already played indices
  new Played("played.log");
  // read the configuration
  Config::load();
  // create the index in memory
  songIndex=new SongIndex(test.songList);
  Config::file_count=songIndex->total_files;
  delete(songIndex);
  // put it in the dataview
  test.findAllTags();
  // start the test app
  test.show();
  int result = app->exec();
  Config::save();
  return result;
}
