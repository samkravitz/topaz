CXX = g++
CXXFLAGS = -std=c++2a -I $(INCLUDE)
LEX = flex -+
VPATH = src
INCLUDE = include
SRC = \
	chunk.cc \
	compiler.cc \
	function.cc \
	klass.cc \
	scanner.cc \
	token.cc \
	value.cc \
	vm.cc

OBJ = lex.yy.o $(SRC:.cc=.o)

all: release

debug: CXXFLAGS += -DDEBUG -g
debug: topazd

release: CXX += -O2
release: topaz

topaz: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

topazd: main.cc $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

lex.yy.o: lex.ll
	$(LEX) -o lex.yy.cc $<
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o $@

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f topaz topazd
	rm -f lex.yy.cc
	rm -f *.o
