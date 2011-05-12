VERSION = 3.4
.EXPORT_ALL_VARIABLES:
all: .link-targets .ui-forms .source .depend .compile 

# We don't use autotools, automake, autoproject and so on since these are
# too complicated. Instead we expect that the one who wants to compile the
# software (the compiler ?) is educated enough to point to the proper
# headers and libraries
ifeq (${shell if test ! -f defines; then echo "N"; else echo "Y"; fi},N)
$(error "Please read 'compile.txt'")
endif
CFLAGS=-DLINUX -DSHORT_ONE
include defines
LINK =  $(CPP) $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)

# Check all the link targets
.link-targets: fix-link-target
	@echo "Link targets:"
	@./fix-link-target

# Generate all the necessary user interface files
.ui-forms: ui-forms
	@echo "User Interface Forms:"
	@make -s --no-print-directory -f ui-forms $@

# Precompile all the source files
.PHONY: .source
.source: sources
	@echo "Source Files:"
	@make -s --no-print-directory -f sources .sources

# Create the dependency file if necessary
.depend: .sources
	@echo "Dependencies:"
	@$(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp >.depend

# Compilation of all source files
.compile: compile .depend
	@echo "Compiling:"
	@make -j 3 -s --no-print-directory -f compile

# Packaging of the distribution
packages: packager all
	@echo "Packaging:"
	@make -s --no-print-directory -f packager

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

check: check-bin check-doc

# Varia
nens:
	@~/Administration/bpmdj-to-nens

tuuster: 
	@scp kbpm-play bpmdj@tuuster:bpmdj/
	@scp kbpm-dj   bpmdj@tuuster:bpmdj/

website:
	rsync -e ssh -xavz documentation/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs/
	ssh krubbens@bpmdj.sourceforge.net chmod +rX -R /home/groups/b/bp/bpmdj/htdocs/*
