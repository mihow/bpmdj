VERSION        = 0.5

notice:
	@echo Divine Intervention $(VERSION)
	@echo "-----------------"
	@echo "               all : makes divine-player divine-period"
	@echo "     divine-period : makes the divine cpm finder"
	@echo "     divine-player : makes the divine player"
	@echo "             clean : cleans this directory"	
	@echo "        very-clean : removes backupfiles too"
 
all: divine-period divine-player

clean:
	rm -f *.log *.tex *.tex.dep *.toc *.dvi *.aux *.raw toplot.dat plot.ps
	rm -f divine-player

very-clean: clean 
	rm -f BeatMixing.ps *~ *.idx *.tgz
	rm -rf DivineIntervention-$(VERSION)

%.ps: %.lyx
	lyx -e ps $<

tgz tar.gz: very-clean
	cd ..; tar -cvzf DivineIntervention-$(VERSION).tgz DivineIntervention
	scp ../DivineIntervention*tgz werner@asus:

package:
	$(MAKE) very-clean
	$(MAKE) all BeatMixing.ps
	$(MAKE) clean
	mkdir DivineIntervention-$(VERSION)
	mkdir DivineIntervention-$(VERSION)/music
	mkdir DivineIntervention-$(VERSION)/index
	cp -f * DivineIntervention-$(VERSION); exit 0
	tar -cvzf DivineIntervention-$(VERSION).tgz DivineIntervention-$(VERSION)
	mv *.tgz ..
