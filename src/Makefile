CXX = g++
CXXFLAGS = -Wall -std=c++0x -g
LDLIBS = -lpng

.PHONY: all clean

all: rpnr

rpnr: main.o LMImageRepresentation.o LMImageRepresentationBlur.o LMImageRepresentationBlurEdges.o LMImageRepresentationBlurSurfaces.o IO.o
	$(CXX) -o $@ $^ $(LDLIBS)
	cp rpnr ../bin/

clean:
	rm -f *~ *.o rpnr
