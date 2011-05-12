VERSION        = 0.6
MAJOR_VERSION  = 0
MINOR_VERSION  = 6

AUTOGEN_PROGRAM=$(shell autoopts-config --autogen)
AUTOGEN_DATADIR=$(shell autoopts-config --pkgdatadir)
AUTOGEN_FLAGS=-L$(AUTOGEN_DATADIR) 
AUTOGEN_CFLAGS=$(shell autoopts-config --cflags)
AUTOGEN_LDFLAGS=$(shell autoopts-config --libs)
CFLAGS+=$(AUTOGEN_CFLAGS)
LDFLAGS+=$(AUTOGEN_LDFLAGS)

notice:
	@echo cBpmDj $(VERSION)
	@echo "-----------------"
	@echo "               all : makes cbpm-player cbpm-period"
	@echo "       cbpm-period : makes the divine bpm finder"
	@echo "       cbpm-player : makes the divine player"
	@echo "             clean : cleans this directory"
	@echo "        very-clean : removes backupfiles too"
 
all: cbpm-period cbpm-player

cbpm-player: cbpm-playeropts.o cbpm-player.o 
	gcc $^ $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

cbpm-period: cbpm-period.c
	gcc $< $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

%.c: %.def
	$(AUTOGEN_PROGRAM) $(AUTOGEN_FLAGS) $^

%.o: %.c
	gcc -c $< $(CFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

       
clean:
	rm -f *.log *.tex *.tex.dep *.toc *.dvi *.aux *.raw toplot.dat plot.ps
	rm -f cbpm-player cbpm-period cbpm-playeropts.[ch] *.o
	rm -fr BeatMixing

very-clean: clean 
	rm -f BeatMixing.ps *~ *.idx *.tgz logfile
	rm -rf cBpmDj-$(VERSION)

%.ps: %.lyx
	lyx -e ps $<

%.tex: %.lyx
	lyx -e latex $<

%.html: %.tex
	latex2html -split +0 -toc_depth 8 -info 0 -address "" -no_navigation $<
	
tgz tar.gz: very-clean
	cd ..; tar -cvzf cBpmDj-$(VERSION).tgz cBpmDj
	scp ../cBpmDj*tgz werner@asus:

package:
	$(MAKE) very-clean
	$(MAKE) all BeatMixing.ps
	$(MAKE) clean
	mkdir cBpmDj-$(VERSION)
	mkdir cBpmDj-$(VERSION)/music
	mkdir cBpmDj-$(VERSION)/index
	cp -f * cBpmDj-$(VERSION); exit 0
	tar -cvzf cBpmDj-$(VERSION).tgz cBpmDj-$(VERSION)
	mv *.tgz ..

BeatMixing.html: BeatMixing.tex

allhtml: BeatMixing.html
	rm BeatMixing/index.html
	cp index.html BeatMixing/index.html

website: allhtml
	scp BeatMixing/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs
