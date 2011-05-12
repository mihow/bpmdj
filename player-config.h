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
#ifndef __loaded__player_config_h__
#define __loaded__player_config_h__
using namespace std;
#line 1 "player-config.h++"
#include <Qt/qdialog.h>
#include "types.h"
#include "accessors.h"

class PlayerConfig
{
 private:
  accessors(QString,meta_name,"");
  void init();
 public:
  // meta information
  // version 2.9
  accessors(int,ui_posx,-1);
  accessors(int,ui_posy,-1);
  accessors(QString,core_rawpath,"./"); // goes into the environment
  /**
   * Which DSP do we want ?
   * 0 = none; 
   * 1 = oss; 
   * 2 = ALSA; 
   * 3 = mixed; 
   * 4 = jack
   */
  accessors(int,player_dsp,0);
  accessors(int,alsa_latency,150);      
  accessors(bool,alsa_verbose,false);
  accessors(QString, alsa_dev, "hw:0,0");
  accessors(QString, oss_dsp,"/dev/dsp");
  accessors(bool,oss_init_fragments,false);
  accessors(int,oss_fragments,16);
  accessors(bool,oss_verbose,true);
  accessors(bool,oss_nolatencyaccounting,false);
  accessors(int,oss_latency,150);
  accessors(int,bpm_channel,1);
  accessors(bool,player_rms,false);
  accessors(float4,player_rms_target,0.12);
  accessors(unsigned2,disabled_capacities,0);
   // version 3.3
  accessors(QString, jack_dev,"");
  accessors(int,jack_latency,150);
  accessors(bool,jack_verbose,true);
  // version 4.1
  accessors(QString, jack_lout,"");
  accessors(QString, jack_rout,"");
  PlayerConfig();
  PlayerConfig(QString configname);
  void load_ui_position(QDialog * dialog);
  void save_ui_position(QDialog * dialog);
  void save();
  void load();
};
#endif // __loaded__player_config_h__
