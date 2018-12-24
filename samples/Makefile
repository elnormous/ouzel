MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(realpath $(dir $(MAKEFILE_PATH)))
debug=0
ifeq ($(OS),Windows_NT)
	platform=windows
else
architecture=$(shell uname -m)
os=$(shell uname -s)
ifeq ($(os),Linux)
platform=linux
else ifeq ($(os),Darwin)
platform=macos
endif

endif
ifeq ($(platform),emscripten)
CC=emcc
CXX=em++
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 -I$(ROOT_DIR)/../ouzel
LDFLAGS=-O2 -L$(ROOT_DIR)/../build -louzel
ifeq ($(platform),windows)
LDFLAGS+=-u WinMain -ld3d11 -lopengl32 -ldxguid -lxinput9_1_0 -lshlwapi -ldinput8 -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -ldsound -luuid -lws2_32
else ifeq ($(platform),linux)
ifneq ($(filter arm%,$(architecture)),) # ARM Linux
LDFLAGS+=-L/opt/vc/lib -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lopenal -lpthread -lasound -ldl
else # X86 Linux
LDFLAGS+=-lGL -lopenal -lpthread -lasound -lX11 -lXcursor -lXss -lXi -lXxf86vm
endif
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
LDFLAGS+=-lopenal --embed-file settings.ini --embed-file Resources -s DISABLE_EXCEPTION_CATCHING=0 -s TOTAL_MEMORY=134217728
endif
SOURCES=$(ROOT_DIR)/AnimationsSample.cpp \
	$(ROOT_DIR)/GameSample.cpp \
	$(ROOT_DIR)/GUISample.cpp \
	$(ROOT_DIR)/InputSample.cpp \
	$(ROOT_DIR)/main.cpp \
	$(ROOT_DIR)/MainMenu.cpp \
	$(ROOT_DIR)/PerspectiveSample.cpp \
	$(ROOT_DIR)/SoundSample.cpp \
	$(ROOT_DIR)/SpritesSample.cpp \
	$(ROOT_DIR)/RTSample.cpp
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
else
all: CXXFLAGS+=-O3
endif
all: bundle

.PHONY: bundle
bundle: $(ROOT_DIR)/$(EXECUTABLE)
ifeq ($(platform),macos)
bundle:
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		-e s/'$$(MACOSX_DEPLOYMENT_TARGET)'/10.8/ \
		$(ROOT_DIR)/macos/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Info.plist
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents/MacOS
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app/Contents/MacOS
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources
	cp -f $(ROOT_DIR)/settings.ini $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources/
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist $(ROOT_DIR)/assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info $(ROOT_DIR)/assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device mac --minimum-deployment-target 10.8 \
		--platform macosx --compile $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Resources $(ROOT_DIR)/macos/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Contents/Info.plist
else ifeq ($(platform),ios)
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		$(ROOT_DIR)/ios/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/settings.ini $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist $(ROOT_DIR)/assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info $(ROOT_DIR)/assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device iphone --target-device ipad --minimum-deployment-target 8.0 \
		--platform iphoneos --compile $(ROOT_DIR)/$(EXECUTABLE).app $(ROOT_DIR)/ios/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
else ifeq ($(platform),tvos)
	mkdir -p $(ROOT_DIR)/$(EXECUTABLE).app
	sed -e s/'$$(EXECUTABLE_NAME)'/samples/ \
		-e s/'$$(PRODUCT_BUNDLE_IDENTIFIER)'/lv.elviss.ouzel/ \
		-e s/'$$(PRODUCT_NAME)'/samples/ \
		$(ROOT_DIR)/tvos/Info.plist > $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	cp -f $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/settings.ini $(ROOT_DIR)/$(EXECUTABLE).app
	cp -f $(ROOT_DIR)/Resources/* $(ROOT_DIR)/$(EXECUTABLE).app
	xcrun actool --output-format human-readable-text --notices --warnings \
		--output-partial-info-plist $(ROOT_DIR)/assetcatalog_generated_info.plist --app-icon AppIcon \
		--export-dependency-info $(ROOT_DIR)/assetcatalog_dependencies \
		--enable-on-demand-resources NO --target-device tv --minimum-deployment-target 9.0 \
		--platform appletvos --compile $(ROOT_DIR)/$(EXECUTABLE).app $(ROOT_DIR)/tvos/Assets.xcassets
	plutil -replace CFBundleIconFile -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
	plutil -replace CFBundleIconName -string AppIcon $(ROOT_DIR)/$(EXECUTABLE).app/Info.plist
endif

$(ROOT_DIR)/$(EXECUTABLE): ouzel $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

.PHONY: ouzel
ouzel:
	$(MAKE) -f $(ROOT_DIR)/../build/Makefile debug=$(debug) platform=$(platform) $(target)

.PHONY: clean
clean:
	$(MAKE) -f $(ROOT_DIR)/../build/Makefile clean
ifeq ($(platform),windows)
	-del /f /q "$(ROOT_DIR)\$(EXECUTABLE).exe" "$(ROOT_DIR)\*.o" "$(ROOT_DIR)\*.d" "$(ROOT_DIR)\*.js.mem" "$(ROOT_DIR)\*.js"
else
	$(RM) $(ROOT_DIR)/$(EXECUTABLE) $(ROOT_DIR)/*.o $(ROOT_DIR)/*.d $(ROOT_DIR)/*.js.mem $(ROOT_DIR)/*.js $(ROOT_DIR)/$(EXECUTABLE).exe $(ROOT_DIR)/assetcatalog_generated_info.plist $(ROOT_DIR)/assetcatalog_dependencies
	$(RM) -r $(ROOT_DIR)/$(EXECUTABLE).app
endif