/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__scripts_h__
#define __loaded__scripts_h__
using namespace std;
#line 1 "scripts.h++"
#include <qstring.h>
#include <stdio.h>
#include "index.h"
#include "process.h"
#include "common.h"

// adding new extensions can be done in dirscanner.cpp
#define RAW_EXT ".raw"
#define IDX_EXT ".idx"
#define SHELL_HEADER "#!/bin/bash\n"
// commands
#define RAW2WAV "sox -r 44100 -s -w -c 2 "
#define BPLAY "bplay -s 44100 -S -b 16 "

/**
 * The SSH prefix requires some attention. 
 * -X makes the target to forward the X events
 * -T disables the allocation of a pseudotty. This was the reason of bug #1047
 * -e none disables the presence of an escape character.
 */
#define SSH " ssh -X -T -e none "

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

/**
 * Will start bpmdjraw and wait for the result or not depending on the 
 * synchronous flag. The return value is the process id in the asynchronous
 * case, 0 when there was an error. The location to place the rawfile is 
 * obtained through the raw_location() function call.
 */
int bpmdjraw(bool synchronous, const char* fname, const char* where);
int bpmdjraw(bool synchronous, const char* fname, QString where);

void start_mkdir(const QString& dir);
void start_cp(const QString& from, const QString& to);
int  start_mv(const QString& from, const QString& to);
void start_rm(const QString& what);
void remove(QString str);

/**
 * This function returns a new string with all characters escaped as
 * necessary, if passing through to a remote shell double escaping might
 * be necessary. For instance when trying to pass a parameter through
 * a secure shell command.
 */
char *escape(const char * in);
QString escape(const QString& in);

/**
 * executes the given command. Returns a simple true
 * if successful, false otherwise. The description will be
 * printed to the terminal. The script will be executed.
 */
bool execute(QString description, QString script);
bool vexecute(const char* script,...);

#endif // __loaded__scripts_h__
