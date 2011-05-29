include defines
LINK = $(CPP) $(LDFLAGS) $(QT_LIBRARY_PATH) $(QT_LIBS)

%.o: %.cpp
	@echo "  [cpp] "$@
	@$(CPP) $(CFLAGS) $(INC) -o $@ -c $<

all: active-objects.h aoc demo1

clean: 
	@echo "[clean] Active"
	@rm -f message-syntax.cpp message-syntax.h message-lexer.cpp .depend
	@rm -f array-test message-test borgiv.a
	@rm -f *.o gmon.out fixname aoc soc demo1
	@touch .depend .sources

.depend:
	@echo " [file] dependencies"
	$(CPP) $(CFLAGS) $(INC) -MM *.cpp >.depend

-include .depend

.PHONY: all clean

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
	@echo " [link] "$@
	@g++ $^ -o $@ -lpthread
