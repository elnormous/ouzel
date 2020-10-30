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
ifeq ($(PLATFORM),emscripten)
CC=emcc
CXX=em++
endif
CXXFLAGS=-std=c++17 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I../engine
LDFLAGS=-L../engine -louzel
ifeq ($(PLATFORM),windows)
LDFLAGS+=-u WinMain -ld3d11 -lopengl32 -ldxguid -lxinput9_1_0 -lshlwapi -lversion -ldinput8 -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -ldsound -luuid -lws2_32
else ifeq ($(PLATFORM),linux)
ifneq ($(filter arm%,$(architecture)),) # ARM Linux
VC_DIR=/opt/vc
LDFLAGS+=-L$(VC_DIR)/lib -lbrcmGLESv2 -lbrcmEGL -lbcm_host
else # X86 Linux
LDFLAGS+=-lGL -lEGL -lX11 -lXcursor -lXss -lXi -lXxf86vm -lXrandr
endif
LDFLAGS+=-lopenal -lpthread -lasound -ldl
else ifeq ($(PLATFORM),macos)
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
else ifeq ($(PLATFORM),ios)
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
else ifeq ($(PLATFORM),tvos)
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
else ifeq ($(PLATFORM),emscripten)
LDFLAGS+=-lopenal --embed-file settings.ini --embed-file Resources -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=134217728
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
ifeq ($(PLATFORM),emscripten)
EXECUTABLE=samples.js
else
EXECUTABLE=samples
endif

.PHONY: all
all: bundle
ifeq ($(DEBUG),1)
all: CXXFLAGS+=-DDEBUG -g
else
all: CXXFLAGS+=-O3
all: LDFLAGS+=-O3
endif

.PHONY: bundle
bundle: $(EXECUTABLE)
ifeq ($(PLATFORM),macos)
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
else ifeq ($(PLATFORM),ios)
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
else ifeq ($(PLATFORM),tvos)
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

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: ouzel
ouzel:
	$(MAKE) -C ../engine/ DEBUG=$(DEBUG) PLATFORM=$(PLATFORM) VC_DIR=$(VC_DIR) $(target)

.PHONY: clean
clean:
	$(MAKE) -C ../engine/ clean
ifeq ($(PLATFORM),windows)
	-del /f /q "$(EXECUTABLE).exe" "*.o" "*.d" "*.js.mem" "*.js"
else
	$(RM) $(EXECUTABLE) *.o *.d *.js.mem *.js $(EXECUTABLE).exe assetcatalog_generated_info.plist assetcatalog_dependencies
	$(RM) -r $(EXECUTABLE).app
endif