COMMON_SRC = src/ColorHist.cpp \
	     src/Color.cpp \
	     src/HSVColor.cpp \
	     src/Triplet.cpp

PALETTE_SRC = src/palette.cpp
DITHER_SRC = src/dither.cpp

COMMON_OBJ = $(COMMON_SRC:.cpp=.o)
PALETTE_OBJ = $(PALETTE_SRC:.cpp=.o)
DITHER_OBJ = $(DITHER_SRC:.cpp=.o)

TARGET = palette dither

CPPFLAGS = \
	 -g -Wall -W -ansi -pedantic \
	 -Dcimg_use_vt100 -Dcimg_use_png \
	 -Iinclude -I/usr/X11R6/include

LDFLAGS =  -L/usr/X11R6/lib -lpthread -lpng -lz -lX11 -lm 

# general targets

all: $(TARGET)

clean:
	rm -rf $(PALETTE_OBJ) $(DITHER_OBJ) $(COMMON_OBJ)

dist_clean: clean
	rm -fr $(TARGET)

# specific targets

palette: $(PALETTE_OBJ) $(COMMON_OBJ)
	g++ -o $@ $(PALETTE_OBJ) $(COMMON_OBJ) $(LDFLAGS)

dither: $(DITHER_OBJ) $(COMMON_OBJ)
	g++ -o $@ $(DITHER_OBJ) $(COMMON_OBJ) $(LDFLAGS)

# dependencies

src/palette.o: \
	include/Color.h \
	include/ColorHist.h \
	include/Triplet.h \
	include/HSVColor.h

src/dither.o: \
	include/Color.h \
	include/ColorHist.h \
	include/Triplet.h \
	include/HSVColor.h

# generic rules

%.o: %.cpp
	g++ $(CPPFLAGS) -c -o $@ $< 
