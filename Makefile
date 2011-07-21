CXX := g++
LD := g++

PACKAGES := opencv glew libglfw
CXXFLAGS := -Wall -std=gnu++0x $(shell pkg-config --cflags $(PACKAGES)) -Isrc -c
LDFLAGS := $(shell pkg-config --libs $(PACKAGES))

# un-uncomment for release
CXXFLAGS += -ggdb -D_DEBUG
LDFLAGS += -ggdb

define compile
$(CXX) $(CXXFLAGS) $< -o $@
endef

define link
$(LD) $(LDFLAGS) $^ -o $@
endef

obj/%.o: src/%.cxx src/pre.hxx.gch
	$(compile) -include pre.hxx

bin/nyeh.x: obj/main.o
	$(link)

src/pre.hxx.gch: src/pre.hxx
	$(compile)

.PHONY: all clean

all: bin/nyeh.x

clean:
	rm -f bin/*.x obj/*.o src/*.hxx.gch
