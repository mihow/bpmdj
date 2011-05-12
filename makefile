VERSION        = 1.0
MAJOR_VERSION  = 1
MINOR_VERSION  = 0

AUTOGEN_PROGRAM=$(shell autoopts-config --autogen)
AUTOGEN_DATADIR=$(shell autoopts-config --pkgdatadir)
AUTOGEN_FLAGS=-L$(AUTOGEN_DATADIR) 
AUTOGEN_CFLAGS=$(shell autoopts-config --cflags)
AUTOGEN_LDFLAGS=$(shell autoopts-config --libs)
CFLAGS+=$(AUTOGEN_CFLAGS)
LDFLAGS+=$(AUTOGEN_LDFLAGS) -lpthread
QT_INCLUDE_PATH=-I/usr/include/qt/
QT_LIBRARY_PATH=-L/usr/X11R6/lib/
QT_LIBS=-lqt -lX11 -lXext
CPP=g++ -O0 -g -pg 
CC=gcc -O0 -g -pg 
GOALS = kbpm-play
PROJECT = kBpmPlay
MKDIR = mkdir 
CP = cp
MV = mv
TAR = tar

%.moc.cpp: %.h
	moc -o $@ $<

%.o: %.cpp
	$(CPP) -c $< -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $(QT_INCLUDE_PATH)

%.c: %.def
	$(AUTOGEN_PROGRAM) $(AUTOGEN_FLAGS) $^ 
	
OBJECTS = songplayer.o\
	songplayer.moc.o\
	cbpm-index.o\
	kbpm-playeropts.o\
	kbpm-play.o\
	songplayerlogic.o\
	songplayerlogic.moc.o
#	about.ui.moc.o\
#	songedit.ui.o\
#	songedit.ui.moc.o\
#	songselector.ui.o\
#	songselector.ui.moc.o\
#	songselector.logic.moc.o\
#	songselector.logic.o\
#	preferences.ui.o\
#	preferences.ui.moc.o\
#	preferences.logic.o\
#	preferences.logic.moc.o\
#	qsongviewitem.o\
#	kbpm-index.o\
#	kbpm-played.o\
#	kbpm-dj.o\
#	kbpm-md5.o

all: kbpm-play

songplayer.cpp songplayer.h: songplayer.ui
	uic -o songplayer.h songplayer.ui
	uic -i songplayer.h -o songplayer.cpp songplayer.ui

kbpm-play: $(OBJECTS)
	$(CPP) $(OBJECTS) -o kbpm-play\
	  -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION)\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

songselector.logic.h: songselector.ui.h about.ui.h songedit.ui.h preferences.ui.h

indextest: cbpm-index.o
	 $(CC) kbpm-indextest.c cbpm-index.o \
	  -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION)

cbpm-index.o: 
	 $(CC) -I../cBpmDj/ -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -c ../cBpmDj/cbpm-index.c -o cbpm-index.o

clean:
	rm -f *.o
	rm -f *.moc.cpp
	rm -f *.ui.cpp
	rm -f *.ui.h
	rm -f a.out build kbpm-play
	rm -rf kbpmdj-$(VERSION)
	rm -f gmon.out played.log fetchfiles.sh
	rm -f kbpm-playeropts.h songplayer.h songplayer.cpp

deb-install: all
	cp kbpm-play $(DESTDIR)/usr/bin/

deb-dist:
	fakeroot debian/rules binary


directories: clean
	$(RM) -r $(PROJECT)-$(VERSION); exit 0
	$(MKDIR) $(PROJECT)-$(VERSION)
	$(MKDIR) $(PROJECT)-$(VERSION)/music
	$(MKDIR) $(PROJECT)-$(VERSION)/index

source.tgz-dist: directories
	$(CP) -f * $(PROJECT)-$(VERSION); exit 0
	$(TAR) -cvzf $(PROJECT)-$(VERSION).source.tgz $(PROJECT)-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r $(PROJECT)-$(VERSION); exit 0

shared.bin.tgz-dist: directories
	$(MAKE) all
	$(CP) -f $(GOALS) $(PROJECT)-$(VERSION); exit 0
	$(TAR) -cvzf $(PROJECT)-$(VERSION).shared.bin.tgz $(PROJECT)-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r $(PROJECT)-$(VERSION); exit 0

tgz-dist: 
	$(MAKE) source.tgz-dist 
	$(MAKE) shared.bin.tgz-dist 

