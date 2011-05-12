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
#include <qlabel.h>
#include <stdlib.h>
#include <qprogressbar.h>
#include <qmessagebox.h>
#include "setupwizard.h"
#include "songselector.logic.h"
#include "kbpm-played.h"
#include "kbpm-index.h"
#include "version.h"
#include "dirscanner.h"

QApplication *app;

class RawScanner: public DirectoryScanner
{
protected:
  virtual void recursing(const QString  dirname)
  {
  }
  virtual void scandir(const QString  dirname, const QString  checkname)
  {
    if (entered) return;
    entered=true;
    DirectoryScanner::scandir(dirname,checkname);
  }
  virtual void checkfile(const QString  fullname, const QString  filename)
  {
      result+=QString::number(number)+". "+filename+"\n";
  }
public:
  QString result;
  bool entered;
  int number;
  RawScanner() : DirectoryScanner(".raw") 
  {
    result = QString::null;
    entered=false;
    number = 0;
  };
  virtual void scan()
  { 
    DirectoryScanner::scan(".",NULL); 
  };
};

int main(int argc, char* argv[])
{
  app = new QApplication(argc,argv);
  Loader * loader = new Loader();
  loader -> show();
  
  // 1.a first check the availability of a number of directories...
  loader->programs->setEnabled(true);
  app->processEvents();
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
  // 1.b check the availability of programs/the PATH evironment variabe...
  //     we only check the existence of kbpm-play to guide the user. 
  int value;
  while ((value=system("kbpm-play 2>/dev/null >/dev/null"))>256)
    {
      QMessageBox::warning(NULL,"Missing kbpm-play",
			   "Could not start kbpm-play.\n" 
			   "Make sure the correct version of the\n"
			   "program is installed in your PATH (err="+QString::number(value)+")");
      exit(0);
    }
  // 1.c checking left over .raw files
  RawScanner raw;
  raw.scan();
  if (!raw.result.isNull())
    if (QMessageBox::warning(NULL,".raw files check",
			     "There are some left over .raw files. These are:\n"+raw.result,
			     "Remove", "Ignore", 0, 0, 1)==0)
      system("rm -- *.raw");
  
  // 2. read the configuration
  loader->config->setEnabled(true);
  app->processEvents();
  Config::load();
  
  // 3. read all the files in memory
  SongIndex *songIndex;
  SongSelectorLogic test;
  app->setMainWidget(&test);
  // read already played indices
  new Played("played.log");
  // create the index in memory
  loader->loading->setEnabled(true);
  loader->progressBar1->setEnabled(true);
  loader->readingFile->setEnabled(true);
  songIndex=new SongIndex(loader,test.songList);
  Config::file_count=songIndex->total_files;
  delete(songIndex);
  
  // 4. Retrieve tags and spectra
  loader->scanning->setEnabled(true);
  app->processEvents();
  test.findAllTags();
  delete loader;
  // start the test app
  test.show();
  int result = app->exec();
  Config::save();
  return result;
}
