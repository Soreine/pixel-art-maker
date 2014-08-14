SOURCES = palette.cpp \
	  lib/ColorHist.cpp \
	  lib/Color.cpp \
	  lib/HSVColor.cpp \
	  lib/Triplet.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = palette



CPPFLAGS = -g -Wall -W -ansi -pedantic -Dcimg_use_vt100 -Dcimg_use_png -I/usr/X11R6/include

LDFLAGS =  -L/usr/X11R6/lib -lpthread -lpng -lz -lX11 -lm 

all: $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%o: %cpp
	g++ $(CPPFLAGS) -c -o $@ $< 

clean:
	rm -rf $(OBJECTS)
