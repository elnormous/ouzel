DEBUG=0
ifeq ($(OS),Windows_NT)
	PLATFORM=windows
else
architecture=$(shell uname -m)
os=$(shell uname -s)
ifeq ($(os),Linux)
PLATFORM=linux
else ifeq ($(os),Darwin)
PLATFORM=macos
endif
endif
CXXFLAGS=-std=c++17 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I../engine
SOURCES=ouzel/main.cpp
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
EXECUTABLE=ouzel/ouzel

.PHONY: all
all: $(EXECUTABLE)
ifeq ($(DEBUG),1)
all: CXXFLAGS+=-DDEBUG -g
else
all: CXXFLAGS+=-O3
all: LDFLAGS+=-O3
endif

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(PLATFORM),windows)
	-del /f /q "$(EXECUTABLE).exe" "*.o" "*.d"
else
	$(RM) $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES) $(EXECUTABLE).exe assetcatalog_generated_info.plist assetcatalog_dependencies
endif