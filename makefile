VERSION = 1.7
include defines

all: cbpm-count cbpm-play kbpm-play kbpm-dj beatmixing.ps

#############################################################################
# Rulesets
#############################################################################
%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.dvi: %.tex
	$(LATEX) $<

%.ps: %.dvi
	$(DVIPS) -o $@ $< 

%.tex: %.lyx
	$(LYX) -e latex $<

%.html: %.tex
	latex2html -split +0 -toc_depth 8 -info 0 -address "" -no_navigation $<

%.moc.cpp: %.h
	$(MOC) -o $@ $<

%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@ $(QT_INCLUDE_PATH)

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
	$(RM) cbpm-count cbpm-play kbpm-play kbpm-dj kbpm-count
	$(RM) *.moc.cpp *.ui.cpp *.ui.h
	$(RM) tagbox.h tagbox.cpp songplayer.h songplayer.cpp
	$(RM) bpmcounter.h bpmcounter.cpp
	$(RM) setupwizard.h setupwizard.cpp
	$(RM) scanningprogress.h scanningprogress.cpp
	$(RM) songselector.h songselector.cpp
	$(RM) about.h about.cpp legende.h legende.cpp
	$(RM) bpmbounds.h bpmbounds.cpp
	$(RM) askinput.h askinput.cpp loader.h loader.cpp
	$(RM) preferences.h preferences.cpp
	$(RM) profile-clock process_bpm.sh process_spectrum.sh
	$(RM) renamer.h renamer.cpp
	$(RM) similars.h similars.cpp
	$(RM) -r kbpmdj-$(VERSION)
	$(RM) -r beatmixing version.h

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

.PHONY: website allhtml all clean 
website: allhtml
	scp beatmixing/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs/

#############################################################################
# Command line version
#############################################################################
version.h: profile-clock
	echo "#define VERSION \""$(VERSION)"\"" >version.h
	./profile-clock >>version.h

cbpm-index.c: cbpm-index.h version.h
cbpm-play.c: cbpm-index.h version.h
cbpm-count.c: cbpm-index.h version.h

cbpm-count: cbpm-index.o cbpm-count.o
	$(CC) $^ -o $@

cbpm-play: cbpm-play.o cbpm-index.o player-core.o
	$(CC) $(LDFLAGS) $^ -o $@

#############################################################################
# Kde version
#############################################################################
KPLAY_OBJECTS = about.o\
	about.moc.o\
	songplayer.o\
	songplayer.moc.o\
	player-core.o\
	cbpm-index.o\
	kbpm-play.o\
	songplayerlogic.o\
	songplayerlogic.moc.o\
	bpmcounter.o\
	bpmcounter.moc.o\
	kbpm-counter.o\
	kbpm-counter.moc.o\
	fourierd.o\
	fftmisc.o

KCOUNT_OBJECTS = bpmcounter.o\
	bpmcounter.moc.o\
	cbpm-index.o\
	kbpm-count.o\
	kbpm-count.moc.o

KSEL_OBJECTS = qstring-factory.o\
	spectrum.o\
	about.o\
	about.moc.o\
	loader.o\
	loader.moc.o\
	askinput.o\
	askinput.moc.o\
	tagbox.o\
	tagbox.moc.o\
	scanningprogress.moc.o\
	scanningprogress.o\
	bpmbounds.moc.o\
	bpmbounds.o\
	songselector.o\
	songselector.moc.o\
	dirscanner.o\
	importscanner.o\
	songselector.logic.moc.o\
	songselector.logic.o\
	process-manager.cpp\
	preferences.o\
	preferences.moc.o\
	qsong.o\
	kbpm-index.o\
	cbpm-index.o\
	kbpm-played.o\
	setupwizard.moc.o\
	setupwizard.o\
	kbpm-dj.o\
	edit-distance.o\
	renamer.o\
	renamer.moc.o\
	renamer.logic.o\
	renamer.logic.moc.o\
	similars.o\
	similars.moc.o\
	similarscanner.o\
	similarscanner.moc.o\
	config.o

process-manager.cpp: process-manager.h
importscanner.h: dirscanner.h
dirscanner.o: dirscanner.h dirscanner.cpp
importscanner.o: importscanner.cpp importscanner.h dirscanner.h
similarscanner.h: similars.h
similars.cpp: similars.h	
renamer.logic.h: renamer.h
renamer.logic.cpp: renamer.logic.h
renamer.cpp: renamer.h
kbpm-play.cpp: bpmcounter.h version.h
bpmcounter.cpp: bpmcounter.h version.h
songselector.cpp: songselector.h version.h renamer.logic.h similarscanner.h
tagbox.cpp: tagbox.h
songselector.logic.h: songselector.h version.h
songselector.logic.cpp: songselector.logic.h about.h tagbox.h askinput.h\
	preferences.h version.h scanningprogress.h bpmbounds.h
preferences.logic.h: preferences.h version.h
preferences.logic.cpp: preferences.logic.h version.h
about.cpp: about.h version.h
loader.cpp: loader.h
askinput.cpp: askinput.h version.h
kbpm-dj.cpp: setupwizard.h
songplayer.cpp songplayer.h: songplayer.ui version.h
	$(UIC) -o songplayer.h songplayer.ui
	$(UIC) -i songplayer.h -o songplayer.cpp songplayer.ui

kbpm-play: $(KPLAY_OBJECTS)
	$(CC) $(KPLAY_OBJECTS) -o kbpm-play\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

kbpm-dj: $(KSEL_OBJECTS) songselector.cpp songselector.h
	$(CC) $(KSEL_OBJECTS) -o kbpm-dj\
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
	$(RM) -fR bpmdj-$(VERSION)/debian/tmp; exit 0
	$(RM) -fR bpmdj-$(VERSION)/*.listing
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
	$(MKDIR) $(DESTDIR)/usr/; exit 0
	$(MKDIR) $(DESTDIR)/usr/bin/; exit 0
	$(MKDIR) $(DESTDIR)/usr/share; exit 0
	$(MKDIR) $(DESTDIR)/usr/share/doc; exit 0
	$(MKDIR) $(DESTDIR)/usr/share/doc/bpmdj; exit 0
	$(CP) $(BIN) $(DESTDIR)/usr/bin/
	$(CP) $(DOC) $(DESTDIR)/usr/share/doc/bpmdj/



### CODE BELOW WRITTEN BY tsteudten@users.sourceforge.net ###################
# Setup for make redhat-dist 
#############################################################################

RPM_BASE = /usr/src/redhat

#############################################################################
# Phony targets
#############################################################################

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
