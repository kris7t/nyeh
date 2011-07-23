CXX := g++
LD := g++

PACKAGES := opencv glew libglfw glu
CXXFLAGS := -Wall -std=gnu++0x $(shell pkg-config --cflags $(PACKAGES)) -Isrc -c
LDFLAGS := $(shell pkg-config --libs $(PACKAGES)) -lboost_thread

# un-uncomment for release
CXXFLAGS += -ggdb -D_NYEH_DEBUG
LDFLAGS += -ggdb

define compile
$(CXX) $(CXXFLAGS) $< -o $@
endef

define link
$(LD) $(LDFLAGS) $^ -o $@
endef

obj/%.o: src/%.cxx src/pre.hxx.gch
	$(compile) -include pre.hxx

bin/nyeh.x: obj/main.o obj/Cam.o obj/HighguiCam.o obj/UvcCam.o obj/HistogramHand.o obj/3DView.o obj/GameUpdater.o obj/Net.o obj/NetCam.o obj/NetGame.o obj/ScoreRenderer.o
	$(link)

src/pre.hxx.gch: src/pre.hxx
	$(compile)

.PHONY: all clean

all: bin/nyeh.x

clean:
	rm -f bin/*.x obj/*.o src/*.hxx.gch
