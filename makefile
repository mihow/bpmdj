VERSION = 4.2-pl2
DEMOS = index-demo idx2txt
BIN =  bpmcount bpmplay bpmdj bpmmerge $(DEMOS)
.EXPORT_ALL_VARIABLES:
all: .link-targets .ui-forms .rc-files .source-creator .depend .compile 

# We don't use autotools, automake, autoproject and so on since these are
# too complicated. Instead we expect that the one who wants to compile the
# software (the compiler ?) is educated enough to point to the proper
# headers and libraries
ifeq (${shell if test ! -f defines; then echo "N"; else echo "Y"; fi},N)
$(error "Please read 'compile.txt' (don't forget the extention .txt)")
endif
CFLAGS=-DLINUX -DSHORT_ONE
JOBS=1
include defines
LINK =  $(CPP) $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)
ACDA = Active Data

# Check all the link targets
.link-targets:
	@if test -e ./fix-link-target; then echo "Link targets:"; ./fix-link-target; fi

# Generate all the necessary user interface files
.ui-forms: ui-forms
	@echo "User Interface Resources:"
	@make -s --no-print-directory -f ui-forms $@

.rc-files: rc-files
	@echo "Resource files:"
	@make -s --no-print-directory -f rc-files $@

# Precompile all the source files
# The output file is the .source file.
.PHONY: .source-creator
.source-creator: sources
	@echo "Source Files:"
	@for a in $(ACDA); do make -s -C $$a -f sources; done
	@make -s -C Active aoc;
	@make -s -C Data doc;
	@make -s --no-print-directory -f sources .sources

# Create the dependency file if necessary
# only necessary if the .sources was modified
.depend: .sources
	@echo "Dependencies:"
	@$(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp >.depend

# Compilation of all source files
.compile: compile .depend
	@echo "Compiling: "$(JOBS)" at a time"
	@for a in $(ACDA); do make -s -C $$a ; done
	@make -j $(JOBS) -s --no-print-directory -f compile 

clean:
	@for a in $(ACDA); do make -s -C $$a clean; done
	@echo "[clean] BpmDj"
	@rm -f *.o *.a $(BIN) sum.tmp

very-clean: clean
	@for a in $(ACDA); do make -s -f sources -C $$a clean; done
	@echo "[clean] BpmDj sources"
	@rm -f *.h *.cpp


# Varia
tuuster: 
	@scp bpmmerge bpmdj@tuuster:.bin/
	@scp bpmplay  bpmdj@tuuster:.bin/
	@scp bpmdj    bpmdj@tuuster:.bin/bpmdj.real
