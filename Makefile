TARGETS = pixam-palette pixam-dither

# sources

COMMON_SRC = src/ColorHist.cpp \
	     src/Color.cpp \
	     src/HSVColor.cpp \
	     src/Triplet.cpp

COMMON_INCLUDE = $(COMMON_SRC:src/%.cpp=include/%.h)

PALETTE_SRC = src/palette.cpp

DITHER_SRC = src/dither.cpp

# object files

COMMON_OBJ = $(COMMON_SRC:src/%.cpp=obj/%.o)
PALETTE_OBJ = $(PALETTE_SRC:src/%.cpp=obj/%.o)
DITHER_OBJ = $(DITHER_SRC:src/%.cpp=obj/%.o)

ALL_OBJ = $(COMMON_OBJ) $(PALETTE_OBJ) $(DITHER_OBJ)

# dependencies

ALL_DEPS = $(ALL_OBJ:%.o=%.d)

CPPFLAGS = \
	 -g -Wall -W -ansi -pedantic \
	 -Dcimg_use_vt100 -Dcimg_use_png \
	 -Iinclude -I/usr/X11R6/include

LDFLAGS = \
	-lboost_filesystem -lboost_system \
	-L/usr/X11R6/lib \
	-lpthread -lpng -lz -lX11 -lm 

# general targets

all: $(TARGETS)

clean:
	rm -rf $(PALETTE_OBJ) $(DITHER_OBJ) $(COMMON_OBJ) $(TARGETS)

dist_clean: clean
	rm -fr $(TARGET)

# specific targets

pixam-palette: $(PALETTE_OBJ) $(COMMON_OBJ)
	g++ -o $@ $(PALETTE_OBJ) $(COMMON_OBJ) $(LDFLAGS)

pixam-dither: $(DITHER_OBJ) $(COMMON_OBJ)
	g++ -o $@ $(DITHER_OBJ) $(COMMON_OBJ) $(LDFLAGS)

# include dependencies (if they exist)

-include $(ALL_DEPS)

# generic rules (with automatic generation of dependencies and automagic
# creation of obj directory)

obj/%.o: src/%.cpp
	@mkdir obj 2>/dev/null || true
	g++ -MMD $(CPPFLAGS) -c -o $@ $<
