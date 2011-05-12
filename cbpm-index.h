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

/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/
extern           char   * index_readfrom;
extern           char   * index_version;
extern           int      index_period;
extern           char   * index_tempo;
extern           char   * index_file;
extern           char   * index_remark;
extern           char   * index_tags;
extern           int      index_changed;
extern           int      index_bpmcount_from;
extern           int      index_bpmcount_to;
extern           char   * index_md5sum;
extern           char   * index_time;
extern unsigned long      index_cue;
extern unsigned long      index_cue_z;
extern unsigned long      index_cue_x;
extern unsigned long      index_cue_c;
extern unsigned long      index_cue_v;

       void     index_init       ();
       void     index_setversion ();
       void     index_write      ();
       void     index_read       (const char*);
       void     index_free       ();

