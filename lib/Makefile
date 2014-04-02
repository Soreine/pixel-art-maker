SOURCES = ColorHist.cpp \
	Color.cpp \
	TestColorHist.cpp \
	Triplet.cpp

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = TestColorHist

all: $(OBJECTS)
	g++ -g -o $(TARGET) $(OBJECTS)
%o: %cpp
	g++ -g -o  $@ -c $<

clean:
	rm -rf $(OBJECTS)