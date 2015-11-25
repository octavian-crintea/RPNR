CXX = g++
CXXFLAGS = -Wall -std=c++0x -g -fopenmp
LDLIBS = -lpng -fopenmp

.PHONY: all clean

all: rpnr

rpnr: main.o LMImageRepresentation.o LMImageRepresentationBlur.o LMImageRepresentationBlurEdges.o LMImageRepresentationBlurSurfaces.o IO.o
	$(CXX) -o $@ $^ $(LDLIBS)
	cp rpnr ../bin/

clean:
	rm -f *~ *.o rpnr
