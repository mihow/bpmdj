VERSION        = 0.8
MAJOR_VERSION  = 0
MINOR_VERSION  = 8

AUTOGEN_PROGRAM=$(shell autoopts-config --autogen)
AUTOGEN_DATADIR=$(shell autoopts-config --pkgdatadir)
AUTOGEN_FLAGS=-L$(AUTOGEN_DATADIR) 
AUTOGEN_CFLAGS=$(shell autoopts-config --cflags)
AUTOGEN_LDFLAGS=$(shell autoopts-config --libs)
CFLAGS+=$(AUTOGEN_CFLAGS)
LDFLAGS+=$(AUTOGEN_LDFLAGS)

GOALS=cbpm-count cbpm-play glue-bpmraw glue-mp3raw readme.txt copyright.txt\
	changelog.txt beatmixing.ps
notice:
	@echo cBpmDj $(VERSION)
	@echo "-----------------"
	@echo "               all : makes cbpm-player cbpm-period"
	@echo "       cbpm-period : makes the divine bpm finder"
	@echo "       cbpm-player : makes the divine player"
	@echo "             clean : cleans this directory"
	@echo "        very-clean : removes backupfiles too"
	@echo "       tgz-package : creates shared, static and source tgz's"
 
all: shared-all

shared-all: cbpm-count.shared cbpm-play.shared
	mv cbpm-count.shared cbpm-count
	mv cbpm-play.shared cbpm-play

static-all: cbpm-count.static cbpm-play.static
	mv cbpm-count.static cbpm-count
	mv cbpm-play.static cbpm-play

cbpm-count.static: cbpm-period.static
	mv cbpm-period.static cbpm-count.static
	strip cbpm-count.static

cbpm-play.static: cbpm-player.static
	mv cbpm-player.static cbpm-play.static
	strip cbpm-play.static

cbpm-count.shared: cbpm-period.shared
	mv cbpm-period.shared cbpm-count.shared
	strip cbpm-count.shared

cbpm-play.shared: cbpm-player.shared
	mv cbpm-player.shared cbpm-play.shared
	strip cbpm-play.shared

cbpm-color: cbpm-coloropts.o cbpm-index.o cbpm-color.o 
	gcc $^ -lm $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

cbpm-player.shared: cbpm-playeropts.o cbpm-index.o cbpm-player.o
	gcc $^ -lm $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

cbpm-period.shared: cbpm-periodopts.o cbpm-index.o cbpm-period.c 
	gcc $^ $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

cbpm-player.static: cbpm-playeropts.o cbpm-index.o cbpm-player.o
	gcc $^ -lm -static $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

cbpm-period.static: cbpm-periodopts.o cbpm-index.o cbpm-period.c 
	gcc $^ -static $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

%.c: %.def
	$(AUTOGEN_PROGRAM) $(AUTOGEN_FLAGS) $^

%.o: %.c
	gcc -c $< $(CFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

clean:
	rm -f *.log *.tex *.tex.dep *.toc *.dvi *.aux *.raw toplot.dat
	rm -f plot.ps gmon.out cbpm-play *.static *.shared
	rm -f cbpm-player cbpm-period cbpm-count cbpm-playeropts.[ch] cbpm-periodopts.[ch] *.o
	rm -fr beatmixing

very-clean: clean 
	rm -f beatmixing.ps *~ *.idx *.tgz logfile
	rm -rf cBpmDj-$(VERSION) BpmDj-$(VERSION)

%.ps: %.lyx
	lyx -e ps $<

%.tex: %.lyx
	lyx -e latex $<

%.html: %.tex
	latex2html -split +0 -toc_depth 8 -info 0 -address "" -no_navigation $<
	
wvb: very-clean
	cd ..; tar -cvzf cBpmDj-$(VERSION).tgz cBpmDj
	scp ../cBpmDj*tgz werner@asus:

tgz-package:
	$(MAKE) very-clean
	mkdir cBpmDj-$(VERSION)
	mkdir cBpmDj-$(VERSION)/music
	mkdir cBpmDj-$(VERSION)/index
	cp -f * cBpmDj-$(VERSION); exit 0
	tar -cvzf cBpmDj-$(VERSION).source.tgz cBpmDj-$(VERSION)
	mv *.tgz ..
	$(MAKE) very-clean
	mkdir cBpmDj-$(VERSION)
	mkdir cBpmDj-$(VERSION)/music
	mkdir cBpmDj-$(VERSION)/index
	$(MAKE) all beatmixing.ps
	cp -f $(GOALS) BpmDj-$(VERSION); exit 0
	tar -cvzf cBpmDj-$(VERSION).shared.bin.tgz cBpmDj-$(VERSION)
	mv *.tgz ..
	$(MAKE) very-clean
	mkdir cBpmDj-$(VERSION)
	mkdir cBpmDj-$(VERSION)/music
	mkdir cBpmDj-$(VERSION)/index
	$(MAKE) static-all beatmixing.ps
	cp -f $(GOALS) BpmDj-$(VERSION); exit 0
	tar -cvzf cBpmDj-$(VERSION).static.bin.tgz cBpmDj-$(VERSION)
	mv *.tgz ..
	$(MAKE) very-clean

beatmixing.html: beatmixing.tex

allhtml: beatmixing.html
	rm beatmixing/index.html
	cp index.html beatmixing/index.html

website: allhtml
	scp beatmixing/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs
