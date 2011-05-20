include defines
LINK = $(CPP) $(LDFLAGS) $(QT_LIBRARY_PATH) $(QT_LIBS)

%.o: %.cpp
	@echo "  [cpp] "$@
	@$(CPP) $(CFLAGS) $(QT_INCLUDE_PATH) -o $@ -c $<

DATA = data-io.o data.o data-lexer.o data-syntax.o \
	data-visitor.o array-creator.o symbol.o data-token.o \
	data-string.o null.o set.o
DATATEST = $(DATA) data-test.o
ARRAY = array-test.o data.o symbol.o string.o token.o \
	data-visitor.o null.o

all: data.a doc
.PHONY: all clean

clean: 
	@echo "[clean] Data"
	@rm -f *.o *.a doc

.depend: do-lexer.h data-lexer.cpp
	@echo " [file] dependencies"
	@$(TIME) $(CPP) $(CFLAGS) $(INC) -MM *.cpp >.depend
-include .depend

array-test: $(ARRAY)
	$(LINK) $(ARRAY) -o $@

data-test: $(DATA)
	$(LINK) $(DATA) -o $@

data.a : $(DATA)
	@echo "   [ar] data.a"
	@ar r $@ $(DATA) 2>.ignore

data-lexer.cpp: data-lexer.l data-syntax.h
	@echo " [flex] data-lexer.cpp"
	@flex -o data-lexer.cpp data-lexer.l

data-syntax.h: data-syntax.y
	@echo "[bison] data-syntax.h data-syntax.cpp"
	@bison --defines=data-syntax.h --output-file=data-syntax.cpp data-syntax.y

data-syntax.cpp: data-syntax.y
	@echo "[bison] data-syntax.h data-syntax.cpp"
	@bison --defines=data-syntax.h --output-file=data-syntax.cpp data-syntax.y


do-lexer.h: do-lexer.l 
	@echo " [flex] do-lexer.cpp do-lexer.h"
	@flex --prefix=do --header-file=do-lexer.h -o do-lexer.cpp do-lexer.l

do-lexer.cpp: do-lexer.l do-syntax.h do-lexer.h
	@echo " [flex] do-lexer.cpp"
	@flex --prefix=do -o do-lexer.cpp do-lexer.l

do-syntax.h: do-syntax.y 
	@echo "[bison] do-syntax.h do-syntax.cpp"
	@bison --defines=do-syntax.h --name-prefix=do --output-file=do-syntax.cpp do-syntax.y

do-syntax.cpp: do-syntax.y do-lexer.h
	@echo "[bison] do-syntax.h do-syntax.cpp"
	@bison --defines=do-syntax.h --name-prefix=do --output-file=do-syntax.cpp do-syntax.y

doc: do-syntax.o do-lexer.o doc.o do-printer.o doc.o
	@echo " [link] "$@
	@$(CPP) $^ -o $@
