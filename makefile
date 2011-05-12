# This top level makefile will run through a set of phases to compile the sofware
# The first phase involvse linking all the links into the cvs, thereby checking the 
# possible conflicts that might arise due to cvs updates
VERSION = 3.3
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
	@make -s --no-print-directory -f compile

# Packaging of the distribution
packages: packager all
	@echo "Packaging:"
	@make -s --no-print-directory -f packager

# Varia
nens:
	@~/Administration/bpmdj-to-nens

tuuster: kbpm-dj kbpm-play
	@scp kbpm-dj kbpm-play bpmdj@tuuster:bpmdj/

website:
	rsync -e ssh -xavz documentation/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs/
	ssh krubbens@bpmdj.sourceforge.net chmod +rX -R /home/groups/b/bp/bpmdj/htdocs/*
