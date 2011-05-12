VERSION = 1.1
include defines

all: cbpm-count cbpm-play kbpm-play kbpm-dj beatmixing.ps
	
#############################################################################
# Rulesets
#############################################################################
%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.ps: %.lyx
	$(LYX) -e ps $<

%.tex: %.lyx
	$(LYX) -e latex $<

%.html: %.tex
	latex2html -split +0 -toc_depth 8 -info 0 -address "" -no_navigation $<

%.moc.cpp: %.h
	$(MOC) -o $@ $<

%.o: %.cpp
	$(CPP) -c $< $(QT_INCLUDE_PATH)

%.c: %.def
	$(AUTOGEN_PROGRAM) $(AUTOGEN_FLAGS) $^ 
	
%.cpp: %.ui
	$(UIC) -i `basename $< .ui`.h -o $@ $<

%.h: %.ui
	$(UIC) -o `basename $< .ui`.h $<

#############################################################################
# Cleanup section
#############################################################################
clean:
	$(RM) a.out core *.o *.log *.tex.dep *.toc *.dvi *.aux *.raw 
	$(RM) plot.ps gmon.out toplot.dat build sum.tmp fetchfiles.sh
	$(RM) -rf beatmixing beatmixing.ps
	$(RM) -f cbpm-count cbpm-play kbpm-play kbpm-dj
	$(RM) -rf kbpmdj-$(VERSION)
	$(RM) -f *.moc.cpp *.ui.cpp *.ui.h
	$(RM) -f songplayer.h songplayer.cpp
	$(RM) -f songedit.h songedit.cpp
	$(RM) -f songselector.h songselector.cpp
	$(RM) -f about.h about.cpp
	$(RM) -f askinput.h askinput.cpp
	$(RM) -f preferences.h preferences.cpp
	
mrproper: clean
	$(RM) *~ playlist.xmms played.log
	$(RM) debian/*~

#############################################################################
# Documentation
#############################################################################
beatmixing.html: beatmixing.tex

allhtml: beatmixing.html
	$(RM) beatmixing/index.html
	$(CP) index.html beatmixing/index.html
	$(CP) mixingdesk.jpg beatmixing/

website: allhtml
	$(SCP) beatmixing/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs

#############################################################################
# Command line version
#############################################################################

cbpm-index.c: cbpm-index.h
cbpm-play.c: cbpm-index.h
cbpm-count.c: cbpm-index.h

cbpm-count: cbpm-index.o cbpm-count.o
	$(CC) $^ -o $@
	
cbpm-play: cbpm-play.o cbpm-index.o player-core.o
	$(CC) $(LDFLAGS) $^ -o $@

#############################################################################
# Kde version
#############################################################################
KPLAY_OBJECTS = songplayer.o\
	songplayer.moc.o\
	player-core.o\
	cbpm-index.o\
	kbpm-play.o\
	songplayerlogic.o\
	songplayerlogic.moc.o
	
KSEL_OBJECTS = about.o\
	about.moc.o\
	askinput.o\
	askinput.moc.o\
	songedit.o\
	songedit.moc.o\
	songselector.o\
	songselector.moc.o\
	songselector.logic.moc.o\
	songselector.logic.o\
	preferences.o\
	preferences.moc.o\
	preferences.logic.o\
	preferences.logic.moc.o\
	qsongviewitem.o\
	kbpm-index.o\
	cbpm-index.o\
	kbpm-played.o\
	kbpm-dj.o\
	kbpm-md5.o

songselector.cpp: songselector.h

songselector.logic.h: songselector.h

songselector.logic.cpp: songselector.logic.h about.h songedit.h askinput.h\
	preferences.h

preferences.logic.h: preferences.h

preferences.logic.cpp: preferences.logic.h

about.cpp: about.h

askinput.cpp: askinput.h

songedit.cpp: songedit.h

songplayer.cpp songplayer.h: songplayer.ui
	$(UIC) -o songplayer.h songplayer.ui
	$(UIC) -i songplayer.h -o songplayer.cpp songplayer.ui

kbpm-play: $(KPLAY_OBJECTS)
	$(CPP) $(KPLAY_OBJECTS) -o kbpm-play\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

kbpm-dj: $(KSEL_OBJECTS)
	$(CPP) $(KSEL_OBJECTS) -o kbpm-dj\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

#############################################################################
# Distributions
#############################################################################
DOC = authors changelog copyright todo beatmixing.ps readme
BIN  = cbpm-count cbpm-play kbpm-play kbpm-dj glue-bpmraw glue-mp3raw\
	rbpm-play xmms-play
GOALS = $(DOC) $(BIN)
	
.PHONY: directories

directories: mrproper
	$(RM) -r bpmdj-$(VERSION); exit 0
	$(MKDIR) bpmdj-$(VERSION)
	$(MKDIR) bpmdj-$(VERSION)/music
	$(MKDIR) bpmdj-$(VERSION)/index

source.tgz-dist: directories
	$(CP) -f * bpmdj-$(VERSION); exit 0
	$(CP) -fR debian bpmdj-$(VERSION); exit 0
	$(TAR) -cvzf bpmdj-$(VERSION).source.tgz bpmdj-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r bpmdj-$(VERSION); exit 0

bin.tgz-dist: directories
	$(MAKE) all
	$(STRIP) $(BIN); exit 0
	$(CP) -f $(GOALS) bpmdj-$(VERSION); exit 0
	$(TAR) -cvzf bpmdj-$(VERSION).bin.tgz bpmdj-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r bpmdj-$(VERSION); exit 0

tgz-dist: 
	$(MAKE) source.tgz-dist 
	$(MAKE) bin.tgz-dist
	
deb-dist: 
	fakeroot debian/rules binary

deb-install: all
	$(CP) $(BIN) $(DESTDIR)/usr/bin/
	$(CP) $(DOC) $(DESTDIR)/usr/share/doc/bpmdj/



######################################### TO CLEAN UP !!! #############################################################################
# Setup for make redhat-dist 
#############################################################################

RPM_BASE = /usr/src/redhat

#############################################################################
# Phony targets
#############################################################################
.PHONY: wvb website  redhat-dist allhtml all clean clean-bin tags install uninstall 

wvb: clobber 
	cd ..; $(TAR) cvzf bpmdj-$(VERSION).tgz bpmdj 
	$(SCP) ../bpmdj-$(VERSION).tgz  werner@asus:

install: install_bin install_doc

install_bin:
	for  bin in $(TARGETS) $(INST_TARGETS); do \
	   $(INSTALL) -D -m 755 $$bin ${ROOT}$(BINDIR)/$$bin; \
	done

install_doc: 
	for doc in $(DOC); do \
	   $(INSTALL) -D -m 755 $$doc ${ROOT}$(DOCDIR)/$$doc; \
	done

uninstall: 
	for bin in $(TARGETS) $(INST_TARGETS); do \
	   $(RM) ${ROOT}$(BINDIR)/$$bin; \
	done \
	for doc in $(DOC); do \
	   $(RM) ${ROOT}$(DOCDIR)/$$doc; \
	done

#############################################################################
# redhat distribution alpha and x86
#############################################################################

redhat-dist:
	@for dir in RPMS SRPMS BUILD SOURCES SPECS; do \
		if [ ! -w $(RPM_BASE)/$$dir ]; then \
			$(ECHO) "$(RPM_BASE)/$$dir is not writable for you. Maybe try as root."; \
			exit; \
		fi; \
	done ; \
	$(MAKE) clean; \
	$(MKDIR) bpmdj-$(VERSION); \
	[ -f $(TAR_ARCH) ] && $(RM) $(TAR_ARCH); \
	$(CP) `find . -maxdepth 1 -type f -print` bpmdj-$(VERSION) ; \
	$(CAT) bpmdj.spec | \
		$(SED) "s/_VERSION_/$(VERSION)/g" > bpmdj-$(VERSION)/bpmdj.spec; \
	$(TAR) --exclude "CVS" -cvzf $(TAR_ARCH) bpmdj-$(VERSION) ; \
	$(RM) -r bpmdj-$(VERSION); \
	$(RPM) --clean -ta  $(TAR_ARCH); \
	[ -f $(TAR_ARCH) ] && $(RM) $(TAR_ARCH)
