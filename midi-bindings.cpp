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
#ifndef __loaded__midi_bindings_cpp__
#define __loaded__midi_bindings_cpp__
using namespace std;
#line 1 "midi-bindings.c++"
#ifdef MIDI
#include <cstdio>
#include "midi-bindings.h"
#include "bpmplay.h"
#include <alsa/asoundlib.h>
#include <stdio.h>

#define slot(_slot) \
  void slot##_slot(Player *p, int i) {		\
  p->_slot();\
  }

#define slot1(_slot) \
  void slot##_slot(Player *p, int i) {		\
  p->_slot(i);\
  }

#define make_action(_slot) \
  if (!strcmp(slot,__STRING(_slot))) {		\
  action[i][value] = slot##_slot;\
  }

slot(retrieveZ);
slot(retrieveX);
slot(retrieveC);
slot(retrieveV);
slot(start_stop);
slot(nudgePlus);
slot(nudgeMinus);
slot(mediumSwitch);
slot1(loop);
slot1(nudge);

midi_bindings::midi_bindings(Player *player) 
{
  p = player;

  /* Simple test implementation: we assume that midi bindings are
     given in a text file located in the current dir, named
     config-midi.cfg, where config is the config argument */
  
  bool valid_bindings = false;
  char filename[100];
  char event[100];
  int value;
  char slot[100];
  sprintf(filename, "%s-midi.cfg", arg_config);
  FILE *f = fopen(filename, "r");
  for (int k = 0; k < 256; k++)
    for (int l = 0; l < 256; l++)
      action[k][l] = NULL;
  if (f) {
    while (!feof(f)) {
      if (fscanf(f, " %s %d %s ", &event, &value, &slot) == 3) {
	int i = 255;
        if (!strcmp(event,"noteon")) {
	  i = SND_SEQ_EVENT_NOTEON;
	}
        if (!strcmp(event,"noteoff")) {
	  i = SND_SEQ_EVENT_NOTEOFF;
	}
	if (!strcmp(event,"controller")) {
	  i = SND_SEQ_EVENT_CONTROLLER;
	}
	if (i != 255) {
	  make_action(retrieveZ);
	  make_action(retrieveX);
	  make_action(retrieveC);
	  make_action(retrieveV);
	  make_action(start_stop);
	  make_action(nudgePlus);
	  make_action(nudgeMinus);
	  make_action(mediumSwitch);
	  make_action(loop);
	  make_action(nudge);
	}
      }
    } 
    fclose(f);
    valid_bindings = true;
  }
  if (valid_bindings) {
    pthread_t *jt = bpmdj_allocate(1,pthread_t);
    pthread_create(jt,NULL,process_midi,this);
  }
}

void * process_midi(void* bindings)
{
  midi_bindings *b = (midi_bindings *) bindings;
  snd_seq_t *seq_handle;
  int npfd;
  struct pollfd *pfd;
  snd_seq_event_t *ev;
  int portid;

  /* opening handle */

  if (snd_seq_open(&seq_handle, "hw", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
    fprintf(stderr, "Error opening ALSA sequencer.\n");
    exit(1);
  }
  snd_seq_set_client_name(seq_handle, "BPM Play");

  /* creating port */

  if ((portid = snd_seq_create_simple_port(seq_handle, "BPM Play",
					   SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
					   SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
    fprintf(stderr, "Error creating sequencer port.\n");
    exit(1);
  }
    

  npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
  pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
  snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
  while (1) {
    if (poll(pfd, npfd, 100000) > 0) {
      do {
	int j = 255;
	snd_seq_event_input(seq_handle, &ev);	
	if ((ev->type == SND_SEQ_EVENT_NOTEON) 
	    && (ev->data.note.velocity == 0))
	  ev->type = SND_SEQ_EVENT_NOTEOFF;
	switch (ev->type) {
	case SND_SEQ_EVENT_CONTROLLER: 
	  j = ev->data.control.param;
	  break;
	case SND_SEQ_EVENT_PITCHBEND:
	  /* fprintf(stderr, "Pitchbender event on Channel %2d: %5d   \r", 
	     ev->data.control.channel, ev->data.control.value);*/
	  break;
	case SND_SEQ_EVENT_NOTEON:	 
	  j = ev->data.note.note;
	  break;        
	case SND_SEQ_EVENT_NOTEOFF: 
	  j = ev->data.note.note;
	  break;        
	}
	if ((j != 255) && b->binding(ev->type,j)) {
	  b->binding(ev->type,j)(b->player(), ev->data.control.value);
	}
       
	snd_seq_free_event(ev);
      } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
    }
  }  
}
#endif
#endif // __loaded__midi_bindings_cpp__
