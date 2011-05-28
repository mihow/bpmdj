/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__song_information_cpp__
#define __loaded__song_information_cpp__
using namespace std;
#line 1 "song-information.c++"
#include <cstdio>
#include "index.h"
#include "song-information.h"

void SongInformation::updateDataFrom(Index * where)
{
  showing=where;
  if (!where) return;
  idxLabel->setText(showing->meta_filename);
  md5Label->setText(showing->index_md5sum);
  titleEdit->setText(showing->title);
  authorEdit->setText(showing->author);
  versionEdit->setText(showing->version);
  remixEdit->setText(showing->remix);
  tagEdit->setText(showing->index_tags);
  old_disabled_capacity = showing->index_disabled_capacities;
  location->setText(showing->index_file);
  init_capacity_widget(capacity,old_disabled_capacity);
}

void SongInformation::on_save_clicked()
{
#define field2this(namea,nameb) if (namea##Edit->text()!=showing->nameb) \
  { showing->nameb = namea##Edit->text(); \
    showing->meta_changed = true; }

  if (!showing) return;
  // printf("Saving an index file\n");
  field2this(title, title);
  field2this(author, author);
  field2this(remix, remix);
  field2this(version, version);
  field2this(tag, index_tags);
  showing->index_disabled_capacities = get_capacity(capacity);
  showing->meta_changed |= old_disabled_capacity!=showing->
    index_disabled_capacities;
}
#endif // __loaded__song_information_cpp__
