ifeq ($(OS),Windows_NT)
    platform=windows
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Linux)
        platform=linux
    endif
    ifeq ($(UNAME),Darwin)
        platform=macos
    endif
endif
ifeq ($(platform),emscripten)
CC=emcc
CXX=em++
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 -I../ouzel -Winvalid-pch -include linux/Prefix.h
LDFLAGS=-O2 -L. -louzel
ifeq ($(platform),raspbian)
CXXFLAGS+=-DRASPBIAN
LDFLAGS+=-L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host -lopenal -lpthread
else ifeq ($(platform),linux)
LDFLAGS+=-lX11 -lGL -lopenal -lpthread
else ifeq ($(platform),macos)
LDFLAGS+=-framework AudioToolbox \
	-framework AudioToolbox \
	-framework CoreVideo \
	-framework Cocoa \
	-framework GameController \
	-framework Metal \
	-framework MetalKit \
	-framework OpenAL \
	-framework OpenGL
else ifeq ($(platform),emscripten)
	LDFLAGS+=--embed-file Resources -s TOTAL_MEMORY=33554432
endif
SOURCES=AnimationsSample.cpp \
	main.cpp \
	Samples.cpp \
	GUISample.cpp \
	InputSample.cpp \
	MainMenu.cpp \
	SoundSample.cpp \
	SpritesSample.cpp \
	RTSample.cpp
OBJECTS=$(SOURCES:.cpp=.o)
ifeq ($(platform),emscripten)
EXECUTABLE=samples.js
else
EXECUTABLE=samples
endif

all: $(SOURCES) $(EXECUTABLE)

debug: target=debug
debug: CXXFLAGS+=-DDEBUG -g
debug: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MAKE) -f ../build/Makefile platform=$(platform) $(target)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	$(MAKE) -f ../build/Makefile clean
	rm -f $(EXECUTABLE) *.o *.js.mem *.js
