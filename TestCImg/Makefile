SOURCES = test.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = test

OPTIONS = -I.. -Wall -W -ansi -pedantic -Dcimg_use_vt100 -I/usr/X11R6/include  -lm -L/usr/X11R6/lib -lpthread -lX11 

all: $(OBJECTS)
	g++ -g -o $(TARGET) $(OBJECTS) $(OPTIONS)
%o: %cpp
	g++ -g -o  $@ -c $< 

clean:
	rm -rf $(OBJECTS)