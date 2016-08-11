ifndef platform
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
endif
CPPFLAGS=-c -std=c++11 -Wall -I../ouzel -Winvalid-pch -include linux/Prefix.h
LDFLAGS=-L. -louzel
ifeq ($(platform),raspbian)
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
EXECUTABLE=samples

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MAKE) -f ../build/Makefile platform=$(platform)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	$(MAKE) -f ../build/Makefile clean
	rm -f $(EXECUTABLE) *.o
