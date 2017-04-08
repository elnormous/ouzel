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
CXXFLAGS=-c -std=c++11 -Wall -O2 -I../ouzel -Winvalid-pch -include Prefix.hpp
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
	-framework OpenAL \
	-framework OpenGL \
	-framework QuartzCore
else ifeq ($(platform),emscripten)
	LDFLAGS+=--embed-file Resources -s TOTAL_MEMORY=33554432
endif
SOURCES=AnimationsSample.cpp \
	main.cpp \
	Samples.cpp \
	GameSample.cpp \
	GUISample.cpp \
	InputSample.cpp \
	MainMenu.cpp \
	PerspectiveSample.cpp \
	SoundSample.cpp \
	SpritesSample.cpp \
	RTSample.cpp
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
ifeq ($(platform),emscripten)
EXECUTABLE=samples.js
else
EXECUTABLE=samples
endif

.PHONY: all
all: bundle

.PHONY: debug
debug: target=debug
debug: CXXFLAGS+=-DDEBUG -g
debug: bundle

.PHONY: bundle
bundle: $(EXECUTABLE)
ifeq ($(platform),macos)
bundle:
	mkdir -p samples.app
	mkdir -p samples.app/Contents
	cp -f macos/app/Info.plist samples.app/Contents
	mkdir -p samples.app/Contents/MacOS
	cp -f $(EXECUTABLE) samples.app/Contents/MacOS
	mkdir -p samples.app/Contents/Resources
	cp -f Resources/* samples.app/Contents/Resources/
	cp -f macos/app/AppIcon.icns samples.app/Contents/Resources
endif

$(EXECUTABLE): ouzel $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS): Prefix.hpp.gch

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

%.hpp.gch: %.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: ouzel
ouzel:
	$(MAKE) -f ../build/Makefile platform=$(platform) $(target)

.PHONY: clean
clean:
	$(MAKE) -f ../build/Makefile clean
	rm -f $(EXECUTABLE) *.o *.js.mem *.js *.hpp.gch
	rm -rf samples.app