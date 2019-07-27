PNAME = pixam
TARGETS = $(PNAME)

# sources

COMMON_SRC = src/ColorHist.cpp \
	     src/Color.cpp \
	     src/HSVColor.cpp \
	     src/Triplet.cpp

COMMON_INCLUDE = $(COMMON_SRC:src/%.cpp=include/%.h)

CLI_SRC = src/pixam.cpp src/pixam-cli.cpp

# object files

COMMON_OBJ = $(COMMON_SRC:src/%.cpp=obj/%.o)
CLI_OBJ = $(CLI_SRC:src/%.cpp=obj/%.o)

ALL_OBJ = $(COMMON_OBJ) $(CLI_OBJ)

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

STRIP = -s
# general targets

all: $(TARGETS)

clean:
	rm -rf $(ALL_OBJ) $(TARGETS) obj

dist_clean: clean
	rm -fr $(TARGETS)

# specific targets

$(PNAME): $(ALL_OBJ)
	g++ -o $@ $^ $(LDFLAGS) $(STRIP)

# include dependencies (if they exist)

-include $(ALL_DEPS)

# generic rules (with automatic generation of dependencies and automagic
# creation of obj directory)

obj/%.o: src/%.cpp
	@mkdir obj 2>/dev/null || true
	g++ -MMD $(CPPFLAGS) -c -o $@ $<
