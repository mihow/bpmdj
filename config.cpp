/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__config_cpp__
#define __loaded__config_cpp__
using namespace std;
#line 1 "config.c++"
#include <qlineedit.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <stdlib.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include "bpmdj-pref.h"
#include "config.h"
#include "song.h"
#include "song-metric.h"
#include "metric-widget.h"
#include "bpmdj.h"
#include "scripts.h"
#include "magic.h"

ConfigState::ConfigState(bool val): QObject(NULL,NULL), state(val)
{
}

void ConfigState::setMenuText(const QString & text)
{
  menu = NULL;
  menu_text=text;
}

bool ConfigState::isOn() const
{
  return state;
}

void ConfigState::addTo(Q3PopupMenu * m)
{
  menu = m;
  item = m->insertItem(menu_text,this,SLOT(toggle()));
  update();
}

void ConfigState::update()
{
  if (menu) 
    menu->setItemChecked(item,state);
}

void ConfigState::toggle()
{
  setOn(!state);
}

void ConfigState::set(bool new_state)
{
  state = new_state;
}

void ConfigState::setOn(bool new_state)
{
  state = new_state;
  update();
  emit toggled();
}

init_singleton_var(Config,file_count,int,1000);
init_singleton_var(Config,yellowTime,int,120);
init_singleton_var(Config,orangeTime,int,150);
init_singleton_var(Config,redTime,int,180); 
init_singleton_var(Config,filterBpm,int,0);
init_singleton_var(Config,authorDecay,int,5);
ConfigState Config::color_range(true);
ConfigState Config::color_played(true);
ConfigState Config::color_authorplayed(true);
ConfigState Config::color_ondisk(true);
ConfigState Config::color_cues(true);
ConfigState Config::color_dcolor(true);
ConfigState Config::color_spectrum(true);
ConfigState Config::limit_ondisk(true);
ConfigState Config::limit_nonplayed(false);
ConfigState Config::limit_uprange(false);
ConfigState Config::limit_downrange(false);
ConfigState Config::limit_indistance(false);
ConfigState Config::limit_authornonplayed(false);
ConfigState Config::color_songs_based_on_history(true);
init_singleton_var(Config,shown_aboutbox,bool,false);
// ConfigState Config::open_mixer(false);
init_singleton_var(Config,distance_temposcale,float4,0.06);
init_singleton_var(Config,distance_spectrumweight,float4,0.11);
ConfigState Config::ask_mix(true);
ConfigState Config::auto_popqueue(false);
SongProcess Config::players[4];
//init_singleton_var(Config,mixer_command,QString,"");
init_singleton_var(Config,color_tempo44,QColor,QColor(0,255,0));
init_singleton_var(Config,color_tempo54,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo64,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo74,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo84,QColor,QColor(255,255,0));
init_singleton_var(Config,show_tempo54,bool,false);
init_singleton_var(Config,show_tempo64,bool,false);
init_singleton_var(Config,show_tempo74,bool,false);
init_singleton_var(Config,show_tempo84,bool,true);
init_singleton_var(Config,show_unknown_tempo,bool,true);
init_singleton_var(Config,color_green_time,QColor,QColor(0,255,0));
init_singleton_var(Config,color_yellow_time,QColor,QColor(255,255,0));
init_singleton_var(Config,color_orange_time,QColor,QColor(255,170,0));
init_singleton_var(Config,color_red_time,QColor,QColor(255,0,0));
init_singleton_var(Config,color_main_window,bool,false);
init_singleton_var(Config,log_spectrum_distance,bool,true);
init_singleton_var(Config,color_played_song,QColor,QColor(255,0,0));
init_singleton_var(Config,color_played_author,QColor,QColor(255,170,0));
init_singleton_var(Config,color_unavailable,QColor,QColor(127,127,127));
init_singleton_var(Config,color_dcolor_col,QColor,QColor(255,255,0));
init_singleton_var(Config,color_cluster_depth,int,3);
//init_singleton_var(Config,taglist,Q3ListView*,NULL);
init_singleton_var(Config,header,Q3Header*,NULL);
//init_singleton_var(Config,bpm_mixer_command,QString,"kbpm-mix --alsa");
// ConfigState Config::open_bpmmixer(false);
init_singleton_var(Config,distance_tempoweight,float4,0.02);
init_singleton_var(Config,distance_echoweight,float4,0.62);
init_singleton_var(Config,distance_rythmweight,float4,0.26);
init_singleton_var(Config,distance_compositionweight,float4,0.33);
init_singleton_var(Config,max_songs,int,100);
init_singleton_var(Config,color_unchecked,QColor,QColor(219,219,219));
SongProcess Config::analyzers[8];
init_singleton_var(Config,anal_bpm_from,float4,90);
init_singleton_var(Config,anal_bpm_to,float4,160);
init_singleton_var(Config,anal_bpm_technique,int,1);
init_singleton_var(Config,color_alltime,QColor,QColor(0,0,255));
ConfigState Config::play_fragments(false);
init_singleton_var(Config,tag_include,QString,"");
init_singleton_var(Config,tag_mustbe,QString,"");
init_singleton_var(Config,tag_exclude,QString,"");

/**
 * we don't open the mixer command because the program its normal 
 * mode of operation should be two soundcards. The mixer is a fix and
 * even almost experimental feature.
 */
void dump_header(Q3Header * h)
{
  // export the header;
  int nr = h->count();
  for(int i = 0 ; i < nr ; i ++)
    printf("%d -> %d [%d]\t",i,h->mapToSection(i),h->sectionSize(i));
}

void realize_mapping(Q3Header * h, int i, int j, int s)
{
  h->moveSection(j,i);
  //  dump_header(h);
  h->resizeSection(j,s);
}

void copy_header(Q3Header * in, Q3Header * out)
{
  for(int i = 0 ; i < in->count() ; i ++)
    {
      int j = in->mapToSection(i);
      realize_mapping(out,i,j,in->sectionSize(j));
    }
}

void Config::save()
{
  QFile f(".bpmdj");
  f.open(IO_WriteOnly);
  if (!f.isOpen() || !f.isWritable())
    {
      Error(true,"Configuration file \".bpmdj\" cannot be opened for writing");
      return;
    }
  QDataStream s(&f);
  s << (Q_UINT16)MAGIC_NOW;
  s << (Q_UINT16)get_file_count();
  s << (Q_UINT16)get_yellowTime();
  s << (Q_UINT16)get_orangeTime();
  s << (Q_UINT16)get_redTime();
  s << (Q_UINT16)get_filterBpm();
  s << (Q_INT8)color_range;
  s << (Q_INT8)color_played;
  s << (Q_INT8)color_authorplayed;
  s << (Q_INT8)color_ondisk;
  s << (Q_INT8)color_cues;
  s << (Q_INT8)color_dcolor;
  s << (Q_INT8)color_spectrum;
  s << (Q_INT8)get_authorDecay();
  s << (Q_INT8)limit_ondisk;
  s << (Q_INT8)limit_nonplayed;
  s << (Q_INT8)limit_uprange;
  s << (Q_INT8)limit_downrange;
  s << (Q_INT8)limit_indistance;
  for(int i = 0 ; i < 4 ; i ++)
    {
      s << (Q_UINT16)players[i].getCmd();
      s << players[i].getName();
      s << players[i].getRemote();
    }
  s << get_distance_temposcale();
  s << get_distance_spectrumweight();
  s << (Q_INT8)limit_authornonplayed;
  s << (Q_INT8)get_shown_aboutbox();
  s << QString(""); // get_mixer_command();
  s << (Q_INT8)0; // the obsolte open_mixer flag
  s << (Q_INT8)ask_mix;
  s << (Q_INT8)auto_popqueue;
  s << QString(""); // get_record_command();
  s << QString(""); // was get_replay_command();
  s << QString(""); // get_record_mixer_command();
  s << get_color_tempo44();
  s << get_color_tempo54();
  s << get_color_tempo64();
  s << get_color_tempo74();
  s << get_color_tempo84();
  s << (Q_INT8)get_show_tempo54();
  s << (Q_INT8)get_show_tempo64();
  s << (Q_INT8)get_show_tempo74();
  s << (Q_INT8)get_show_tempo84();
  s << (Q_INT8)get_show_unknown_tempo();
  s << get_color_green_time();
  s << get_color_yellow_time();
  s << get_color_orange_time();
  s << get_color_red_time();
  s << (Q_INT8)get_color_main_window();
  s << (Q_INT8)get_log_spectrum_distance();
  s << get_color_played_song();
  s << get_color_played_author();
  s << get_color_unavailable();
  s << get_color_dcolor_col();
  s << (Q_UINT16)get_color_cluster_depth();
  s << QString(""); // get_bpm_mixer_command();
  s << (Q_INT8)0 ; // open_bpmmixer;
  Q_UINT16 nr = 0;
  s << nr;
  // export the header (tricky thing (tm)
  nr = get_header()->count();
  s << nr;
  for(int i = 0 ; i < nr ; i ++)
    {
      int j =  get_header()->mapToSection(i);
      s <<(Q_UINT16)j;
      s <<(Q_UINT16)get_header()->sectionSize(j);
    }
  s << get_distance_tempoweight();
  s << get_distance_echoweight();
  s << get_distance_rythmweight();
  s << get_distance_compositionweight();
  s << (Q_UINT16)get_max_songs();
  // save pixmap if it is there
  s << get_color_unchecked();
  for(int i = 0 ; i < 8 ; i ++)
    {
      s << (Q_UINT16)analyzers[i].getCmd();
      s << analyzers[i].getName();
      s << analyzers[i].getRemote();
    }
  s << get_anal_bpm_from();
  s << get_anal_bpm_to();
  s << (Q_UINT16)get_anal_bpm_technique();
  s << (Q_INT8) color_songs_based_on_history;
  s << get_color_alltime();
  s << (Q_INT8)play_fragments;
  s << get_tag_include();
  s << get_tag_mustbe();
  s << get_tag_exclude();
}

bool Config::load()
{
  // initialize the analyzers and players
  for(int i = 0 ; i < 8 ; i++)
    {
      analyzers[i].setNameId("analyzer"+QString::number(i),i);
      analyzers[i].setAnalyzer();
    }
  for(int i = 0 ; i < 4 ; i++)
    players[i].setNameId("player"+QString::number(i),i);
  Q3ListView * taglist = new Q3ListView();
  set_header(new Q3Header());
  if (QFile::exists(".bpmdj"))
    {
      Q_INT8 b;
      Q_UINT16 magic;
      Q_UINT16 w;
      QString str;
      QColor clr;
      float4 fl;
      QFile f(".bpmdj");
      f.open(IO_ReadOnly);
      QDataStream s(&f);
      s >> magic;
      if ( magic < MAGIC_3_4 )
	{
	  return false;
	}
      else if (magic == MAGIC_3_4)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; color_range.state=b;
	  s >> b; color_played.state=b;
	  s >> b; color_authorplayed.state=b;
	  s >> b; color_ondisk.state=b;
	  s >> b; color_cues.state=b;
	  s >> b; color_dcolor.state=b;
	  s >> b; color_spectrum.state=b;
	  s >> b; set_authorDecay(b);
	  s >> b; limit_ondisk.state=b;
	  s >> b; limit_nonplayed.state=b;
	  s >> b; limit_uprange.state=b;
	  s >> b; limit_downrange.state=b;
	  s >> b; limit_indistance.state=b;
	  for(int i = 0 ; i < 4 ; i ++)
	    {
	      s >> w;   players[i].setCommand((SongProcess::command_type)w);
	      s >> str; players[i].setName(str);
	      s >> str; players[i].setRemote(str);
	    }
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; limit_authornonplayed.state=b;
	  s >> b; set_shown_aboutbox(b);
	  s >> str; //set_mixer_command(str);
	  s >> b; // open_mixer.set(b);
	  s >> b; ask_mix.set(b);
	  s >> b; auto_popqueue.set(b);
	  s >> str; // set_record_command(str);
	  s >> str; // set_replay_command(str);
	  s >> str; // set_record_mixer_command(str);
	  s >> clr; set_color_tempo44(clr);
	  s >> clr; set_color_tempo54(clr);
	  s >> clr; set_color_tempo64(clr);
	  s >> clr; set_color_tempo74(clr);
	  s >> clr; set_color_tempo84(clr);
	  s >> b; set_show_tempo54(b);
	  s >> b; set_show_tempo64(b);
	  s >> b; set_show_tempo74(b);
	  s >> b; set_show_tempo84(b);
	  s >> b; set_show_unknown_tempo(b);
	  s >> clr; set_color_green_time(clr);
	  s >> clr; set_color_yellow_time(clr);
	  s >> clr; set_color_orange_time(clr);
	  s >> clr; set_color_red_time(clr);
	  s >> b; set_color_main_window(b);
	  s >> b; set_log_spectrum_distance(b);
	  s >> clr; set_color_played_song(clr);
	  s >> clr; set_color_played_author(clr);
	  s >> clr; set_color_unavailable(clr);
	  s >> clr; set_color_dcolor_col(clr);
	  s >> w; set_color_cluster_depth (w);
	  s >> str; // set_bpm_mixer_command(str);
	  s >> b; // open_bpmmixer.set(b);
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
	      new Q3ListViewItem(taglist, t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	  float4 F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  for(int i = 0 ; i < 8 ; i ++)
	    {
	      s >> w;   analyzers[i].setCommand((SongProcess::command_type)w);
	      s >> str; analyzers[i].setName(str);
	      s >> str; analyzers[i].setRemote(str);
	    }
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	  s >> b;   color_songs_based_on_history.set(b);
	  s >> clr; set_color_alltime(clr);
	  s >> b;   play_fragments.set(b);
	}
      else if (magic == MAGIC_3_5 || magic == MAGIC_3_6)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; color_range.state=b;
	  s >> b; color_played.state=b;
	  s >> b; color_authorplayed.state=b;
	  s >> b; color_ondisk.state=b;
	  s >> b; color_cues.state=b;
	  s >> b; color_dcolor.state=b;
	  s >> b; color_spectrum.state=b;
	  s >> b; set_authorDecay(b);
	  s >> b; limit_ondisk.state=b;
	  s >> b; limit_nonplayed.state=b;
	  s >> b; limit_uprange.state=b;
	  s >> b; limit_downrange.state=b;
	  s >> b; limit_indistance.state=b;
	  for(int i = 0 ; i < 4 ; i ++)
	    {
	      s >> w;   players[i].setCommand((SongProcess::command_type)w);
	      s >> str; players[i].setName(str);
	      s >> str; players[i].setRemote(str);
	    }
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; limit_authornonplayed.state=b;
	  s >> b; set_shown_aboutbox(b);
	  s >> str; //set_mixer_command(str);
	  s >> b; // open_mixer.set(b);
	  s >> b; ask_mix.set(b);
	  s >> b; auto_popqueue.set(b);
	  s >> str; // set_record_command(str);
	  s >> str; // set_replay_command(str);
	  s >> str; // set_record_mixer_command(str);
	  s >> clr; set_color_tempo44(clr);
	  s >> clr; set_color_tempo54(clr);
	  s >> clr; set_color_tempo64(clr);
	  s >> clr; set_color_tempo74(clr);
	  s >> clr; set_color_tempo84(clr);
	  s >> b; set_show_tempo54(b);
	  s >> b; set_show_tempo64(b);
	  s >> b; set_show_tempo74(b);
	  s >> b; set_show_tempo84(b);
	  s >> b; set_show_unknown_tempo(b);
	  s >> clr; set_color_green_time(clr);
	  s >> clr; set_color_yellow_time(clr);
	  s >> clr; set_color_orange_time(clr);
	  s >> clr; set_color_red_time(clr);
	  s >> b; set_color_main_window(b);
	  s >> b; set_log_spectrum_distance(b);
	  s >> clr; set_color_played_song(clr);
	  s >> clr; set_color_played_author(clr);
	  s >> clr; set_color_unavailable(clr);
	  s >> clr; set_color_dcolor_col(clr);
	  s >> w; set_color_cluster_depth (w);
	  s >> str; // set_bpm_mixer_command(str);
	  s >> b; // open_bpmmixer.set(b);
	  // read tag data
	  s >> w; assert (w==0);
	  /*	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new Q3ListViewItem(taglist, t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
				}
	  */
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	  float4 F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  for(int i = 0 ; i < 8 ; i ++)
	    {
	      s >> w;   analyzers[i].setCommand((SongProcess::command_type)w);
	      s >> str; analyzers[i].setName(str);
	      s >> str; analyzers[i].setRemote(str);
	    }
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	  s >> b;   color_songs_based_on_history.set(b);
	  s >> clr; set_color_alltime(clr);
	  s >> b;   play_fragments.set(b);
	  s >> str; set_tag_include(str);
	  s >> str; set_tag_mustbe(str);
	  s >> str; set_tag_exclude(str);
	}
      else
	printf("Wrong config file format\n");
      calc_and_cache();
      if (magic < MAGIC_3_5)
	taglist2config(taglist);
    }
  // if the magic was csmaller than 3_5 WE CONVER THTE TAGLIST
  return true;
}

void taglist2strings(Q3ListView * tagList, QString &in, QString& must, QString&out)
{
  Q3ListViewItemIterator t(tagList);
  while(t.current())
    {
      Q3ListViewItem * i = t.current();
      QString tag = i->text(0);
      if (i->text(1)==TAG_TRUE) in+=OneSpace+tag+OneSpace;
      if (i->text(2)==TAG_TRUE) must+=OneSpace+tag+OneSpace;
      if (i->text(3)==TAG_TRUE) out+=OneSpace+tag+OneSpace;
      t++;
    } 
}

void taglist2config(Q3ListView*taglist)
{
  QString in,must,out;
  in=must=out="";
  taglist2strings(taglist,in,must,out);
  Config::set_tag_include(in);
  Config::set_tag_exclude(out);
  Config::set_tag_mustbe(must);
}

bool Config::open_ui(int pane)
{
  char tmp[50];
  BpmDjPreferences preferences(NULL,NULL,TRUE);

  sprintf(tmp,"%d",get_yellowTime());
  preferences.yellowTime->setText(tmp);
  sprintf(tmp,"%d",get_orangeTime());
  preferences.orangeTime->setText(tmp);
  sprintf(tmp,"%d",get_redTime());
  preferences.redTime->setText(tmp);
  preferences.greenTimeColor -> setPaletteBackgroundColor( get_color_green_time ());
  preferences.yellowTimeColor -> setPaletteBackgroundColor( get_color_yellow_time ());
  preferences.orangeTimeColor -> setPaletteBackgroundColor( get_color_orange_time ());
  preferences.redTimeColor -> setPaletteBackgroundColor( get_color_red_time ());
  preferences.color_main_window -> setChecked(get_color_main_window());
  
  preferences.authorDecay  -> setValue ( get_authorDecay() ) ;
  preferences.tempoSpin    -> setValue ( lround(get_distance_temposcale()*100.0) ) ;
  // preferences.mixerCommand -> setText  ( get_mixer_command() ) ;

  preferences.colorTempo44 -> setPaletteBackgroundColor( get_color_tempo44 ());
  preferences.colorTempo54 -> setPaletteBackgroundColor( get_color_tempo54 ());
  preferences.colorTempo64 -> setPaletteBackgroundColor( get_color_tempo64 ());
  preferences.colorTempo74 -> setPaletteBackgroundColor( get_color_tempo74 ());
  preferences.colorTempo84 -> setPaletteBackgroundColor( get_color_tempo84 ());
  preferences.colorAlltimePlaycount -> setPaletteBackgroundColor( get_color_alltime ());
  
  preferences.showTempo54 -> setChecked( get_show_tempo54 ());
  preferences.showTempo64 -> setChecked( get_show_tempo64 ());
  preferences.showTempo74 -> setChecked( get_show_tempo74 ());
  preferences.showTempo84 -> setChecked( get_show_tempo84 ());
  preferences.showUnknownTempo -> setChecked( get_show_unknown_tempo ());

  preferences.colorPlayedSong -> setPaletteBackgroundColor( get_color_played_song ());
  preferences.colorPlayedAuthor -> setPaletteBackgroundColor( get_color_played_author ());
  preferences.colorUnavailableSong -> setPaletteBackgroundColor( get_color_unavailable ());
  preferences.colorUncheckedSong -> setPaletteBackgroundColor( get_color_unchecked ());
  preferences.colordColorCol -> setPaletteBackgroundColor( get_color_dcolor_col ());

  preferences.dColorMetric->spectrumSpin -> setValue ( (int)(0.5+get_distance_spectrumweight()*100.0) );
  preferences.dColorMetric->tempoDistanceSpin -> setValue( (int)(0.5+get_distance_tempoweight()*100.0) );
  preferences.dColorMetric->echoSpin -> setValue( (int)(0.5+get_distance_echoweight()*100.0) );
  preferences.dColorMetric->rythmSpin -> setValue( (int)(0.5+get_distance_rythmweight()*100.0) );
  preferences.dColorMetric->compositionSpin -> setValue( (int)(0.5+get_distance_compositionweight()*100.0) );
  preferences.max_songs->setValue(get_max_songs());

  preferences.clusterDepth -> setValue (get_color_cluster_depth ());

  // preferences.bpmmixcmd -> setText( get_bpm_mixer_command () );
  preferences.subtabs -> setCurrentIndex( pane );

  preferences.From -> setText(get_anal_bpm_from() ? QString::number(get_anal_bpm_from()) : "");
  preferences.To   -> setText(get_anal_bpm_to() ? QString::number(get_anal_bpm_to()): "");
  preferences.bpmalgo -> setButton(get_anal_bpm_technique()-1);

  if (preferences.exec()==QDialog::Accepted)
    {
      set_yellowTime(atoi(preferences.yellowTime->text()));
      set_orangeTime(atoi(preferences.orangeTime->text()));
      set_redTime(atoi(preferences.redTime->text()));
      set_authorDecay ( preferences.authorDecay->value());
      set_distance_temposcale ( (float4)(preferences.tempoSpin->value())/100.0);
      // set_mixer_command ( preferences.mixerCommand->text());
      
      set_color_tempo44(preferences.colorTempo44->paletteBackgroundColor());
      set_color_tempo54(preferences.colorTempo54->paletteBackgroundColor());
      set_color_tempo64(preferences.colorTempo64->paletteBackgroundColor());
      set_color_tempo74(preferences.colorTempo74->paletteBackgroundColor());
      set_color_tempo84(preferences.colorTempo84->paletteBackgroundColor());
      set_color_alltime(preferences.colorAlltimePlaycount->paletteBackgroundColor());

      set_show_tempo54 (preferences.showTempo54->isChecked());
      set_show_tempo64 (preferences.showTempo64->isChecked());
      set_show_tempo74 (preferences.showTempo74->isChecked());
      set_show_tempo84 (preferences.showTempo84->isChecked());
      set_show_unknown_tempo(preferences.showUnknownTempo->isChecked());
      
      set_color_red_time(preferences.redTimeColor -> paletteBackgroundColor( ));
      set_color_orange_time(preferences.orangeTimeColor -> paletteBackgroundColor( ));
      set_color_yellow_time(preferences.yellowTimeColor -> paletteBackgroundColor( ));
      set_color_green_time(preferences.greenTimeColor -> paletteBackgroundColor( ));
      set_color_main_window(preferences.color_main_window->isChecked());

      set_color_played_song(preferences.colorPlayedSong -> paletteBackgroundColor( ));
      set_color_played_author(preferences.colorPlayedAuthor -> paletteBackgroundColor( ));
      set_color_unavailable(preferences.colorUnavailableSong -> paletteBackgroundColor( ));
      set_color_unchecked(preferences.colorUncheckedSong -> paletteBackgroundColor( ));
      set_color_dcolor_col(preferences.colordColorCol -> paletteBackgroundColor( ));
      set_color_cluster_depth(preferences.clusterDepth -> value());
      // set_bpm_mixer_command(preferences.bpmmixcmd -> text());
      
      set_distance_tempoweight       ( (float4)(preferences.dColorMetric->tempoDistanceSpin->value())/100.0);
      set_distance_spectrumweight    ( (float4)(preferences.dColorMetric->spectrumSpin->value())/100.0);
      set_distance_echoweight        ( (float4)(preferences.dColorMetric->echoSpin->value())/100.0);
      set_distance_rythmweight       ( (float4)(preferences.dColorMetric->rythmSpin->value())/100.0);
      set_distance_compositionweight ( (float4)(preferences.dColorMetric->compositionSpin->value())/100.0);
      set_max_songs(preferences.max_songs->value());

      set_anal_bpm_from(atof(preferences.From -> text()));
      set_anal_bpm_to(  atof(preferences.To   -> text()));
      set_anal_bpm_technique(preferences.bpmalgo -> selectedId()+1);
      if (get_anal_bpm_from()>get_anal_bpm_to())
	{
	  float4 t=get_anal_bpm_from();
	  set_anal_bpm_from(get_anal_bpm_to());
	  set_anal_bpm_to(t);
	}

      save();
      calc_and_cache();
      return true;
    }
  return false;
}

void Config::calc_and_cache()
{
  SongMetriek::std.set_tempo_limits(get_distance_temposcale());
  SongMetriek::std.set_show_harmonics(get_show_tempo54(),get_show_tempo64(),get_show_tempo74(),get_show_tempo84());
  SongMetriek::std.set_unknown_tempo_matches(get_show_unknown_tempo());
  SongMetriek::std.set_tempo_weight(get_distance_tempoweight());
  SongMetriek::std.set_spectrum_weight(get_distance_spectrumweight());
  SongMetriek::std.set_echo_weight(get_distance_echoweight());
  SongMetriek::std.set_rythm_weight(get_distance_rythmweight());
  SongMetriek::std.set_composition_weight(get_distance_compositionweight());
  SongMetriek::std.prepare();
}
#endif // __loaded__config_cpp__
