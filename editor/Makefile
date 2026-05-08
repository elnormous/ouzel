# Editor application — Linux / macOS CLI build.
#
# Builds editor.exe (or 'editor' on POSIX) by linking the editor's own
# translation units against the engine's libouzel.a. Mobile / web targets
# are intentionally not handled here: the editor ships only for desktop.
#
# Usage:
#   make -C ../engine        (build the engine library first)
#   make                      (build the editor)
#
# Cross-platform notes:
#   * The same source list compiles on Linux and macOS — no platform-
#     specific files in editor/ itself; everything platform-aware is in
#     the engine layer (window, render device, input).
#   * On macOS the linker pulls Cocoa / Metal / IOKit / etc. via
#     -framework, mirroring the samples Makefile pattern. We avoid
#     duplicating the full framework list by linking transitively
#     through the engine's own list (libouzel.a wraps everything).

ifeq ($(OS),Windows_NT)
PLATFORM=windows
else
os=$(shell uname -s)
ifeq ($(os),Linux)
PLATFORM=linux
else ifeq ($(os),Darwin)
PLATFORM=macos
endif
endif

CXXFLAGS=-std=c++17 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I"../engine" \
	-I"../external/imgui"

OBJCXXFLAGS=-std=c++17 \
	-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow \
	-I"../engine" \
	-I"../external/imgui"

SOURCES=main.cpp \
	EditorApp.cpp \
	EditorPicking.cpp \
	SelectionContext.cpp \
	panels/InfoPanel.cpp \
	panels/InspectorPanel.cpp \
	panels/PlayControlPanel.cpp \
	panels/SceneTreePanel.cpp

ifeq ($(PLATFORM),linux)
LDFLAGS=-L"../engine" -louzel \
	-lGL -lGLU -lopenal -lasound -lX11 -lXcursor -lXi -lXrandr -lXss -lpthread -ldl
else ifeq ($(PLATFORM),macos)
LDFLAGS=-L"../engine" -louzel \
	-framework Cocoa -framework Metal -framework MetalKit -framework QuartzCore \
	-framework OpenGL -framework OpenAL -framework GameController \
	-framework IOKit -framework CoreVideo -framework CoreAudio -framework CoreFoundation
endif

OBJECTS=$(SOURCES:.cpp=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
TARGET=editor

.PHONY: all clean
all: $(TARGET)
ifeq ($(DEBUG),1)
all: CXXFLAGS+=-g -DDEBUG
all: OBJCXXFLAGS+=-g -DDEBUG
else
all: CXXFLAGS+=-O3
all: OBJCXXFLAGS+=-O3
all: LDFLAGS+=-O3
endif

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(DEPENDENCIES) $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

-include $(DEPENDENCIES)
