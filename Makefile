CXX=g++
OPT=-O3
STD=-std=c++14
DEPFLAGS=-MMD -MP
CXXFLAGS=-Wall -Wextra -Iinclude $(OPT)


CPPFILES=$(wildcard src/*.cpp)
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(CPPFILES))
DEPFILES=$(patsubst src/%.cpp,obj/%.d,$(CPPFILES))

BINARY=bin/raycaster
BINFLAGS=-lSDL2 -fopenmp -lpng -lz


all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(BINFLAGS) -o $(BINARY) $(OBJECTS)

obj/%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

obj/loadpng.o:src/loadpng.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -Dcimg_use_png=1 -Dcimg_display=0 -c -o obj/loadpng.o src/loadpng.cpp

obj/main.o:src/main.cpp
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

# create dirs
obj: FORCE
	mkdir -p obj

bin: FORCE
	mkdir -p bin

FORCE:

clean:
	rm -rf $(BINARY) $(OBJECTS)

-include $(DEPFILES)