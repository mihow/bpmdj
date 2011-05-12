DESTDIR = /usr/local/
VERSION = 2.5

include defines
BIN = kbpm-play kbpm-dj merger bpmdj-raw rbpm-play xmms-play alsamixerguis \
      bpmdj-replay bpmdj-record record_mixer bpmdj-import-mp3.pl bpmdj-import-ogg.pl
DOC = authors changelog copyright readme
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
	$(RM) *.moc.cpp *.ui.cpp *.ui.h  pattern-analyzer.cpp
	$(RM) songplayer.h songplayer.cpp songproperties.cpp
	$(RM) albumbox.h albumbox.cpp impulseanalyzer.cpp
	$(RM) bpmcounter.h bpmcounter.cpp playercommandwizard.cpp
	$(RM) setupwizard.h setupwizard.cpp song-information.cpp
	$(RM) scanningprogress.h scanningprogress.cpp
	$(RM) songselector.h songselector.cpp edit.cpp
	$(RM) about.h about.cpp legende.h legende.cpp duploc-dialog.h
	$(RM) loader.h loader.cpp choose-analyzers.cpp duploc-dialog.cpp
	$(RM) compacter.h compacter.cpp spectrumanalyzer.cpp
	$(RM) merger-dialog.h merger-dialog.cpp songproperties.h
	$(RM) preferences.h preferences.cpp tempolineanalyzer.cpp
	$(RM) profile-clock process_bpm.sh process_spectrum.sh analyze.sh
	$(RM) renamer.h renamer.cpp choose-analyzers.h spectrumanalyzer.h
	$(RM) similars.h similars.cpp song-information.h edit.h
	$(RM) playercommandwizard.h pattern-analyzer.h impulseanalyzer.h 
	$(RM) choose-analyers.h tempolineanalyzer.h
	$(RM) -r kbpmdj-$(VERSION)
	$(RM) -r beatmixing version.h

mrproper: clean
	$(RM) *~ playlist.xmms played.log
	$(RM) \#*\#
	$(RM) debian/*~ depends

#############################################################################
# Installation & Documentation
#############################################################################
.PHONY: website all clean files

files:
	@echo Checking filelist
	@rm -r tmp 2>/dev/null; exit 0
	@find . -iname "*" | grep -v ^\./index | grep -v ^\./music > existing_files # list of existing files
	@grep \  existing_files > problem_files; exit 0 # problematic files
	@test ! -s problem_files
	@gawk '{print $$1}' files >known_files # list of known files
	@grep -vxFf known_files existing_files >unknown_files; exit 0 # unknown files
	@gawk '{print $$1"  ---"}' unknown_files >>files # attach these to the files
	@gawk '{if ($$2=="---") print $1}' files >untagged_files; exit 0 # find files missing tags
	@if test -s untagged_files; then cat untagged_files; exit 1; fi

source_files: files
	@echo Creating list of sources
	@gawk '{if (index($$2,"source")) print $$1}' files >source_files 

install_files: files
	@echo Creating list of install files
	@gawk '{if (index($$2,"install")) print $$1}' files >install_files

documentation_files: files
	@echo Creating list of documentation files
	@gawk '{if (index($$2,"doc")) print $$1}' files >documentation_files 

support_files: files
	@echo Creating list of support files
	@gawk '{if (index($$2,"support")) print $$1}' files >support_files

temp_files: files
	@echo Creating list of temporary files
	@gawk '{if (index($$2,"temp")) print $$1}' files >temp_files

bpmdj-source.tgz: source_files
	@echo Creating source bundle
	@tar -cz --no-recursion --ignore-failed-read -T source_files -f bpmdj-source.tgz  2>/dev/null 

bpmdj-bin.tgz: install_files
	@echo Creating install bundle
	@tar -cz --no-recursion --ignore-failed-read -T install_files -f bpmdj-bin.tgz 2>/dev/null 

bpmdj-doc.tgz: documentation_files
	@echo Creating documentation bundle
	@tar -cz --no-recursion --ignore-failed-read -T documentation_files -f bpmdj-doc.tgz 2>/dev/null 

bpmdj-support.tgz: support_files
	@echo Creating support bundle
	@tar -cz --no-recursion --ignore-failed-read -T support_files -f bpmdj-support.tgz 2>/dev/null 

check-bin: bpmdj-source.tgz
	mkdir tmp
	cd tmp; tar -xzf ../bpmdj-source.tgz; make -j 12
	rm -r tmp

check-doc: bpmdj-doc.tgz
	mkdir tmp
	cd tmp; tar -xzf ../bpmdj-doc.tgz
	cd tmp/documentation; for a in *.html; do cat $$a >>all; done
	cd tmp/documentation; for a in *.png; do echo -n $$a "  :::"; grep -c $$a all; done  >../../count
	grep :::0 count >bpmdj-doc-errors.txt; exit 0
	linkchecker -s -r -1 tmp/documentation/index.html -ohtml >bpmdj-doc-errors.html
	sed 's/\/tmp\//\//' bpmdj-doc-errors.html >bpmdj-doc-errors2.html
	rm -r tmp
	test ! -s bpmdj-doc-errors.txt

look:
	mozilla file://`pwd`/bpmdj-doc-errors2.html &
	emacs bpmdj-doc-errors.txt &

install: bpmdj-bin.tgz
	mkdir -p $(DESTDIR)/bpmdj;
	cp bpmdj-bin.tgz $(DESTDIR)/bpmdj
	cd $(DESTDIR)/bpmdj; tar -xzf bpmdj-bin.tgz
	rm $(DESTDIR)/bpmdj/bpmdj-bin.tgz

source.tgz-dist: bpmdj-source.tgz
	@echo Creating bpmdj-$(VERSION).source.tgz
	@$(MKDIR) bpmdj-$(VERSION)
	@$(CD) bpmdj-$(VERSION); tar -xzf ../bpmdj-source.tgz
	@$(TAR) -czf bpmdj-$(VERSION).source.tgz bpmdj-$(VERSION)
	@$(RM) -r bpmdj-$(VERSION)


bin.tgz-dist: bpmdj-bin.tgz
	@echo Creating bpmdj-$(VERSION).bin.tgz
	@$(MKDIR) bpmdj-$(VERSION)
	@$(CD) bpmdj-$(VERSION); tar -xzf ../bpmdj-bin.tgz
	@$(TAR) -czf bpmdj-$(VERSION).bin.tgz bpmdj-$(VERSION)
	@$(RM) -r bpmdj-$(VERSION)


doc.tgz-dist: bpmdj-doc.tgz
	@echo Creating bpmdj-$(VERSION).doc.tgz
	@$(MKDIR) bpmdj-$(VERSION)
	@$(CD) bpmdj-$(VERSION); tar -xzf ../bpmdj-doc.tgz
	@$(TAR) -czf bpmdj-$(VERSION).doc.tgz bpmdj-$(VERSION)
	@$(RM) -r bpmdj-$(VERSION)

tgz: source.tgz-dist doc.tgz-dist bin.tgz-dist bpmdj-support.tgz
	@echo Finished
	@$(MV) bpmdj-support.tgz ../bpmdj-$(VERSION).support.tgz
	@$(MV) *$(VERSION)*tgz ..
	@rm *.tgz

website: 
	rsync -e ssh -xavz documentation/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs/

nancy: nens
nens:	
	tar -cvzf index.tgz index
	scp index.tgz root@orbit:/home/nens/; exit 0
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
VECTOR_OBJECTS = qvectorview.o qvectorview.moc.o vector-view.o vector-view.moc.o

KPLAY_OBJECTS = avltree.o about.o about.moc.o\
	songplayer.o songplayer.moc.o\
	player-core.o dsp-oss.o dsp-alsa.o dsp-none.o\
	song-information.o song-information.moc.o\
	index.o kbpm-play.o analyzer.o\
	songplayer.logic.o songplayer.logic.moc.o\
	md5-analyzer.o efence.o page.o efence-print.o\
	bpmcounter.o bpmcounter.moc.o bpm-analyzer.logic.o bpm-analyzer.logic.moc.o\
	pattern-analyzer.o pattern-analyzer.moc.o pattern-analyzer.logic.o pattern-analyzer.logic.moc.o\
	spectrumanalyzer.o spectrumanalyzer.moc.o spectrumanalyzer.logic.o spectrumanalyzer.logic.moc.o\
	pattern-filter.logic.o   pattern-rythm.logic.o\
	fourierd.o\
	fftmisc.o\
	common.o\
	scripts.o

KCOUNT_OBJECTS = bpmcounter.o bpmcounter.moc.o\
	index.o kbpm-count.o kbpm-count.moc.o\
	scripts.o merger.o

DUPLOC_OBJECTS = duploc-dialog.o duploc-dialog.moc.o\
	duploc.o duploc.moc.o

FILETAGS_OBJECTS = filetags.o filetags.moc.o filetags.logic.o filetags.logic.moc.o\
	dirscanner.o

UNPHASER_OBJECTS = unphaser.o common.o fourierd.o fftmisc.o

avltree-test:
	g++ -pg -g avltree-test.cpp

KSEL_OBJECTS = 	avltree.o songtree.o qstring-factory.o tags.o\
	spectrum.o scripts.o cluster.o pca.o about.o about.moc.o\
	loader.o loader.moc.o compacter.o compacter.moc.o\
	qvectorview.o qvectorview.moc.o freq-mapping.o freq-mapping.moc.o\
	albumbox.o albumbox.moc.o compact-idx.o\
	song-information.o song-information.moc.o\
	scanningprogress.o scanningprogress.moc.o\
	choose-analyzers.o choose-analyzers.moc.o\
	database.o dirscanner.o importscanner.o efence.o page.o efence-print.o\
	songselector.o songselector.moc.o songselector.logic.o songselector.logic.moc.o\
	process-manager.o playercommandwizard.o playercommandwizard.moc.o\
	preferences.o preferences.moc.o preferences.logic.o preferences.logic.moc.o\
	song.o qsong.o queuedsong.o historysong.o\
	index-reader.o index.o history.o albumitem.o\
	setupwizard.moc.o setupwizard.o kbpm-dj.o edit-distance.o\
	renamer.o renamer.moc.o renamer.logic.o renamer.logic.moc.o\
	similars.o similars.moc.o similarscanner.o similarscanner.moc.o\
	config.o merger-dialog.o merger-dialog.moc.o common.o

MERGER_OBJECTS = merger.o index.o common.o scripts.o song-information.o song-information.moc.o\
	efence.o page.o efence-print.o

#############################################################################
# Binaries
#############################################################################
duploc: $(DUPLOC_OBJECTS)
	$(CPP) $(DUPLOC_OBJECTS) -o duploc\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

filetags: $(FILETAGS_OBJECTS)
	$(CPP) $(FILETAGS_OBJECTS) -o filetags\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

unphaser: $(UNPHASER_OBJECTS)
	$(CPP) $(UNPHASER_OBJECTS) -o unphaser $(LDFLAGS)

kbpm-play: $(KPLAY_OBJECTS)
	$(CPP) $(KPLAY_OBJECTS) -o kbpm-play\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

vector-view: $(VECTOR_OBJECTS)
	$(CPP) $(VECTOR_OBJECTS) -o vector-view\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

kbpm-dj: $(KSEL_OBJECTS) songselector.cpp songselector.h
	$(CPP) $(KSEL_OBJECTS) -o kbpm-dj\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

merger: $(MERGER_OBJECTS)
	$(CPP) $(MERGER_OBJECTS) -o merger\
	  $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)
