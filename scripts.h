/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__scripts_h__
#define __loaded__scripts_h__
using namespace std;
#line 1 "scripts.h++"
#include <stdio.h>
#include "index.h"
#include "common.h"

// adding new extensions can be done in dirscanner.cpp
#define RAW_EXT ".raw"
#define IDX_EXT ".idx"
#define SHELL_HEADER "#!/bin/bash\n"
// commands
#define CDROM "/cdrom"
#define RAW2WAV "sox -r 44100 -s -w -c 2 "
#define BPLAY "bplay -s 44100 -S -b 16 "

// filenames
#define PROCESS_MIX "./process_mix.sh"
#define MIXDUMP_NAME "mix.raw"
#define TAILDUMP_NAME "tail.raw"
#define HEADDUMP_NAME "head.raw"
#define BPMMIXED_NAME "bpm-mixed.raw"

char * getRawFilename(const char* dir, const char* name);
FILE * openRawFile(Index* index, const char* dir);
FILE * openScriptFile(const char* name);
FILE * openRawFileForWriting(Index* index, const char* dir);
void   removeRawFile(Index* index, const char* dir);
void   removeAllRaw(const char* dir);
void   removeAllLog();
void   dumpAudio(const char* fname, unsigned4 * buffer, long length);
void   spawn(const char* script);
bool   execute(const char* script);
bool   vexecute(const char* script,...);
int    bpmdj_fork();

// external programs
int    start_bpmdj_raw(const char* where, const char* file);
void   start_mkdir(const char* dir);
void   start_cp(const char* from, const char* to);
int    start_mv(const char* from, const char* to);
void   start_rm(const char* what);
int    start_mount_cdrom();
int    start_umount_cdrom(bool eject = true);

// logging, warning, and other similar functions
void Log(const char* prefix, const char* text);
void Debug(const char* script,...);
void Info(const char* script,...);
void Warning(const char* script,...);
void Error(bool ui, const char* script,...);
void Fatal(const char* script,...);
void Remote(const char* script,...);

// this function returns a new string with all characters escaped as
// necessary, if passing through to a remote shell double escaping might
// be necessary.
char * escape(const char * in);
#endif // __loaded__scripts_h__
