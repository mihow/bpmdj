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

#include <qlineedit.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <stdlib.h>
#include <qpushbutton.h>
#include "qstring-factory.h"
#include "preferences.logic.h"
#include "config.h"

int  Config::file_count = 1000;
int  Config::yellowTime = 120;   
int  Config::orangeTime = 150;
int  Config::redTime = 180; 
int  Config::filterBpm;
int  Config::authorDecay = 5;
bool Config::color_range = true;
bool Config::color_played = true;
bool Config::color_authorplayed = true;
bool Config::color_ondisk = true;
bool Config::color_cues = true;
bool Config::color_dcolor = true;
bool Config::color_spectrum = true;
bool Config::limit_ondisk = true;
bool Config::limit_nonplayed = false;
bool Config::limit_uprange = false;
bool Config::limit_downrange = false;
bool Config::limit_indistance = false;
bool Config::limit_authornonplayed = false;
bool Config::shown_aboutbox = false;
bool Config::open_mixer = false;
float Config::distance_temposcale = 0.06;
float Config::distance_spectrumscale = 1.0;
bool Config::ask_mix = true;
bool Config::auto_popqueue = false;
QString Config::playCommand1 = "";
QString Config::playCommand2 = "";
QString Config::playCommand3 = "";
QString Config::playCommand4 = "";
QString Config::tmp_directory = "./";
QString Config::mixer_command = "";
QString Config::record_command = "";
QString Config::replay_command = "";
QString Config::record_mixer_command = "";
QColor Config::color_tempo44(0,255,0);
QColor Config::color_tempo54(255,0,0);
QColor Config::color_tempo64(255,0,0);
QColor Config::color_tempo74(255,0,0);
QColor Config::color_tempo84(255,255,0);
bool   Config::show_tempo54 = false;
bool   Config::show_tempo64 = false;
bool   Config::show_tempo74 = false;
bool   Config::show_tempo84 = true;
bool   Config::show_unknown_tempo = true;
QColor Config::color_green_time(0,255,0);
QColor Config::color_yellow_time(255,255,0);
QColor Config::color_orange_time(255,170,0);
QColor Config::color_red_time(255,0,0);
bool   Config::color_main_window = false;
bool   Config::log_spectrum_distance = true;
QColor Config::color_played_song(255,0,0);
QColor Config::color_played_author(255,170,0);
QColor Config::color_unavailable(127,127,127);
QColor Config::color_dcolor_col(255,255,0);
int    Config::color_cluster_depth = 3;
QListView * Config::taglist = NULL;
QHeader   * Config::header = NULL;

void Config::save()
{
  QFile f(".kbpmdj");
  f.open(IO_WriteOnly);
  QDataStream s(&f);
  s << (Q_UINT16)MAGIC_NOW;
  s << (Q_UINT16)file_count;
  s << (Q_UINT16)yellowTime;
  s << (Q_UINT16)orangeTime;
  s << (Q_UINT16)redTime;
  s << (Q_UINT16)filterBpm;
  s << (Q_INT8)color_range;
  s << (Q_INT8)color_played;
  s << (Q_INT8)color_authorplayed;
  s << (Q_INT8)color_ondisk;
  s << (Q_INT8)color_cues;
  s << (Q_INT8)color_dcolor;
  s << (Q_INT8)color_spectrum;
  s << (Q_INT8)authorDecay;
  s << (Q_INT8)limit_ondisk;
  s << (Q_INT8)limit_nonplayed;
  s << (Q_INT8)limit_uprange;
  s << (Q_INT8)limit_downrange;
  s << (Q_INT8)limit_indistance;
  s << playCommand1;
  s << playCommand2;
  s << playCommand3;
  s << playCommand4;
  s << distance_temposcale;
  s << distance_spectrumscale;
  s << (Q_INT8)limit_authornonplayed;
  s << (Q_INT8)shown_aboutbox;
  s << tmp_directory;
  s << mixer_command;
  s << (Q_INT8)open_mixer;
  s << (Q_INT8)ask_mix;
  s << (Q_INT8)auto_popqueue;
  s << record_command;
  s << replay_command;
  s << record_mixer_command;
  s << color_tempo44;
  s << color_tempo54;
  s << color_tempo64;
  s << color_tempo74;
  s << color_tempo84;
  s << (Q_INT8)show_tempo54;
  s << (Q_INT8)show_tempo64;
  s << (Q_INT8)show_tempo74;
  s << (Q_INT8)show_tempo84;
  s << (Q_INT8)show_unknown_tempo;
  s << color_green_time;
  s << color_yellow_time;
  s << color_orange_time;
  s << color_red_time;
  s << (Q_INT8)color_main_window;
  s << (Q_INT8)log_spectrum_distance;
  s << color_played_song;
  s << color_played_author;
  s << color_unavailable;
  s << color_dcolor_col;
  s << (Q_UINT16)color_cluster_depth;
  // export the taglist... tricky thing (tm)
  Q_UINT16 nr = taglist->childCount();
  s << nr;
  QListViewItemIterator it(taglist);
  while(it.current())
    {
      QListViewItem * i = it.current();
      s << (i->text(0)); 
      bool b = i->text(1) == TAG_TRUE;
      s << (Q_INT8)b;
      b = i->text(2) == TAG_TRUE;
      s << (Q_INT8)b;
      b = i->text(3) == TAG_TRUE;
      s << (Q_INT8)b;
      it++;
    }
  // export the header;
  nr = header->count();
  s << nr;
  for(int i = 0 ; i < nr ; i ++)
    {
      s <<(Q_UINT16)header->mapToIndex(i);
      s <<(Q_UINT16)header->sectionSize(i);
    }
}

void Config::load()
{
  taglist=new QListView();
  header = new QHeader();
  if (QFile::exists(".kbpmdj"))
    {
      Q_INT8 b;
      Q_UINT16 magic;
      Q_UINT16 w;
      float fl;
      QFile f(".kbpmdj");
      f.open(IO_ReadOnly);
      QDataStream s(&f);
      s >> magic;
      if (magic == MAGIC_1_6)
	{
	  printf("Loading config v1.6\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; authorDecay = b;
	  s >> playCommand1;
	  s >> playCommand2;
	}
      else if (magic == MAGIC_1_7)
	{
	  printf("Loading config v1.7\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> b; // limit_inspectrum
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	}
      else if (magic == MAGIC_1_8)
	{
	  printf("Loading config v1.8\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	}
      else if (magic == MAGIC_1_9)
	{
	  printf("Loading config v1.9\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	  s >> fl; distance_temposcale = fl;
	  s >> fl; distance_spectrumscale = fl;
	  s >> b; limit_authornonplayed = b;
	  s >> b; shown_aboutbox = b;
	}
      else if (magic == MAGIC_2_1)
	{
	  printf("Loading config v2.1\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	  s >> fl; distance_temposcale = fl;
	  s >> fl; distance_spectrumscale = fl;
	  s >> b; limit_authornonplayed = b;
	  s >> b; shown_aboutbox = b;
	  s >> tmp_directory;
	  s >> mixer_command;
	  s >> b; open_mixer = b;
	}
      else if (magic == MAGIC_2_2)
	{
	  printf("Loading config v2.2\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	  s >> fl; distance_temposcale = fl;
	  s >> fl; distance_spectrumscale = fl;
	  s >> b; limit_authornonplayed = b;
	  s >> b; shown_aboutbox = b;
	  s >> tmp_directory;
	  s >> mixer_command;
	  s >> b; open_mixer = b;
	  s >> b; ask_mix = b;
	  s >> b; auto_popqueue = b;
	}
      else if (magic == MAGIC_2_4)
	{
	  printf("Loading config v2.4\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	  s >> fl; distance_temposcale = fl;
	  s >> fl; distance_spectrumscale = fl;
	  s >> b; limit_authornonplayed = b;
	  s >> b; shown_aboutbox = b;
	  s >> tmp_directory;
	  s >> mixer_command;
	  s >> b; open_mixer = b;
	  s >> b; ask_mix = b;
	  s >> b; auto_popqueue = b;
	  s >> record_command;
	  s >> replay_command;
	  s >> record_mixer_command;
	}
      else if (magic == MAGIC_2_5)
	{
	  printf("Loading config v2.5\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; color_ondisk = b;
	  s >> b; color_cues = b;
	  s >> b; color_dcolor = b;
	  s >> b; color_spectrum = b;
	  s >> b; authorDecay = b;
	  s >> b; limit_ondisk = b;
	  s >> b; limit_nonplayed = b;
	  s >> b; limit_uprange = b;
	  s >> b; limit_downrange = b;
	  s >> b; limit_indistance = b;
	  s >> playCommand1;
	  s >> playCommand2;
	  s >> playCommand3;
	  s >> playCommand4;
	  s >> fl; distance_temposcale = fl;
	  s >> fl; distance_spectrumscale = fl;
	  s >> b; limit_authornonplayed = b;
	  s >> b; shown_aboutbox = b;
	  s >> tmp_directory;
	  s >> mixer_command;
	  s >> b; open_mixer = b;
	  s >> b; ask_mix = b;
	  s >> b; auto_popqueue = b;
	  s >> record_command;
	  s >> replay_command;
	  s >> record_mixer_command;
	  s >> color_tempo44;
	  s >> color_tempo54;
	  s >> color_tempo64;
	  s >> color_tempo74;
	  s >> color_tempo84;
	  s >> b; show_tempo54 = b;
	  s >> b; show_tempo64 = b;
	  s >> b; show_tempo74 = b;
	  s >> b; show_tempo84 = b;
	  s >> b; show_unknown_tempo = b;
	  s >> color_green_time;
	  s >> color_yellow_time;
	  s >> color_orange_time;
	  s >> color_red_time;
	  s >> b; color_main_window = b;
	  s >> b; log_spectrum_distance = b;
	  s >> color_played_song;
	  s >> color_played_author;
	  s >> color_unavailable;
	  s >> color_dcolor_col;
	  s >> w; color_cluster_depth = w;
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(taglist, t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m;
	      header->addLabel("");
	      s >> m; header->moveSection(i,m);
	      s >> m; header->resizeSection(i,m);
	    }
	}
      else
	printf("Wrong config file format\n");
      if (magic != MAGIC_NOW)
	{
	  shown_aboutbox = false;
	  if (MAGIC_NOW == MAGIC_2_1)
	    {
	      playCommand1="";
	      playCommand2="";
	      playCommand3="";
	      playCommand4="";
	    }
	}
      fflush(stdout);
    }
}

void Config::openUi()
{
  char tmp[50];
  PreferencesLogic preferences(NULL,NULL,TRUE);
  preferences.playerCommand1 -> setText ( playCommand1 ) ;
  preferences.playerCommand2 -> setText ( playCommand2 ) ;
  preferences.playerCommand3 -> setText ( playCommand3 ) ;
  preferences.playerCommand4 -> setText ( playCommand4 ) ;
  sprintf(tmp,"%d",yellowTime);
  preferences.yellowTime->setText(tmp);
  sprintf(tmp,"%d",orangeTime);
  preferences.orangeTime->setText(tmp);
  sprintf(tmp,"%d",redTime);
  preferences.redTime->setText(tmp);
  preferences.greenTimeColor -> setPaletteBackgroundColor( color_green_time );
  preferences.yellowTimeColor -> setPaletteBackgroundColor( color_yellow_time );
  preferences.orangeTimeColor -> setPaletteBackgroundColor( color_orange_time );
  preferences.redTimeColor -> setPaletteBackgroundColor( color_red_time );
  preferences.color_main_window -> setChecked(color_main_window);
  preferences.authorDecay  -> setValue ( authorDecay ) ;
  preferences.tempoSpin    -> setValue ( (int)(distance_temposcale*100.0) ) ;
  preferences.spectrumSpin -> setValue ( (int)(distance_spectrumscale*100.0) );
  preferences.tmpDirectory -> setText  ( tmp_directory ) ;
  preferences.mixerCommand -> setText  ( mixer_command ) ;
  preferences.record       -> setText  ( record_command ) ;
  preferences.record_mixer -> setText  ( record_mixer_command ) ;
  preferences.replay       -> setText  ( replay_command ) ;

  preferences.colorTempo44 -> setPaletteBackgroundColor( color_tempo44 );
  preferences.colorTempo54 -> setPaletteBackgroundColor( color_tempo54 );
  preferences.colorTempo64 -> setPaletteBackgroundColor( color_tempo64 );
  preferences.colorTempo74 -> setPaletteBackgroundColor( color_tempo74 );
  preferences.colorTempo84 -> setPaletteBackgroundColor( color_tempo84 );

  preferences.showTempo54 -> setChecked( show_tempo54 );
  preferences.showTempo64 -> setChecked( show_tempo64 );
  preferences.showTempo74 -> setChecked( show_tempo74 );
  preferences.showTempo84 -> setChecked( show_tempo84 );
  preferences.showUnknownTempo -> setChecked( show_unknown_tempo );

  preferences.logdcolor -> setChecked( log_spectrum_distance );

  preferences.colorPlayedSong -> setPaletteBackgroundColor( color_played_song );
  preferences.colorPlayedAuthor -> setPaletteBackgroundColor( color_played_author );
  preferences.colorUnavailableSong -> setPaletteBackgroundColor( color_unavailable );
  preferences.colordColorCol -> setPaletteBackgroundColor( color_dcolor_col );

  preferences.clusterDepth -> setValue (color_cluster_depth );

  if (preferences.exec()==QDialog::Accepted)
    {
      playCommand1=preferences.playerCommand1->text();
      playCommand2=preferences.playerCommand2->text();
      playCommand3=preferences.playerCommand3->text();
      playCommand4=preferences.playerCommand4->text();
      yellowTime=atoi(preferences.yellowTime->text());
      orangeTime=atoi(preferences.orangeTime->text());
      redTime=atoi(preferences.redTime->text());
      authorDecay = preferences.authorDecay->value();
      distance_temposcale = (float)(preferences.tempoSpin->value())/100.0;
      distance_spectrumscale = (float)(preferences.spectrumSpin->value())/100.0;
      tmp_directory = preferences.tmpDirectory->text();
      mixer_command = preferences.mixerCommand->text();
      record_command = preferences.record->text();
      replay_command = preferences.replay->text();
      record_mixer_command = preferences.record_mixer->text();
      
      color_tempo44 = preferences.colorTempo44->paletteBackgroundColor();
      color_tempo54 = preferences.colorTempo54->paletteBackgroundColor();
      color_tempo64 = preferences.colorTempo64->paletteBackgroundColor();
      color_tempo74 = preferences.colorTempo74->paletteBackgroundColor();
      color_tempo84 = preferences.colorTempo84->paletteBackgroundColor();

      show_tempo54  = preferences.showTempo54->isChecked();
      show_tempo64  = preferences.showTempo54->isChecked();
      show_tempo74  = preferences.showTempo64->isChecked();
      show_tempo84  = preferences.showTempo84->isChecked();
      show_unknown_tempo = preferences.showUnknownTempo->isChecked();
      
      color_red_time = preferences.redTimeColor -> paletteBackgroundColor( );
      color_orange_time = preferences.orangeTimeColor -> paletteBackgroundColor( );
      color_yellow_time = preferences.yellowTimeColor -> paletteBackgroundColor( );
      color_green_time = preferences.greenTimeColor -> paletteBackgroundColor( );
      color_main_window = preferences.color_main_window->isChecked();

      log_spectrum_distance = preferences.logdcolor -> isChecked();

      color_played_song = preferences.colorPlayedSong -> paletteBackgroundColor( );
      color_played_author = preferences.colorPlayedAuthor -> paletteBackgroundColor( );
      color_unavailable = preferences.colorUnavailableSong -> paletteBackgroundColor( );
      color_dcolor_col = preferences.colordColorCol -> paletteBackgroundColor( );
      color_cluster_depth = preferences.clusterDepth -> value();
      
      save();
    }
}


// the problem we have is that the taglist is possibly destroyed before 
// the configurator can save it. The results is that we must copy it before,
// womething I would rather not like to do...
