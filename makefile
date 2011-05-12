VERSION = 4.0
BIN = bos bpmdj bpmplay bpmmerge bpmcount
.EXPORT_ALL_VARIABLES:
all: .link-targets .ui-forms .rc-files .source-creator .depend .compile 

# We don't use autotools, automake, autoproject and so on since these are
# too complicated. Instead we expect that the one who wants to compile the
# software (the compiler ?) is educated enough to point to the proper
# headers and libraries
ifeq (${shell if test ! -f defines; then echo "N"; else echo "Y"; fi},N)
$(error "Please read 'compile.txt'")
endif
CFLAGS=-DLINUX -DSHORT_ONE
JOBS=1
include defines
LINK =  $(CPP) $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

# Check all the link targets
.link-targets: fix-link-target
	@echo "Link targets:"
	@./fix-link-target

# Generate all the necessary user interface files
.ui-forms: ui-forms
	@echo "User Interface Resources:"
	@make -s --no-print-directory -f ui-forms $@
.rc-files: rc-files
	@echo "Resource files:"
	@make -s --no-print-directory -f rc-files $@

# Precompile all the source files
# The output file is the .source file.
.PHONY: .source-creator documentation
.source-creator: sources
	@echo "Source Files:"
	@make -s --no-print-directory -f sources .sources

# Create the dependency file if necessary
# only necessary if the .sources was modified
.depend: .sources
	@echo "Dependencies:"
	@$(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp >.depend

# Compilation of all source files
.compile: compile .depend
	@echo "Compiling: "$(JOBS)" at a time"
	@make -j $(JOBS) -s --no-print-directory -f compile 

# Packaging of the distribution
packages: packager all
	@echo "Packaging:"
	@make -s --no-print-directory -f packager

documentation:
	@echo "Documenting:"
	@make -s --no-print-directory -f documenter

check-bin: 
	@make -s --no-print-directory -f packager bpmdj-source.tgz
	@echo =========== Entering Compilation Reactor ===================
	@mkdir tmp
	@echo [unzip]
	@cd tmp; tar -xzf ../bpmdj-source.tgz
	@cp -L defines tmp/
	@cd tmp; make -s
	@echo [clean] reactor
	@rm -rf tmp
	@echo ============================================================

check-doc: 
	@make -s --no-print-directory -f packager bpmdj-doc.tgz
	@echo =========== Entering Documentation Reactor =================
	@echo [unzip]
	@mkdir tmp
	@cd tmp; tar -xzf ../bpmdj-doc.tgz
	@echo " [test] widowed files"
	@cd tmp/documentation; for a in *.html; do cat $$a >>all; done
	@cd tmp/documentation; for a in *.png; do echo -n $$a "  :::"; grep -c $$a all; done  >../../count
	@grep :::0 count >bpmdj-doc-errors.txt; exit 0
	@echo " [test] widowed links"
	@linklint -error -root tmp/documentation /@ 2>>bpmdj-doc-errors.txt
	@echo "[clean] reactor"
	@rm -rf tmp
	@test -s bpmdj-doc-errors.txt
	@echo "Report is in bpmdj-doc-errors.txt"
	@echo ============================================================

clean:
	@echo "[clean]"
	@rm -f *.o *.a *.h *.cpp $(BIN) aoc soc sum.tmp 

check: check-bin check-doc

# Varia
nens:
	@~/Administration/bpmdj-to-nens

tuuster: 
	@scp bpmmerge bpmdj@tuuster:.bin/
	@scp bpmplay  bpmdj@tuuster:.bin/
	@scp bpmdj    bpmdj@tuuster:.bin/bpmdj.real

website: documentation
	@echo "  [web] uploading website"
	@rsync -xrlptoDvz --delete-excluded --exclude .svn --exclude "*~" documentation/* werner@yellowcouch.org:/home/bpmdj/public_html/
