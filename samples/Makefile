debug=0
ifeq ($(OS),Windows_NT)
	platform=windows
else
	UNAME:=$(shell uname -s)
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
ifeq ($(platform),windows)
LDFLAGS+=-u WinMain -ld3d11 -lopengl32 -ldxguid -lxinput9_1_0 -lshlwapi -ldinput8 -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -ldsound -luuid -lws2_32.lib
else ifeq ($(platform),raspbian)
CXXFLAGS+=-DRASPBIAN
LDFLAGS+=-L/opt/vc/lib -lGLESv2 -lEGL -lbcm_host -lopenal -lpthread -lasound
else ifeq ($(platform),linux)
LDFLAGS+=-lGL -lopenal -lpthread -lasound -lX11 -lXcursor -lXss -lXi -lXxf86vm
else ifeq ($(platform),macos)
LDFLAGS+=-framework AudioToolbox \
	-framework AudioUnit \
	-framework Cocoa \
	-framework CoreAudio \
	-framework CoreVideo \
	-framework GameController \
	-framework IOKit \
	-framework Metal \
	-framework OpenAL \
	-framework OpenGL \
	-framework QuartzCore
else ifeq ($(platform),ios)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0 \
	-framework AudioToolbox \
	-framework AVFoundation \
	-framework Foundation \
	-framework GameController \
	-framework Metal \
	-framework OpenAL \
	-framework OpenGLES \
	-framework QuartzCore \
	-framework UIKit
else ifeq ($(platform),tvos)
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
LDFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0 \
	-framework AudioToolbox \
	-framework AVFoundation \
	-framework Foundation \
	-framework GameController \
	-framework Metal \
	-framework OpenAL \
	-framework OpenGLES \
	-framework QuartzCore \
	-framework UIKit
else ifeq ($(platform),emscripten)
LDFLAGS+=--embed-file settings.ini --embed-file Resources -s TOTAL_MEMORY=134217728
endif
SOURCES=AnimationsSample.cpp \
	GameSample.cpp \
	GUISample.cpp \
	InputSample.cpp \
	main.cpp \
	MainMenu.cpp \
	PerspectiveSample.cpp \
	SoundSample.cpp \
	SpritesSample.cpp \
	RTSample.cpp
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
DEPENDENCIES=$(OBJECTS:.o=.d)
ifeq ($(platform),emscripten)
EXECUTABLE=samples.js
else
EXECUTABLE=samples
endif

.PHONY: all
ifeq ($(debug),1)
all: CXXFLAGS+=-DDEBUG -g
endif
all: bundle

.PHONY: bundle
bundle: $(EXECUTABLE)
ifeq ($(platform),macos)
bundle:
	mkdir -p $(EXECUTABLE).app
	mkdir -p $(EXECUTABLE).app/Contents
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		-e s/'$$(MACOSX_DEPLOYMENT_TARGET)'/10.8/ \
		macos/Info.plist > $(EXECUTABLE).app/Contents/Info.plist
	mkdir -p $(EXECUTABLE).app/Contents/MacOS
	cp -f $(EXECUTABLE) $(EXECUTABLE).app/Contents/MacOS
	mkdir -p $(EXECUTABLE).app/Contents/Resources
	cp -f settings.ini $(EXECUTABLE).app/Contents/Resources
	cp -f Resources/* $(EXECUTABLE).app/Contents/Resources/
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device mac --minimum-deployment-target 10.8 \
		--platform macosx --compile $(EXECUTABLE).app/Contents/Resources macos/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(EXECUTABLE).app/Contents/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(EXECUTABLE).app/Contents/Info.plist
else ifeq ($(platform),ios)
	mkdir -p $(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		ios/Info.plist > $(EXECUTABLE).app/Info.plist
	cp -f $(EXECUTABLE) $(EXECUTABLE).app
	cp -f settings.ini $(EXECUTABLE).app
	cp -f Resources/* $(EXECUTABLE).app
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device iphone --target-device ipad --minimum-deployment-target 8.0 \
		--platform iphoneos --compile $(EXECUTABLE).app ios/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(EXECUTABLE).app/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(EXECUTABLE).app/Info.plist
else ifeq ($(platform),tvos)
	mkdir -p $(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		tvos/Info.plist > $(EXECUTABLE).app/Info.plist
	cp -f $(EXECUTABLE) $(EXECUTABLE).app
	cp -f settings.ini $(EXECUTABLE).app
	cp -f Resources/* $(EXECUTABLE).app
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device tv --minimum-deployment-target 9.0 \
		--platform appletvos --compile $(EXECUTABLE).app tvos/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(EXECUTABLE).app/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(EXECUTABLE).app/Info.plist
endif

$(EXECUTABLE): ouzel $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS): Prefix.hpp.gch

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

%.hpp.gch: %.hpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: ouzel
ouzel:
	$(MAKE) -f ../build/Makefile debug=$(debug) platform=$(platform) $(target)

.PHONY: clean
clean:
	$(MAKE) -f ../build/Makefile clean
ifeq ($(platform),windows)
	-del /f /q $(EXECUTABLE).exe *.o *.d *.js.mem *.js *.hpp.gch 
else
	$(RM) $(EXECUTABLE) *.o *.d *.js.mem *.js *.hpp.gch $(EXECUTABLE).exe assetcatalog_generated_info.plist
	$(RM) -r $(EXECUTABLE).app
endif