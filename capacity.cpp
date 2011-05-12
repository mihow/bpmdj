/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__capacity_cpp__
#define __loaded__capacity_cpp__
using namespace std;
#line 1 "capacity.c++"
#include <cstdio>
#include <qcheckbox.h>
#include <qstring.h>
#include "capacity-widget.h"
#include "capacity.h"
#include "player-config.h"
#include "index.h"

Capacity get_ext_disabled_capacities(QString ext)
{
  ext = ext.toLower();
  if (ext.endsWith(".mp3"))  
    return CAPACITY_OGG123                                        ;
  if (ext.endsWith(".ogg"))  
    return                   CAPACITY_MPG123059R | CAPACITY_MPG321;
  if (ext.endsWith(".m4a"))  
    return CAPACITY_OGG123 | CAPACITY_MPG123059R | CAPACITY_MPG321;
  if (ext.endsWith(".mpc"))  
    return CAPACITY_OGG123 | CAPACITY_MPG123059R | CAPACITY_MPG321;
  if (ext.endsWith(".flac")) 
    return                   CAPACITY_MPG123059R | CAPACITY_MPG321;
  return ~0;
}

QString extensionList()
{
  return ".mp3 .ogg .m4a .mpc .flac";
}

bool goodExtension(QString ext)
{
  ext = ext.toLower();
  return (ext.endsWith(".mp3") || 
	  ext.endsWith(".ogg") || 
	  ext.endsWith(".m4a") || 
	  ext.endsWith(".mpc") || 
	  ext.endsWith(".flac"));
}

int capacity_to_prognr(int full_capacity)
{
  int nr = 1;
  while(!(full_capacity%2)) 
    {
      full_capacity/=2;
      nr++;
    }
  return nr;
}

int get_decoder_program(Capacity host_disabled, Capacity ext_disabled,
			Capacity song_disabled)
{
  Capacity full_capacity=(CAPACITY_LAST*2)-1;
  full_capacity&=~host_disabled;
  full_capacity&=~ext_disabled;
  full_capacity&=~song_disabled;
  if (!full_capacity) return 0;
  return capacity_to_prognr(full_capacity);
}

static int used_capacity = 0;

static void used(QCheckBox * b)
{
  if (!b->text().endsWith(" (used)"))
    b->setText(b->text()+" (used)");
}

static void unavailable(QCheckBox * b)
{
  QPalette palette;
  palette.setColor(b->backgroundRole(), QColor(255,0,0,255));
  b->setPalette(palette);
}

Capacity unavailable_capacities = no_disabled_capacities;

void init_capacity_widget(CapacityWidget * cw, Capacity c)
{
  cw->mpg123059r->setChecked(c &    CAPACITY_MPG123059R);
  cw->mpg321->setChecked(c &        CAPACITY_MPG321);
  cw->mplayer1pre6->setChecked(c &  CAPACITY_MPLAYER1PRE6);
  cw->mplayer1pre7->setChecked(c &  CAPACITY_MPLAYER1PRE7);
  cw->ogg123->setChecked(c &        CAPACITY_OGG123);

  // inform user on unavailable decoders
  if (unavailable_capacities & CAPACITY_MPG123059R) 
    unavailable(cw->mpg123059r);
  if (unavailable_capacities & CAPACITY_MPG321)      
    unavailable(cw->mpg321);
  if (unavailable_capacities & CAPACITY_MPLAYER1PRE6) 
    unavailable(cw->mplayer1pre6);
  if (unavailable_capacities & CAPACITY_MPLAYER1PRE7) 
    unavailable(cw->mplayer1pre7);
  if (unavailable_capacities & CAPACITY_OGG123) unavailable(cw->ogg123);

  // make the used capacity text somewhat different
  if (used_capacity & CAPACITY_MPG123059R) used(cw->mpg123059r);
  if (used_capacity & CAPACITY_MPG321) used(cw->mpg321);
  if (used_capacity & CAPACITY_MPLAYER1PRE6) used(cw->mplayer1pre6);
  if (used_capacity & CAPACITY_MPLAYER1PRE7) used(cw->mplayer1pre7);
  if (used_capacity & CAPACITY_OGG123) used(cw->ogg123);
}

void set_used_decoder(int decoder)
{
  used_capacity = 1 << (decoder-1);
}

Capacity get_capacity(CapacityWidget * cw)
{
  Capacity result = 0;
  result |= cw->mpg123059r->isChecked()   * CAPACITY_MPG123059R;
  result |= cw->mpg321->isChecked()       * CAPACITY_MPG321; 
  result |= cw->mplayer1pre6->isChecked() * CAPACITY_MPLAYER1PRE6;
  result |= cw->mplayer1pre7->isChecked() * CAPACITY_MPLAYER1PRE7;
  result |= cw->ogg123->isChecked()       * CAPACITY_OGG123;
  return result;
}

int set_decoder_environment(PlayerConfig * config, Index * idx)
{
  assert(config);
  assert(idx);
  char environment[50];
  QString filename_b = idx->get_filename();
  Capacity host_disabled = config->get_disabled_capacities();
  Capacity ext_disabled  = get_ext_disabled_capacities(filename_b);
  Capacity song_disabled = idx->get_disabled_capacities();
  int decoder = get_decoder_program(host_disabled, ext_disabled, song_disabled);
  if (!decoder) return decoder;
  assert(decoder);
  set_used_decoder(decoder);
  sprintf(environment,"DECODER=%d",decoder);
  putenv(strdup(environment));
  return decoder;
}
#endif // __loaded__capacity_cpp__
