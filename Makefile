SOURCES = pixelArtMaker.cpp \
	  lib/ColorHist.cpp \
	  lib/Color.cpp \
	  lib/Triplet.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = pixelArtMaker

OPTIONS = -I.. -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -lX11 

all: $(OBJECTS)
	g++  -g -o $(TARGET) $(OBJECTS) $(OPTIONS)
%o: %cpp
	g++ -std=c++11 -g -o  $@ -c $< 

clean:
	rm -rf $(OBJECTS)