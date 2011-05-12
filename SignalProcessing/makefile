VERSION = 0.1
ECHO = echo 

include defines

LINK = $(CPP) $(LDFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) $(QT_LIBS)
COMPILE = $(CPP)

%.o: %.cpp
	@$(ECHO) "  [c++] "$@
	@$(COMPILE) -c $< $(CFLAGS) -o $@ $(QT_INCLUDE_PATH)

SIGNAL = normalize.o  

all: om-signal.a

om-signal.a : $(SIGNAL)
	@$(ECHO) "   [ar] om-signal.a"
	@ar r $@ $(SIGNAL) 2>/dev/null

#############################################################################
# Dependencies
#############################################################################
version.h: 
	@echo "#define VERSION \""$(VERSION)"\"" >version.h

depend: version.h $(EXTRA)
	@$(ECHO) " [file] depend"
	@$(TIME) $(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) $(QT_LIBRARY_PATH) -MM *.cpp *.cpp >depend

include depend

clean: 
	rm -f *.o gmon.out


