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
CXXFLAGS=-c -std=c++17 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I../engine
SOURCES=main.cpp
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
EXECUTABLE=test

.PHONY: all
ifeq ($(DEBUG),1)
all: CXXFLAGS+=-DDEBUG -g
else
all: CXXFLAGS+=-O3
all: LDFLAGS+=-O3
endif
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(PLATFORM),windows)
	-del /f /q "$(EXECUTABLE).exe" "*.o" "*.d"
else
	$(RM) $(EXECUTABLE) *.o *.d *.js.mem *.js $(EXECUTABLE).exe assetcatalog_generated_info.plist assetcatalog_dependencies
endif