DATA_VERSION_MAJOR = 0
DATA_VERSION_MINOR = 2
ECHO = echo 
RM = rm -f
CD = cd
CP = cp
MV = mv
TAR = tar
MKDIR = mkdir
AR = ar

include defines

LINK = $(CPP) $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)
COMPILE = $(CPP)

%.o: %.cpp
	@$(ECHO) "  [c++] "$@
	@$(COMPILE) $(CFLAGS) $(QT_INCLUDE_PATH) -o $@ -c $<

DATA = data.o data-lexer.o data-syntax.o data-io.o \
	data-visitor.o array-creator.o symbol.o token.o string.o \
	null.o set.o
DATATEST = $(DATA) data-test.o
ARRAY = array-test.o data.o symbol.o string.o token.o \
	data-visitor.o null.o
EXTRA = data-lexer.cpp data-syntax.h data-syntax.cpp

all: om-data.a
bin: om-data.a

array-test: $(ARRAY)
	$(LINK) $(ARRAY) -o $@

data-test: $(DATA)
	$(LINK) $(DATA) -o $@

om-data.a : $(DATA)
	@$(ECHO) "   [ar] om-data.a"
	@$(AR) r $@ $(DATA) 2>.ignore

data-lexer.cpp: data-lexer.l data-syntax.h
	@$(ECHO) " [flex] data-lexer.cpp"
	@flex -o data-lexer.cpp data-lexer.l

data-syntax.h: data-syntax.y
	@$(ECHO) "[bison] data-syntax.h data-syntax.cpp"
	@bison --defines=data-syntax.h --output-file=data-syntax.cpp data-syntax.y

data-syntax.cpp: data-syntax.y
	@$(ECHO) "[bison] data-syntax.h data-syntax.cpp"
	@bison --defines=data-syntax.h --output-file=data-syntax.cpp data-syntax.y

DATA_VERSION = $(DATA_VERSION_MAJOR).$(DATA_VERSION_MINOR)

test:
	echo $(DATA_VERSION)

version.h: 
	@echo "#define DATA_VERSION \""$(DATA_VERSION_MAJOR)"."$(DATA_VERSION_MINOR)"\"" >version.h
	@echo "#define DATA_VERSION_MAJOR "$(DATA_VERSION_MAJOR) >>version.h
	@echo "#define DATA_VERSION_MINOR "$(DATA_VERSION_MINOR) >>version.h
	@echo "#define DATA_VERSION_NR (DATA_VERSION_MAJOR*256+DATA_VERSION_MINOR)" >>version.h

depend: version.h $(EXTRA)
	@$(ECHO) " [file] depend"
	@$(TIME) $(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp *.cpp >depend

include depend

clean: 
	rm -f data-syntax.cpp data-syntax.h data-lexer.cpp depend
	rm -f array-test data-test 
	rm -f *.o gmon.out

.PHONY: all clean files

files:
	@echo " [file] list"
	@rm -r tmp 2>/dev/null; exit 0
	@find ./* -iname "*" | grep -v ^\./index | grep -v ^\./music > existing_files # list of existing files
	@grep \  existing_files > problem_files; exit 0 # problematic files
	@test ! -s problem_files
	@gawk '{print $$1}' files >known_files # list of known files
	@grep -vxFf known_files existing_files >unknown_files; exit 0 # unknown files
	@gawk '{print $$1"  ---"}' unknown_files >>files # attach these to the files
	@gawk '{if ($$2=="---") print $1}' files >untagged_files; exit 0 # find files missing tags
	@if test -s untagged_files; then cat untagged_files; exit 1; fi

source_files: files
	@echo " [file] source"
	@gawk '{if (index($$2,"source")) print $$1}' files >source_files 

install_files: files
	@echo " [file] install"
	@gawk '{if (index($$2,"install")) print $$1}' files >install_files

documentation_files: files
	@echo " [file] documentation"
	@gawk '{if (index($$2,"doc")) print $$1}' files >documentation_files 

support_files: files
	@echo " [file] support"
	@gawk '{if (index($$2,"support")) print $$1}' files >support_files

temp_files: files
	@echo " [file] temporary"
	@gawk '{if (index($$2,"temp")) print $$1}' files >temp_files

omdata-source.tgz: source_files
	@echo "  [tgz] source"
	@tar -czh --no-recursion --ignore-failed-read -T source_files -f omdata-source.tgz  2>/dev/null 

omdata-bin.tgz: install_files bin
	@echo "[strip]"
	@strip $(ELFBIN) 2>/dev/null; exit 0
	@echo "  [tgz] install"
	@tar -czh --no-recursion --ignore-failed-read -T install_files -f omdata-bin.tgz 2>/dev/null 

omdata-doc.tgz: documentation_files
	@echo "  [tgz] documentation"
	@tar -czh --no-recursion --ignore-failed-read -T documentation_files -f omdata-doc.tgz 2>/dev/null 

omdata-support.tgz: support_files
	@echo "  [tgz] support"
	@tar -czh --no-recursion --ignore-failed-read -T support_files -f omdata-support.tgz 2>/dev/null 

install: omdata-bin.tgz
	mkdir -p $(DESTDIR)/omdata;
	cp omdata-bin.tgz $(DESTDIR)/omdata
	cd $(DESTDIR)/omdata; tar -xzf omdata-bin.tgz
	rm $(DESTDIR)/omdata/omdata-bin.tgz

source.tgz-dist: omdata-source.tgz
	@echo "  [tgz] "omdata-$(DATA_VERSION).source.tgz
	@$(MKDIR) omdata-$(DATA_VERSION)
	@$(CD) omdata-$(DATA_VERSION); tar -xzf ../omdata-source.tgz
	@$(TAR) -czf omdata-$(DATA_VERSION).source.tgz omdata-$(DATA_VERSION)
	@$(RM) -r omdata-$(DATA_VERSION)


bin.tgz-dist: omdata-bin.tgz
	@echo "  [tgz] "omdata-$(DATA_VERSION).bin.tgz
	@$(MKDIR) omdata-$(DATA_VERSION)
	@$(CD) omdata-$(DATA_VERSION); tar -xzf ../omdata-bin.tgz
	@$(TAR) -czf omdata-$(DATA_VERSION).bin.tgz omdata-$(DATA_VERSION)
	@$(RM) -r omdata-$(DATA_VERSION)


#doc.tgz-dist: omdata-doc.tgz
#	@echo "  [tgz] "omdata-$(DATA_VERSION).doc.tgz
#	@$(MKDIR) omdata-$(DATA_VERSION)
#	@$(CD) omdata-$(DATA_VERSION); tar -xzf ../omdata-doc.tgz
#	@$(TAR) -czf omdata-$(DATA_VERSION).doc.tgz omdata-$(DATA_VERSION)
#	@$(RM) -r omdata-$(DATA_VERSION)

tgz: source.tgz-dist bin.tgz-dist omdata-support.tgz #doc.tgz-dist 
	@echo Finished
	@$(MV) omdata-support.tgz ../omdata-$(DATA_VERSION).support.tgz
	@$(MV) *$(DATA_VERSION)*tgz ..
	@rm *.tgz
