/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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
#include "history.h"
#include "index-reader.h"
#include "version.h"
#include "dirscanner.h"
#include "scripts.h"
#include "loader.h"
#include "tags.h"

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
      number++;
  }
public:
  QString result;
  bool entered;
  int number;
  RawScanner() : DirectoryScanner(RAW_EXT) 
  {
    result = QString::null;
    entered=false;
    number = 0;
  };
  virtual void scan()
  { 
    DirectoryScanner::scan(Config::tmp_directory,NULL); 
  };
};

int main(int argc, char* argv[])
{
  Tags::init();

  printf("Loading  bpmdj v"VERSION"\n"); fflush(stdout);
  QApplication application(argc,argv);
  SongSelectorLogic main_window;
  app = &application;
  
  Loader * loader = new Loader(NULL,NULL,true,Qt::WStyle_Customize | Qt::WStyle_NoBorder);
  loader -> show();
  
  // 1.a first check the availability of a number of directories...
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
  if(!(execute("kbpm-play --check-version "VERSION" 2>/dev/null >/dev/null")))
    {
      QMessageBox::warning(NULL,"Missing kbpm-play",
			   "Could not start kbpm-play.\n" 
			   "Make sure the correct version of the\n"
			   "program is installed in your PATH");
      exit(0);
    }
  
  // 2. read the configuration
  Config::load();
  
  // 1.c checking left over raw files (deze komt laatst omdat tmp_directory het kuiste pad bevat)
  RawScanner raw;
  raw.scan();
  if (!raw.result.isNull())
    if (QMessageBox::warning(NULL,RAW_EXT " files check",
			     "There are some left over "RAW_EXT" files. These are:\n"+raw.result,
			     "Remove", "Ignore", 0, 0, 1)==0)
      removeAllRaw(Config::tmp_directory);  
  
  // 3. read all the files in memory
  main_window.initialize_using_config();
  application.setMainWidget(&main_window);
  // read already played indices
  new Played("played.log");
  
  // create the index in memory
  loader->progressBar1->setEnabled(true);
  IndexReader * indexReader = new IndexReader(loader->progressBar1, loader->readingDir ,main_window.database);
  Config::file_count=indexReader->total_files;
  delete indexReader;
  indexReader = NULL;
  
  // 4. Retrieve tags and spectra
  application.processEvents();
  main_window.findAllAlbums();
  
  delete loader;
  loader = NULL;
  
  // 5. Some extra version dependent blurb...
  if (!Config::shown_aboutbox)
    {
      if (MAGIC_NOW == MAGIC_2_1)
	QMessageBox::message(NULL,"The player now supports OSS and ALSA, together\n"
			     "with an extra (obligatory) parameter to specify where the\n"
			     ".raw files are to be stored. Therefore, please go to\n"
			     "Preferences and create suitable player commands");
      if (MAGIC_NOW == MAGIC_2_5)
	QMessageBox::message(NULL,"The selector has now a much better spectrum-matching\n"
			     "algorithm. Please go to Preferences|spectrum to revert it\n"
			     "back to the old (lesser accurate) version.\n"
			     "Also, be sure to look at the fragment sequencer of the player !\n");
    }
  
  // 5b. startup mixers ?
  if (Config::open_mixer)
    main_window.openMixer();
  
  // 6. start the application
  main_window.show();
  int result = application.exec();
  if (!Config::shown_aboutbox)
    {
      Config::shown_aboutbox=true;
      QMessageBox::message(NULL,"If you like this software please consider putting a\n"
			   "link to http://bpmdj.sourceforge.net/ on your webpage\n"
			   "(together with some nice words of course :)");
    }
  Config::save();
  return result;
}
