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

#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qprogressbar.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include "index-reader.h"
#include "kbpm-dj.h"
#include "scripts.h"
#include "songselector.logic.h"
#include "songtree.h"
#include "compacter.h"
#include "tags.h"

#define SPLIT_ONEFILE 0
#define SPLIT_ARTIST 1
#define SPLIT_TAGS 2

void SongSelectorLogic::compactIdxDirectory()
{
  // obtain information how to split all information
  int type = -1;
  CompactDialog compacter(NULL,NULL,TRUE);
  if (compacter.exec()==compacter.Rejected) return;
  if (compacter.onebigfile->isChecked()) type = SPLIT_ONEFILE;
  else if (compacter.artist->isChecked()) type = SPLIT_ARTIST;
  else if (compacter.tags->isChecked()) type = SPLIT_TAGS;
  else assert(0);
  // check existense of new_index...
  if (exists("./new_index"))
    {
      if (QMessageBox::question(this,"New Index Directory Exists","The new index directory ./new_index/ already exists.\nRemove ?",
				QMessageBox::Yes, QMessageBox::Abort) != QMessageBox::Yes) return;
      if (!execute("rm -fr ./new_index/"))
	{
	  QMessageBox::critical(this,"Delete Failed","Could not delete the directory ./new_index/",QMessageBox::Abort,0);
	  return;
	}
    }
  execute("mkdir ./new_index/");
  // fetch all songs
  GrowingArray<Song*> * all = database->getAllSongs();
  assert(all->count>=0);
  // create a tree containing the different filenames
  AvlTree<QString> * tree = new AvlTree<QString>();
  if (type == SPLIT_ONEFILE)
    tree->add(new QStringNode(compacter.filename->text()));
  else
    for (int i = 0 ; i < all->count ; i ++)
      {
	Song * song = all->elements[i];
	switch(type)
	  {
	  case SPLIT_ARTIST:
	    if (!tree->search(song->get_author()))
	      tree->add(new QStringNode(song->get_author()));
	    break;
	  case SPLIT_TAGS:
	    if (!tree->search(Tags::full_string(song->get_tags())))
	      tree->add(new QStringNode(Tags::full_string(song->get_tags())));
	    break;
	  }
      }
  // dump everything to the ./new_index/ directory..
  compacter.ok->setEnabled(false);
  compacter.cancel->setEnabled(false);
  compacter.buttons->setEnabled(false);
  compacter.show();
  QStringNode * filetocreate;
  int progress = 0;
  while ( (filetocreate = (QStringNode*)tree->top()) )
    {
      QString key =  filetocreate->content;
      tree->del(key);
      QString qfilename = QString("new_index/")+key+".bib";
      if (key.isEmpty()) 
	qfilename = QString("new_index/_empty_.bib");
      const char* filename = qfilename;
      compacter.fileinfo->setText(qfilename);
      FILE * target = fopen(filename,"wb");
      if (!target)
	{
	  QMessageBox::critical(this,QString("Compaction failed"),
				QString("The file\n") +QString(filename)+ QString("\ncould not be written !"),
				QMessageBox::Abort,0);
	  return;
	}
      app->processEvents();
      assert(target);
      for(int i = 0 ; i < all->count ; i ++)
	{
	  Song * song = all->elements[i];
	  bool handlethisone = false;
	  switch (type)
	    {
	    case SPLIT_ARTIST:
	      handlethisone = song->get_author() == key;
	      break;
	    case SPLIT_TAGS:
	      handlethisone = Tags::full_string(song->get_tags()) == key;
	      break;
	    case SPLIT_ONEFILE:
	      handlethisone=true;
	    }
	  if (handlethisone)
	    {
	      QString indexfilename = song->get_storedin();
	      Index index(indexfilename);
	      if (index.can_be_stored_in_bib())
		{
		  index.write_bib_field(target);
		}
	      else
		{
		  QString withoutindexprefix = indexfilename.remove("./index");
		  QString withnewprefix = QString("./new_index")+withoutindexprefix;
		  QString targetdirectory = withnewprefix.left(withnewprefix.findRev("/"));
		  vexecute(MKDIR"\"%s\"",(const char*)targetdirectory);
		  index.write_idx((const char*)withnewprefix);
		}
	      if (all->count>0 && (++progress)%(all->count/100) == 0)
		{
		  compacter.progress->setProgress(progress,all->count);
		  app->processEvents();
		}
	    }
	}
      fclose(target);
    }
  if (progress!=all->count)
    {
      QMessageBox::critical(this,QString("Compaction failed"),
			    QString("From the ")+QString::number(all->count)+" files\n"
			    "only "+QString::number(progress)+" have been written !",
			    QMessageBox::Abort,0);
      return;
    }
  if (!execute("rm -fr ./index/"))
    {
      QMessageBox::critical(this,"Delete Failed",
			    "Could not delete the directory ./index\n"
			    "Make sure to delete it MANUALLY and\n"
			    "rename the ./new_index directory to ./index",QMessageBox::Abort,0);
      return;
    }
  
  if (!execute(MV" ./new_index ./index"))
    {
      QMessageBox::critical(this,"Move failed",
			    "The ./new_index directory contains all your data.\n"
			    "However, somehow I could not rename it to ./index\n"
			    "Please do this manually.",QMessageBox::Abort,0);
      return;
    }
  // rereading. The index-reader will automatically update all existing Song* pointers..
  // wo we don't care about them
  compacter.prompt->setText("Rereading");
  app->processEvents();
  IndexReader * indexReader = new IndexReader(compacter.progress,compacter.fileinfo,database);
  delete indexReader;
  compacter.prompt->setText("Finishing");
  app->processEvents();
  findAllAlbums(); // necessary to update the album pointers and the view itself...
}
