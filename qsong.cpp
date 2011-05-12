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
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "qstring-factory.h"
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "kbpm-played.h"
#include "dirscanner.h"
#include "spectrum.h"

#define LIST_DCOLOR 2
#define LIST_TITLE 5
#define LIST_AUTHOR 6
#define LIST_TEMPO 0
#define LIST_TIME 3
#define LIST_CUES 4
#define LIST_VERSION 7
#define LIST_TAGS 8
#define LIST_ONDISK 9
#define LIST_SPECTRUM 1
#define LIST_INDEX 10
#define LIST_MD5SUM 11
#define LIST_FILE 12


extern "C"
{
#include "cbpm-index.h"
}

void QSong::reread()
{
  index_read((const char*)song_index);
  /* when the index changes immediately (new version and so on)
   * write it out again
   */
  if (index_changed)
    index_write();
  /* copy everything to object */
  song_tempo = index_tempo;
  song_file = index_file;
  song_tags = index_tags;
  song_time = index_time;
  song_md5sum = index_md5sum;
  song_spectrum = index_spectrum;
  /* are there any cues stored */
  has_cues = index_cue_z + index_cue_x + index_cue_c + index_cue_v;
  /* free all */
  index_free();
  /* try to open the song */
  QString mp3filename = MusicDir + "/" + song_file;
  song_ondisk = DirectoryScanner::exists(mp3filename);
}

void QSong::init(const QString filename, const QString currentpath) 
{
  char *fulltitle;
  int len = filename.length();
  /* check for .idx at the end */
  assert(filename.contains(".idx"));
  /* full index file */
  song_index = currentpath+"/"+filename;
  /* retrieve fullname without .idx */
  fulltitle=strdup(filename);
  assert(fulltitle);
  fulltitle[len-4]=0;
  /* get all data */
  if (!obtainTitleAuthor(fulltitle))
    {
      song_title = fulltitle;
      song_author = "";
      song_version = "";
    }
  free(fulltitle);
  /* read the index file */
  reread();
  /* set played flag */
  song_played=Played::IsPlayed(song_index);
  played_author_at_time = -100;
  hue=-2; // -2 = no initialised, -1 = no spectrum
  sat=-1;
  color_distance = 0;
  spectrum_string = "";
  distance_string = "";
}

bool QSong::obtainTitleAuthor(char * fulltitle)
{
  char * author, * version;
  /* find '[' */
  author=fulltitle;
  while(*author && (*author!='[')) 
    author++;
  if (!*author)
    return false;
  *author=0;
  author++;
  /* version is everything after the last ] */
  version=author;
  while(*version && (*version!=']')) 
    version++;
  if (!*version) 
    return false;
  *version=0;
  version++;
  if (!*version) 
    version="1";
  /* succeeded, assign and return */
  song_title = fulltitle;
  song_author = author;
  song_version = version;
  return true;
}

bool QSong::containsTag(const QString tag)
{
  if (!song_tags) 
    return false;
  return song_tags.contains(tag)>0;
}


QSong::QSong(QString a, QString b, QListView* parent) :
  QListViewItem(parent,"","","","","","","")
{
  init(a,b);
  newSpectrum(song_spectrum);
}

void QSong::getColors()
{
  int ih, is;
  if (song_spectrum.isNull())
    {
      hue=-1;
      sat=-1;
      return;
    }
  QColor transfer;
  float red = 0, green = 0, blue = 0;
  int val;
  for (int i = 0; i<24;i++)
    {
      char letter = song_spectrum.at(i).latin1() - 'a';
      float mismatch=letter;
      mismatch=mismatch*scales[i];
      mismatch*=mismatch;
      mismatch/=625.0;
      transfer.setHsv(i*240/24,255,255);
      red+=transfer.red()*mismatch;
      green+=transfer.green()*mismatch;
      blue+=transfer.blue()*mismatch;
    }
  if (red >= green && red >= blue)
    {
      green *= 255.0/red;
      blue  *= 255.0/red;
      red    = 255.0;
    }
  if (green >= red && green >= blue)
    {
      red   *= 255.0/green;
      blue  *= 255.0/green;
      green  = 255.0;
    }
  if (blue >= red && blue >= green)
    {
      red   *= 255.0/blue;
      green *= 255.0/blue;
      blue   = 255.0;
    }
  transfer.setRgb((int)red,(int)green,(int)blue);
  transfer.getHsv(ih,is,val);
  hue=ih;
  sat=is;

  spectrum_string = 
    ( hue==-2 ? 
      QString::null :
      ( hue==-1 ? 
	QString::null :
	( hue < 10 ?
	  QString("00")+QString::number(hue) :
	  ( hue < 100 ?
	    QString("0")+QString::number(hue) :
	    QString::number(hue) ) ) ) );
}

static float max(float a, float b)
{
  if (a>b)
    return a;
  else
    return b;
}

static float min(float a, float b)
{
  if (a<b)
    return a;
  else
    return b;
}

bool QSong::getDistance()
{
  color_distance=255;
  if (!song_spectrum.isNull())
    if (ProcessManager::playingInMain)
      {
	QString main = ProcessManager::playingInMain->song_spectrum;
	if (!main.isNull())
	  {
	    float distance=0;
	    for (int i = 0; i<24;i++)
	      {
		char letter1 = song_spectrum.at(i).latin1();
		char letter2 = main.at(i).latin1();
		float mismatch=letter1-letter2;
		mismatch*=scales[i];
		mismatch*=mismatch;
		distance+=mismatch;
	      }
	    // maximum is 14 * 25 * 25
	    // distance /= (34.3/16.0);
	    if (distance>255) distance=255;
	    color_distance = (int)distance;
	  }
      }
  distance_string = 
    ( ( color_distance<0 || color_distance == 255 ) ?
      QString::null :
      ( color_distance < 10 ?
	QString("00")+QString::number(color_distance) :
	( color_distance < 100 ? 
	  QString("0")+QString::number(color_distance) :
	  QString::number(color_distance) ) ) );
  return color_distance<255;
}

/**
 * this is a mess... (TM)
 */
void QSong::paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align)
{
  switch(col)
    {
    case LIST_CUES:
      if (Config::color_cues && !has_cues)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_TEMPO:
      if (Config::color_range && (ProcessManager::mainTempo!=0) && song_tempo)
	{
	  int nogreen_value = 255;
	  double t=atof(song_tempo);
	  if (   t > ProcessManager::mainTempo * 0.94 
	      && t < ProcessManager::mainTempo * 1.06 )
	    nogreen_value = (int)(fabs(t/ProcessManager::mainTempo-1.0)*255.0/0.06);
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(nogreen_value,255,nogreen_value));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;

    case LIST_TITLE:
      if (Config::color_played && song_played)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(255,0,0));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
      
    case LIST_AUTHOR:
      {
	if (Config::color_authorplayed)
	  {
	    int played_author_songs_ago = Played::songs_played - played_author_at_time;
	    if (played_author_songs_ago < Config::authorDecay)
	      {
		QColorGroup ncg(cg);
		ncg.setColor(QColorGroup::Base,
			     QColor(255,150+(255-150)*played_author_songs_ago/Config::authorDecay
				    ,255*played_author_songs_ago/Config::authorDecay));
		QListViewItem::paintCell(p,ncg,col,wid,align);
		return;
	      }
	  }
	break;
      }
      
    case LIST_DCOLOR:
      {
	if (Config::color_dcolor)
	  {
	    QColorGroup ncg(cg);
	    ncg.setColor(QColorGroup::Base,QColor(255,255,color_distance));
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      }
      
    case LIST_SPECTRUM:
      if (Config::color_spectrum)
	if (!song_spectrum.isNull())
	  {
	    QColorGroup ncg(cg);
	    float distance = 1.0;
	    if (ProcessManager::playingInMain && !ProcessManager::playingInMain->song_spectrum.isNull())
	      {
		QSong *main= ProcessManager::playingInMain;
		distance  = fabs(main->hue-hue);
		distance  = min(distance,360-distance);
		if (distance<45)
		  distance = 1.0;
		else if (distance<60)
		  distance = 1.0-(distance-45)/15.0;
		else
		  distance = 0.0;
	      }
	    QColor transfer;
	    transfer.setHsv(hue,sat,(int)(255.0*distance));
	    ncg.setColor(QColorGroup::Base,transfer);
	    QListViewItem::paintCell(p,ncg,col,wid,align);
	    return;
	  }
      break;
      
    case LIST_ONDISK:
      if (Config::color_ondisk && !song_ondisk)
	{
	  QColorGroup ncg(cg);
	  ncg.setColor(QColorGroup::Base,QColor(0,0,255));
	  QListViewItem::paintCell(p,ncg,col,wid,align);
	  return;
	}
      break;
    }
  
  QListViewItem::paintCell(p,cg,col,wid,align);
}


QString QSong::TRUE_TEXT("Yes");
QString QSong::FALSE_TEXT("No");

QString QSong::text(int i) const
{
  switch (i)
    {
    case LIST_VERSION : return song_version;
    case LIST_TITLE : return song_title;
    case LIST_AUTHOR : return song_author;
    case LIST_TEMPO : return song_tempo;
    case LIST_INDEX : return song_index;
    case LIST_TAGS : return song_tags;
    case LIST_TIME : return song_time;
    case LIST_MD5SUM : return song_md5sum;
    case LIST_DCOLOR : return distance_string;
    case LIST_SPECTRUM : return spectrum_string;
    case LIST_ONDISK :
      if (song_ondisk) 
	return TRUE_TEXT;
      else 
	return FALSE_TEXT;
    case LIST_CUES :
      if (has_cues) 
	return TRUE_TEXT;
      else 
	return FALSE_TEXT;
    case LIST_FILE : return song_file;
    }
  return QString::null;
}
