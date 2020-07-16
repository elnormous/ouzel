ifeq ($(OS),Windows_NT)
PLATFORM=windows
else
architecture=$(shell uname -m)
os=$(shell uname -s)
ifeq ($(os),Linux)
PLATFORM=linux
else ifeq ($(os),FreeBSD)
PLATFORM=linux
else ifeq ($(os),NetBSD)
PLATFORM=linux
else ifeq ($(os),DragonFly)
PLATFORM=linux
else ifeq ($(os),Solaris)
PLATFORM=linux
else ifeq ($(os),Darwin)
PLATFORM=macos
else ifeq ($(os),Haiku)
PLATFORM=haiku
endif
endif
ifeq ($(PLATFORM),emscripten)
CC=emcc
CXX=em++
AR=emar
endif
CFLAGS=-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
CXXFLAGS=-std=c++17 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I"../shaders" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCFLAGS=-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCXXFLAGS=-std=c++17 \
	-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Woverloaded-virtual -Wold-style-cast \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
SOURCES=assets/BmfLoader.cpp \
	assets/Bundle.cpp \
	assets/Cache.cpp \
	assets/ColladaLoader.cpp \
	assets/CueLoader.cpp \
	assets/GltfLoader.cpp \
	assets/ImageLoader.cpp \
	assets/MtlLoader.cpp \
	assets/ObjLoader.cpp \
	assets/ParticleSystemLoader.cpp \
	assets/SpriteLoader.cpp \
	assets/TtfLoader.cpp \
	assets/VorbisLoader.cpp \
	assets/WaveLoader.cpp \
	audio/mixer/Bus.cpp \
	audio/mixer/Mixer.cpp \
	audio/Audio.cpp \
	audio/AudioDevice.cpp \
	audio/Containers.cpp \
	audio/Cue.cpp \
	audio/Effect.cpp \
	audio/Effects.cpp \
	audio/Listener.cpp \
	audio/Mix.cpp \
	audio/Node.cpp \
	audio/Oscillator.cpp \
	audio/PcmClip.cpp \
	audio/SilenceSound.cpp \
	audio/Sound.cpp \
	audio/Submix.cpp \
	audio/Voice.cpp \
	audio/VorbisClip.cpp \
	core/Engine.cpp \
	core/System.cpp \
	core/NativeWindow.cpp \
	core/Window.cpp \
	events/EventDispatcher.cpp \
	graphics/opengl/OGLBlendState.cpp \
	graphics/opengl/OGLBuffer.cpp \
	graphics/opengl/OGLDepthStencilState.cpp \
	graphics/opengl/OGLRenderDevice.cpp \
	graphics/opengl/OGLRenderTarget.cpp \
	graphics/opengl/OGLShader.cpp \
	graphics/opengl/OGLTexture.cpp \
	graphics/renderer/Renderer.cpp \
	graphics/BlendState.cpp \
	graphics/Buffer.cpp \
	graphics/DepthStencilState.cpp \
	graphics/Graphics.cpp \
	graphics/RenderDevice.cpp \
	graphics/RenderTarget.cpp \
	graphics/Shader.cpp \
	graphics/Texture.cpp \
	gui/BMFont.cpp \
	gui/TTFont.cpp \
	gui/Widget.cpp \
	gui/Widgets.cpp \
	input/Cursor.cpp \
	input/Gamepad.cpp \
	input/GamepadDevice.cpp \
	input/InputDevice.cpp \
	input/InputManager.cpp \
	input/InputSystem.cpp \
	input/Keyboard.cpp \
	input/KeyboardDevice.cpp \
	input/Mouse.cpp \
	input/MouseDevice.cpp \
	input/Touchpad.cpp \
	input/TouchpadDevice.cpp \
	localization/Localization.cpp \
	math/MathUtils.cpp \
	math/Matrix.cpp \
	network/Client.cpp \
	network/Network.cpp \
	network/Server.cpp \
	scene/Actor.cpp \
	scene/Animator.cpp \
	scene/Animators.cpp \
	scene/Camera.cpp \
	scene/Component.cpp \
	scene/Layer.cpp \
	scene/Light.cpp \
	scene/ParticleSystem.cpp \
	scene/Scene.cpp \
	scene/SceneManager.cpp \
	scene/ShapeRenderer.cpp \
	scene/SkinnedMeshRenderer.cpp \
	scene/SpriteRenderer.cpp \
	scene/StaticMeshRenderer.cpp \
	scene/TextRenderer.cpp \
	storage/FileSystem.cpp \
	utils/Log.cpp \
	utils/Utils.cpp
ifeq ($(PLATFORM),windows)
SOURCES+=audio/dsound/DSAudioDevice.cpp \
	core/windows/EngineWin.cpp \
	core/windows/NativeWindowWin.cpp \
	core/windows/SystemWin.cpp \
	graphics/direct3d11/D3D11BlendState.cpp \
	graphics/direct3d11/D3D11Buffer.cpp \
	graphics/direct3d11/D3D11DepthStencilState.cpp \
	graphics/direct3d11/D3D11RenderDevice.cpp \
	graphics/direct3d11/D3D11Shader.cpp \
	graphics/direct3d11/D3D11Texture.cpp \
	graphics/opengl/windows/OGLRenderDeviceWin.cpp \
	input/windows/GamepadDeviceDI.cpp \
	input/windows/GamepadDeviceWin.cpp \
	input/windows/GamepadDeviceXI.cpp \
	input/windows/InputSystemWin.cpp \
	input/windows/KeyboardDeviceWin.cpp \
	input/windows/MouseDeviceWin.cpp \
	input/windows/CursorWin.cpp
CPPFLAGS+=-DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(PLATFORM),linux)
SOURCES+=audio/alsa/ALSAAudioDevice.cpp \
	audio/openal/OALAudioDevice.cpp \
	core/linux/EngineLinux.cpp \
	core/linux/NativeWindowLinux.cpp \
	core/linux/SystemLinux.cpp \
	graphics/opengl/linux/OGLRenderDeviceLinux.cpp \
	input/linux/EventDevice.cpp \
	input/linux/InputSystemLinux.cpp \
	input/linux/KeyboardDeviceLinux.cpp \
	input/linux/MouseDeviceLinux.cpp \
	input/linux/CursorLinux.cpp
ifneq ($(filter arm%,$(architecture)),) # ARM Linux
VC_DIR=/opt/vc
CFLAGS+=-I$(VC_DIR)/include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
CXXFLAGS+=-I$(VC_DIR)include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
endif
else ifeq ($(PLATFORM),macos)
SOURCES+=audio/coreaudio/CAAudioDevice.mm \
	audio/openal/OALAudioDevice.cpp \
	core/macos/EngineMacOS.mm \
	core/macos/NativeWindowMacOS.mm \
	core/macos/SystemMacOS.mm \
	core/macos/ViewMacOS.mm \
	graphics/metal/macos/MetalView.m \
	graphics/metal/macos/MetalRenderDeviceMacOS.mm \
	graphics/metal/MetalBlendState.mm \
	graphics/metal/MetalBuffer.mm \
	graphics/metal/MetalDepthStencilState.mm \
	graphics/metal/MetalRenderDevice.mm \
	graphics/metal/MetalRenderTarget.mm \
	graphics/metal/MetalShader.mm \
	graphics/metal/MetalTexture.mm \
	graphics/opengl/macos/OGLRenderDeviceMacOS.mm \
	graphics/opengl/macos/OpenGLView.m \
	input/macos/GamepadDeviceGC.mm \
	input/macos/GamepadDeviceIOKit.cpp \
	input/macos/GamepadDeviceMacOS.cpp \
	input/macos/InputSystemMacOS.mm \
	input/macos/MouseDeviceMacOS.mm \
	input/macos/CursorMacOS.cpp
else ifeq ($(PLATFORM),ios)
SOURCES+=audio/coreaudio/CAAudioDevice.mm \
	audio/openal/OALAudioDevice.cpp \
	core/ios/DisplayLink.mm \
	core/ios/EngineIOS.mm \
	core/ios/NativeWindowIOS.mm \
	core/ios/SystemIOS.mm \
	core/ios/ViewIOS.mm \
	graphics/metal/ios/MetalView.m \
	graphics/metal/ios/MetalRenderDeviceIOS.mm \
	graphics/metal/MetalBlendState.mm \
	graphics/metal/MetalBuffer.mm \
	graphics/metal/MetalDepthStencilState.mm \
	graphics/metal/MetalRenderDevice.mm \
	graphics/metal/MetalRenderTarget.mm \
	graphics/metal/MetalShader.mm \
	graphics/metal/MetalTexture.mm \
	graphics/opengl/ios/OGLRenderDeviceIOS.mm \
	graphics/opengl/ios/OpenGLView.m \
	input/ios/GamepadDeviceIOS.mm \
	input/ios/InputSystemIOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
else ifeq ($(PLATFORM),tvos)
SOURCES+=audio/coreaudio/CAAudioDevice.mm \
	audio/openal/OALAudioDevice.cpp \
	core/tvos/DisplayLink.mm \
	core/tvos/EngineTVOS.mm \
	core/tvos/NativeWindowTVOS.mm \
	core/tvos/SystemTVOS.mm \
	core/tvos/ViewTVOS.mm \
	graphics/metal/tvos/MetalView.m \
	graphics/metal/tvos/MetalRenderDeviceTVOS.mm \
	graphics/metal/MetalBlendState.mm \
	graphics/metal/MetalBuffer.mm \
	graphics/metal/MetalDepthStencilState.mm \
	graphics/metal/MetalRenderDevice.mm \
	graphics/metal/MetalRenderTarget.mm \
	graphics/metal/MetalShader.mm \
	graphics/metal/MetalTexture.mm \
	graphics/opengl/tvos/OGLRenderDeviceTVOS.mm \
	graphics/opengl/tvos/OpenGLView.m \
	input/tvos/GamepadDeviceTVOS.mm \
	input/tvos/InputSystemTVOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
else ifeq ($(PLATFORM),emscripten)
SOURCES+=audio/openal/OALAudioDevice.cpp \
	core/emscripten/EngineEm.cpp \
	core/emscripten/NativeWindowEm.cpp \
	core/emscripten/SystemEm.cpp \
	graphics/opengl/emscripten/OGLRenderDeviceEm.cpp \
	input/emscripten/GamepadDeviceEm.cpp \
	input/emscripten/InputSystemEm.cpp \
	input/emscripten/MouseDeviceEm.cpp
CFLAGS+=-s DISABLE_EXCEPTION_CATCHING=0
CXXFLAGS+=-s DISABLE_EXCEPTION_CATCHING=0
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
LIBRARY=libouzel.a
DEPENDENCIES=$(OBJECTS:.o=.d)

.PHONY: all
ifeq ($(DEBUG),1)
all: CPPFLAGS+=-DDEBUG # C preprocessor flags
all: CFLAGS+=-g # C flags
all: CXXFLAGS+=-g # C++ flags
all: OBJCFLAGS+=-g # Objective-C flags
all: OBJCXXFLAGS+=-g # Objective-C++ flags
else
all: CFLAGS+=-O3
all: CXXFLAGS+=-O3
all: OBJCFLAGS+=-O3
all: OBJCXXFLAGS+=-O3
all: LDFLAGS+=-O3
endif
.PHONY: all
all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

-include $(DEPENDENCIES)

# The build config file
Config.h:
ifeq ($(PLATFORM),windows)
	if not exist Config.h copy DefaultConfig.h Config.h
else
	if [ ! -f Config.h ]; then \
		cp DefaultConfig.h Config.h; \
	fi
endif

# Precompiled headers
pch/cpp/Prefix.h: Prefix.pch
ifeq ($(PLATFORM),windows)
	if not exist pch md pch
	if not exist pch\cpp md pch\cpp
	copy Prefix.pch pch\cpp\Prefix.h
else
	mkdir -p pch/cpp
	cp Prefix.pch pch/cpp/Prefix.h
endif

pch/objc/Prefix.h: Prefix.pch
ifeq ($(PLATFORM),windows)
	if not exist pch md pch
	if not exist pch\objc md pch\objc
	copy Prefix.pch pch\objc\Prefix.h
else
	mkdir -p pch/objc
	cp Prefix.pch pch/objc/Prefix.h
endif

pch/objcpp/Prefix.h: Prefix.pch
ifeq ($(PLATFORM),windows)
	if not exist pch md pch
	if not exist pch\objcpp md pch\objcpp
	copy Prefix.pch pch\objcpp\Prefix.h
else
	mkdir -p pch/objcpp
	cp Prefix.pch pch/objcpp/Prefix.h
endif

pch/cpp/Prefix.h.gch: pch/cpp/Prefix.h
	$(CXX) -c -x c++-header $(CPPFLAGS) $(CXXFLAGS) $< -o pch/cpp/Prefix.h.gch

pch/objc/Prefix.h.gch: pch/objc/Prefix.h
	$(CC) -c -x objective-c-header $(CPPFLAGS) $(OBJCFLAGS) $< -o pch/objc/Prefix.h.gch

pch/objcpp/Prefix.h.gch: pch/objcpp/Prefix.h
	$(CXX) -c -x objective-c++-header $(CPPFLAGS) $(OBJCXXFLAGS) $< -o pch/objcpp/Prefix.h.gch

# Source files
%.o: %.cpp pch/cpp/Prefix.h.gch Config.h
	$(CXX) -c -x c++ $(CPPFLAGS) $(CXXFLAGS) -include pch/cpp/Prefix.h -MMD -MP $< -o $@

%.o: %.m pch/objc/Prefix.h.gch Config.h
	$(CC) -c -x objective-c $(CPPFLAGS) $(OBJCFLAGS) -include pch/objc/Prefix.h -MMD -MP $< -o $@

%.o: %.mm pch/objcpp/Prefix.h.gch Config.h
	$(CXX) -c -x objective-c++ $(CPPFLAGS) $(OBJCXXFLAGS) -include pch/objcpp/Prefix.h -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(PLATFORM),windows)
	-del /f /q "$(LIBRARY)"
	-rmdir /s pch
	-del /s "..\*.o"
	-del /s "..\*.d"
else
	$(RM) $(LIBRARY)
	$(RM) -rf pch
	find .. -name '*.o' -type f -delete
	find .. -name '*.d' -type f -delete
endif