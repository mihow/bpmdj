VERSION = 2.2
include defines
BIN = kbpm-play kbpm-dj merger bpmdj-raw rbpm-play xmms-play alsamixerguis
DOC = authors changelog copyright todo readme ${shell ls *.html *.png *.css *.pdf}
UIS = ${shell ls *.ui}

all: $(BIN)

#############################################################################
# Rules
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
	$(CPP) -c $< $(CFLAGS) -o $@ $(QT_INCLUDE_PATH)

%.cpp: %.ui
	$(UIC) -i `basename $< .ui`.h -o $@ $<

%.h: %.ui
	$(UIC) -o `basename $< .ui`.h $<

#############################################################################
# Cleanup section
#############################################################################
rc: 
	$(RM) *.raw
	killall mpg123

clean: 
	$(RM) a.out core *.o *.log *.tex.dep *.toc *.dvi *.aux
	$(RM) plot.ps gmon.out toplot.dat build sum.tmp fetchfiles.sh
	$(RM) cbpm-count cbpm-play kbpm-play kbpm-dj kbpm-count merger
	$(RM) *.moc.cpp *.ui.cpp *.ui.h 
	$(RM) tagbox.h tagbox.cpp songplayer.h songplayer.cpp
	$(RM) bpmcounter.h bpmcounter.cpp
	$(RM) setupwizard.h setupwizard.cpp
	$(RM) scanningprogress.h scanningprogress.cpp
	$(RM) songselector.h songselector.cpp
	$(RM) about.h about.cpp legende.h legende.cpp
	$(RM) askinput.h askinput.cpp loader.h loader.cpp
	$(RM) merger-dialog.h merger-dialog.cpp
	$(RM) preferences.h preferences.cpp
	$(RM) profile-clock process_bpm.sh process_spectrum.sh analyze.sh
	$(RM) renamer.h renamer.cpp
	$(RM) similars.h similars.cpp
	$(RM) -r kbpmdj-$(VERSION)
	$(RM) -r beatmixing version.h

mrproper: clean
	$(RM) *~ playlist.xmms played.log
	$(RM) \#*\#
	$(RM) debian/*~ depends

#############################################################################
# Installation & Documentation
#############################################################################
.PHONY: website install uninstall all clean directories

install: 
	for bin in $(BIN); do $(INSTALL) -D -m 755 $$bin ${DESTDIR}/usr/bin/$$bin; done
	for doc in $(DOC); do $(INSTALL) -D -m 755 $$doc ${DESTDIR}/usr/share/doc/bpmdj/$$doc; done

uninstall: 
	for bin in $(BIN); do $(RM) ${DESTDIR}/usr/bin/$$bin; done
	for doc in $(DOC); do $(RM) ${DESTDIR}/usr/share/doc/bpmdj/$$doc; done

website: 
	rsync -e ssh -xavz *.html *.css *.png *.pdf krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs/

nancy: nens
nens:	
	tar -cvzf index.tgz index
	scp index.tgz root@tnm:/home/nens/
	mv index.tgz ..

#############################################################################
# Dependencies
#############################################################################
version.h: profile-clock
	echo "#define VERSION \""$(VERSION)"\"" >version.h
	./profile-clock >>version.h

depend: $(UIS:%.ui=%.h) $(UIS:%.ui=%.cpp) version.h
	$(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp *.c >depend
include depend


#############################################################################
# Objects
#############################################################################
KPLAY_OBJECTS = about.o about.moc.o\
	songplayer.o songplayer.moc.o\
	player-core.o dsp-drivers.o dsp-oss.o dsp-alsa.o\
	song-information.o song-information.moc.o\
	index.o kbpm-play.o analyzer.o\
	songplayer.logic.o songplayer.logic.moc.o\
	md5-analyzer.o\
	bpmcounter.o       bpmcounter.moc.o       bpm-analyzer.logic.o     bpm-analyzer.logic.moc.o\
	pattern-analyzer.o pattern-analyzer.moc.o pattern-analyzer.logic.o pattern-analyzer.logic.moc.o\
	spectrumanalyzer.o spectrumanalyzer.moc.o spectrumanalyzer.logic.o spectrumanalyzer.logic.moc.o\
	impulseanalyzer.o impulseanalyzer.moc.o\
	tempolineanalyzer.o tempolineanalyzer.moc.o\
	tempolineanalyzer.logic.o tempolineanalyzer.logic.moc.o\
	impulseanalyzer.logic.o impulseanalyzer.logic.moc.o\
	fourierd.o\
	fftmisc.o\
	common.o\
	scripts.o

KCOUNT_OBJECTS = bpmcounter.o bpmcounter.moc.o\
	index.o kbpm-count.o kbpm-count.moc.o\
	scripts.o merger.o

avltree-test:
	g++ -pg -g avltree-test.cpp

KSEL_OBJECTS = 	avltree.o songtree.o qstring-factory.o spectrum.o\
	scripts.o cluster.o pca.o\
	about.o about.moc.o\
	loader.o loader.moc.o\
	mixinformation.o mixinformation.moc.o\
	askinput.o askinput.moc.o\
	tagbox.o tagbox.moc.o\
	song-information.o song-information.moc.o\
	scanningprogress.o scanningprogress.moc.o\
	choose-analyzers.o choose-analyzers.moc.o\
	database.o dirscanner.o importscanner.o\
	songselector.o songselector.moc.o songselector.logic.o songselector.logic.moc.o\
	process-manager.o playercommandwizard.o playercommandwizard.moc.o\
	preferences.o preferences.moc.o preferences.logic.o preferences.logic.moc.o\
	song.o qsong.o queuedsong.o historysong.o\
	index-reader.o index.o kbpm-played.o\
	setupwizard.moc.o setupwizard.o kbpm-dj.o edit-distance.o\
	renamer.o renamer.moc.o renamer.logic.o renamer.logic.moc.o\
	similars.o similars.moc.o similarscanner.o similarscanner.moc.o\
	config.o merger-dialog.o merger-dialog.moc.o common.o

MERGER_OBJECTS = merger.o index.o common.o scripts.o song-information.o song-information.moc.o

#############################################################################
# Binaries
#############################################################################
#cbpm-play: cbpm-play.o index.o player-core.o common.o scripts.o dsp-drivers.o dsp-oss.o dsp-alsa.o
#	$(CPP) $(CFLAGS) $(LDFLAGS) $^ -o $@

kbpm-play: $(KPLAY_OBJECTS)
	$(CPP) $(KPLAY_OBJECTS) -o kbpm-play\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

kbpm-dj: $(KSEL_OBJECTS) songselector.cpp songselector.h
	$(CPP) $(KSEL_OBJECTS) -o kbpm-dj\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

merger: $(MERGER_OBJECTS)
	$(CPP) $(MERGER_OBJECTS) -o merger\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

#############################################################################
# Distributions
#############################################################################
GOALS = $(DOC) $(BIN)

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
	$(RM) -f depend
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
