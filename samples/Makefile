CFLAGS=-c -std=c++11 -Wall -I../ouzel -Winvalid-pch -include linux/Prefix.h
LDFLAGS=-L. -louzel
ifeq ($(platform),raspbian)
LDFLAGS+=-L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host
else
LDFLAGS+=-lX11 -lGL
endif
SOURCES=AnimationsSample.cpp \
	Application.cpp \
	main.cpp \
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
	$(CXX) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	$(MAKE) -f ../build/Makefile clean
	rm -f $(EXECUTABLE) *.o
