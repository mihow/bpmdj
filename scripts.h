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
#include "common.h"

// adding new extensions can be done in dirscanner.cpp
#define RAW_EXT ".raw"
#define IDX_EXT ".idx"
#define SONG_EXT ".mp3 .ogg"
#define SHELL_HEADER "#!/bin/bash\n"
// commands
#define CDROM "/cdrom"
#define UMOUNT "umount"
#define MOUNT "mount"
#define EJECT "eject"
#define RM "rm -- "
#define CP "cp -- "
#define MV "mv -i -- "
#define CREATERAW_CMD "bpmdj-raw \"%s\"\n" 
#define ENCODE "bladeenc -del *.wav"
#define RAW2WAV "sox -r 44100 -s -w -c 2 "
#define BPLAY "bplay -s 44100 -S -b 16 "
#define INSERT_CD_DIALOG "dialog --msgbox \"Please insert %s\" 10 40\n"
// composite commands
#define MOUNT_CDROM MOUNT " " CDROM "\n"
#define UMOUNT_CDROM UMOUNT " " CDROM "\n"
#define EJECT_CDROM EJECT " " CDROM "\n"
// filenames
#define FETCHFILES "./fetchfiles.sh"
#define PROCESS_BPM "./process_bpm.sh"
#define PROCESS_SPECTRUM "./process_spectrum.sh"
#define PROCESS_MIX "./process_mix.sh"
#define MIXDUMP_NAME "mix.raw"
#define TAILDUMP_NAME "tail.raw"
#define HEADDUMP_NAME "head.raw"
#define BPMMIXED_NAME "bpm-mixed.raw"

char * getRawFilename(const char* name);
FILE * openRawFile();
FILE * openScriptFile(const char* name);
FILE * openRawFileForWriting();
void   removeRawFile();
void   removeAllRaw();
char * getMd5(const char* argument);
void   dumpAudio(const char* fname, unsigned4 * buffer, long length);
void   spawn(const char* script);
int    execute(const char* script);
int    vexecute(const char* script,...);
