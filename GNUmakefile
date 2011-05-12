# Note:  Makefile is built automatically from Makefile.in
#
# $Id: GNUmakefile.in,v 1.4 2002/02/03 19:38:25 krubbens Exp $
#
# Written by Thomas Steudten <thomas@steudten.com>
# January 25, 2002
#
# This program is free software; you can redistribute it 
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# The GNU General Public License should be included with
# this file.  If not, you can view it at
# http://www.gnu.org/copyleft/gpl.html
# or write to the Free Software Foundation, Inc., 59
# Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# $Log: GNUmakefile.in,v $
# Revision 1.4  2002/02/03 19:38:25  krubbens
# - cbpm-period moved to cbpm-count
# - cbpm-player moved to cbpm-play
# - GNUmakefile.in containted some wrong dependencies
# - GNUmakefile.in was msising the tgz-packaging stuff
#
# Revision 1.3  2002/02/03 18:03:23  krubbens
# - added .tgz make rules
#
# Revision 1.2  2002/02/03 17:40:34  krubbens
# - removed the color build
#
# Revision 1.1  2002/02/03 16:59:37  krubbens
# - added Thomas configure things
#
#

#############################################################################
# Build tools
#############################################################################

CC         = gcc
ECHO       = echo
GREP       = grep
GZIP_PROG  = gzip
INSTALL    = install
LD         = gcc
RM         = rm -f
STRIP_PROG = strip
SED	     = sed
MV         = mv
CP         = cp
MKDIR      = mkdir
CAT        = cat
MAKE       = make
TAR        = tar 
SCP        = scp 
LYX        = lyx
RPM        = rpm

#############################################################################
# Versions 
#############################################################################
VERSION_FILE   = version.h
VERSION        = $(shell $(GREP) -i version= $(VERSION_FILE) | $(SED) 's/^.*=//;s/ *//g')
MAJOR_VERSION  = $(shell $(ECHO) $(VERSION) | $(SED) 's/\..*$$//')
MINOR_VERSION  = $(shell $(ECHO) $(VERSION) | $(SED) 's/^.*\.//')

#############################################################################
# Project name 
#############################################################################
PROJECT   =  cBpmDj

#############################################################################
# Values for AUTOGEN 
#############################################################################
AUTOGEN_PROGRAM=$(shell autoopts-config --autogen)
AUTOGEN_DATADIR=$(shell autoopts-config --pkgdatadir)
AUTOGEN_FLAGS=-L$(AUTOGEN_DATADIR) 
AUTOGEN_CFLAGS=$(shell autoopts-config --cflags)
AUTOGEN_LDFLAGS=$(shell autoopts-config --libs)

#############################################################################
# directories 
#############################################################################

prefix       = /usr/local
exec_prefix  = ${prefix}
BINDIR       = ${exec_prefix}/bin
SRCDIR       = .
DOCDIR       = ${prefix}/doc

#############################################################################
# Setup for make redhat-dist 
#############################################################################

TAR_ARCH = ./$(PROJECT)-$(VERSION).tar.gz 
RPM_BASE = /usr/src/redhat

#############################################################################
# defines 
#############################################################################

PRE  = cbpm

SRC1 = $(AUTOGEN_FILES1:.def=.c) $(PRE)-play.c $(PRE)-index.c 
SRC2 = $(AUTOGEN_FILES2:.def=.c) $(PRE)-count.c $(PRE)-index.c 
SRC3 = $(AUTOGEN_FILES3:.def=.c) $(PRE)-color.c $(PRE)-index.c 
SRCS = $(SRC1) $(SRC2) $(SRC3)

TARGET1 = $(PRE)-play
TARGET2 = $(PRE)-count
STATIC-TARGET1 = $(PRE)-play.static
STATIC-TARGET2 = $(PRE)-count.static
TARGET3 = $(PRE)-color
TARGETS = $(TARGET1) $(TARGET2)
STATIC-TARGETS = $(STATIC-TARGET1) $(STATIC-TARGET2)
# WVB -- i removed this third target because it is still in serieus 
# development (read: it doesn't work at all. prob doesn't even compile)
# $(TARGET3) 

INST_TARGETS = $(PRE)-plot glue-bpmraw glue-mp3raw

AUTOGEN_FILES1 = $(PRE)-playeropts.def
AUTOGEN_FILES2 = $(PRE)-periodopts.def
AUTOGEN_FILES3 = $(PRE)-coloropts.def  

AUTOGEN_FILES = $(AUTOGEN_FILES1) $(AUTOGEN_FILES2) $(AUTOGEN_FILES3)

INC  = $(PRE)-index.h common.h $(VERSION_FILE) config.h 

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJ3 = $(SRC3:.c=.o)

DOCS = README TODO INSTALL AUTHORS COPYRIGHT ChangeLog index.html

LIBS   =  -lm  

CFLAGS  +=  $(AUTOGEN_CFLAGS)
LDFLAGS += $(AUTOGEN_LDFLAGS) 

all: $(TARGETS) GNUmakefile
static-all: $(STATIC-TARGETS) GNUmakefile
	$(MV) $(STATIC-TARGET1) $(TARGET1)
	$(MV) $(STATIC-TARGET2) $(TARGET2)

beatmixing.html: beatmixing.tex

allhtml: beatmixing.html
	$(RM) beatmixing/index.html
	$(CP) index.html beatmixing/index.html

website: allhtml
	$(SCP beatmixing/* krubbens@bpmdj.sourceforge.net:/home/groups/b/bp/bpmdj/htdocs

#############################################################################
# Phony targets
#############################################################################
.PHONY: wvb website  redhat-dist allhtml all clean clean-bin clobber tags install uninstall 

#############################################################################
# Rulesets
#############################################################################

%.c: %.def
	$(AUTOGEN_PROGRAM) $(AUTOGEN_FLAGS) $^

%.o: %.c
	$(CC) -c $< $(CFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) -o $@

%.ps: %.lyx
	$(LYX) -e ps $<

%.tex: %.lyx
	$(LYX) -e latex $<

%.html: %.tex
	latex2html -split +0 -toc_depth 8 -info 0 -address "" -no_navigation $<


#############################################################################
# TARGETS 
#############################################################################

$(TARGET1): $(OBJ1) 
	$(CC)  $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $(LIBS) $^ -o $@
$(TARGET2): $(OBJ2) 
	$(CC)  $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $(LIBS) $^ -o $@
$(STATIC-TARGET1): $(OBJ1) 
	$(CC)  -static $(CFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $^ -o $@ $(LDFLAGS) $(LIBS)
$(STATIC-TARGET2): $(OBJ2) 
	$(CC)  -static $(CFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $^ -o $@ $(LDFLAGS) $(LIBS)
$(TARGET3): $(OBJ3) 
	$(CC)  $(CFLAGS) $(LDFLAGS) -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION) $(LIBS) $^ -o $@

clean-bin:
	$(RM) $(TARGETS) 

clean:
	$(RM) a.out core *.o *.log *.tex *.tex.dep *.toc *.dvi *.aux *.raw toplot.dat
	$(RM) plot.ps gmon.out  
	$(RM) $(AUTOGEN_FILES:.def=.c) $(AUTOGEN_FILES:.def=.h) 
	$(RM) beatmixing

clobber: clean clean-bin
	$(RM) `find . -name tags` config.cache config.log
	$(RM) GNUmakefile configure config.h config.status

tags: $(SRCS) $(HDRS)
	etags $(SRCS) $(HDRS)

wvb: clobber 
	cd ..; $(TAR) cvzf $(PROJECT)-$(VERSION).tgz $(PROJECT) 
	$(SCP) ../$(PROJECT)-$(VERSION).tgz  werner@asus:

GNUmakefile: GNUmakefile.in
	./configure

install: install_bin install_doc

install_bin: 
	for  bin in $(TARGETS) $(INST_TARGETS); do \
	   $(INSTALL) -D -m 755 $$bin ${ROOT}$(BINDIR)/$$bin; \
	done

install_doc: 
	for doc in $(DOCS); do \
	   $(INSTALL) -D -m 755 $$doc ${ROOT}$(DOCDIR)/$$doc; \
	done

uninstall: 
	for bin in $(TARGETS) $(INST_TARGETS); do \
	   $(RM) ${ROOT}$(BINDIR)/$$bin; \
	done \
	for doc in $(DOCS); do \
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
	$(MKDIR) $(PROJECT)-$(VERSION); \
	[ -f $(TAR_ARCH) ] && $(RM) $(TAR_ARCH); \
	$(CP) `find . -maxdepth 1 -type f -print` $(PROJECT)-$(VERSION) ; \
	$(CAT) $(PROJECT).spec | \
		$(SED) "s/_VERSION_/$(VERSION)/g" > $(PROJECT)-$(VERSION)/$(PROJECT).spec; \
	$(TAR) --exclude "CVS" -cvzf $(TAR_ARCH) $(PROJECT)-$(VERSION) ; \
	$(RM) -r $(PROJECT)-$(VERSION); \
	$(RPM) --clean -ta  $(TAR_ARCH); \
	[ -f $(TAR_ARCH) ] && $(RM) $(TAR_ARCH)

#############################################################################
# Source file dependencies
#############################################################################

$(PRE)-player.o: $(PRE)-playeropts.c $(PRE)-index.c $(PRE)-player.c $(INC) $(AUTOGEN_FILES1:.def=.h)
$(PRE)-period.o: $(PRE)-periodopts.c $(PRE)-index.c $(PRE)-period.c $(INC) $(AUTOGEN_FILES2:.def=.h)
$(PRE)-color.o:  $(PRE)-coloropts.c $(PRE)-index.c $(PRE)-color.c $(INC) $(AUTOGEN_FILES3:.def=.h) 

$(AUTOGEN_FILES1:.def=.c) $(AUTOGEN_FILES1:.def=.h): $(AUTOGEN_FILES1)
$(AUTOGEN_FILES2:.def=.c) $(AUTOGEN_FILES2:.def=.h): $(AUTOGEN_FILES2)
$(AUTOGEN_FILES3:.def=.c) $(AUTOGEN_FILES3:.def=.h): $(AUTOGEN_FILES3)
#############################################################################

# WVB -- some necesarry (temporarily) dirt, for building the
#        static and shared versions
.PHONY: directories
GOALS=cbpm-count cbpm-play glue-bpmraw glue-mp3raw README COPYRIGHT AUTHORS\
	ChangeLog INSTALL TODO cbpm-plot beatmixing.ps
	
directories: clean
	$(RM) -r $(PROJECT)-$(VERSION); exit 0
	$(MKDIR) $(PROJECT)-$(VERSION)
	$(MKDIR) $(PROJECT)-$(VERSION)/music
	$(MKDIR) $(PROJECT)-$(VERSION)/index

source.tgz-dist: directories
	$(CP) -f * $(PROJECT)-$(VERSION); exit 0
	cd $(PROJECT)-$(VERSION); autoheader; autoconf;
	$(TAR) -cvzf $(PROJECT)-$(VERSION).source.tgz $(PROJECT)-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r $(PROJECT)-$(VERSION); exit 0

shared.bin.tgz-dist: directories
	$(MAKE) all beatmixing.ps
	$(CP) -f $(GOALS) $(PROJECT)-$(VERSION); exit 0
	$(TAR) -cvzf $(PROJECT)-$(VERSION).shared.bin.tgz $(PROJECT)-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r $(PROJECT)-$(VERSION); exit 0

static.bin.tgz-dist: directories
	$(MAKE) static-all beatmixing.ps
	$(CP) -f $(GOALS) $(PROJECT)-$(VERSION); exit 0
	$(TAR) -cvzf $(PROJECT)-$(VERSION).static.bin.tgz $(PROJECT)-$(VERSION)
	$(MV) *.tgz ..
	$(RM) -r $(PROJECT)-$(VERSION); exit 0

tgz-dist: 
	$(MAKE) source.tgz-dist 
	$(MAKE) shared.bin.tgz-dist 
	$(MAKE) static.bin.tgz-dist
