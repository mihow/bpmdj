AO_VERSION_MAJOR = 4
AO_VERSION_MINOR = 3
ECHO = echo 
RM = rm -f
CD = cd
MV = mv
TAR = tar
MKDIR = mkdir
AR = ar

include defines
LINK = $(CPP) $(LDFLAGS) $(QT_LIBRARY_PATH) $(QT_LIBS)

%.h: %.h++ header.txt
	@$(ECHO) "  [h++] "$@
	@"++2--" $< $@

%.cpp: %.c++ header.txt
	@$(ECHO) "  [c++] "$@
	@"++2--" $< $@

%.o: %.cpp
	@$(ECHO) "  [cpp] "$@
	@$(CPP) $(CFLAGS) $(INC) -o $@ -c $<

HEADERS = ${shell ls *.h++}
SOURCES = ${shell ls *.c++}
all: active-objects.h aoc demo1

.sources: $(HEADERS:%.h++=%.h) $(SOURCES:%.c++=%.cpp) 
	@grep "#include\ *\"" *.h *.cpp >/tmp/t
	@if ! [ -e .sources ]; then cp /tmp/t .sources; fi
	@if ! cmp /tmp/t .sources >/dev/null; then mv /tmp/t .sources; fi

.depend: .sources 
	@$(ECHO) " [file] dependencies"
	$(CPP) $(CFLAGS) $(INC) -MM *.cpp >.depend

-include .depend

clean: 
	@$(ECHO) "[clean] Active"
	@rm -f message-syntax.cpp message-syntax.h message-lexer.cpp .depend
	@rm -f array-test message-test borgiv.a
	@rm -f *.o *.h *.cpp gmon.out fixname aoc soc demo1
	@touch .depend .sources

.PHONY: all clean files sources

sources: .sources

ao-lexer.h: ao-lexer.l 
	@echo " [flex] ao-lexer.cpp ao-lexer.h"
	@$(FLEX) --prefix=ao --header-file=ao-lexer.h -o ao-lexer.cpp ao-lexer.l

ao-lexer.cpp: ao-lexer.l ao-syntax.h ao-lexer.h
	@echo " [flex] ao-lexer.cpp"
	@$(FLEX) --prefix=ao -o ao-lexer.cpp ao-lexer.l


ao-syntax.h: ao-syntax.y 
	@echo "[bison] ao-syntax.h ao-syntax.cpp"
	$(BISON) --defines=ao-syntax.h --name-prefix=ao --output-file=ao-syntax.cpp ao-syntax.y

ao-syntax.cpp: ao-syntax.y ao-lexer.h
	@echo "[bison] ao-syntax.h ao-syntax.cpp"
	@$(BISON) --defines=ao-syntax.h --name-prefix=ao --output-file=ao-syntax.cpp ao-syntax.y

aoc: ao-syntax.o ao-lexer.o aoc.o sao-printer.o
	@echo "  [cpp] "$@
	@g++ $^ -o $@

ao-pool.cpp: ao-tracker.h

%.h: %.ao aoc
	@echo "  [aoc] "$@
	@aoc $< $@

# Demo
demo1.cpp: demo1.h

demo1: demo1.o lock.o ao-pool.o ao-tracker.o ao-scheduler.o
	@$(ECHO) " [link] "$@
	@g++ $^ -o $@ -lpthread
