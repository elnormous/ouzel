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
CFLAGS=-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion \
	-I"../engine" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
CXXFLAGS=-std=c++14 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Wold-style-cast \
	-I"../engine" \
	-I"../shaders" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCFLAGS=-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion \
	-I"../engine" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCXXFLAGS=-std=c++14 \
	-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Wold-style-cast \
	-I"../engine" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
SOURCES=../engine/assets/BmfLoader.cpp \
	../engine/assets/Bundle.cpp \
	../engine/assets/Cache.cpp \
	../engine/assets/ColladaLoader.cpp \
	../engine/assets/CueLoader.cpp \
	../engine/assets/GltfLoader.cpp \
	../engine/assets/ImageLoader.cpp \
	../engine/assets/MtlLoader.cpp \
	../engine/assets/ObjLoader.cpp \
	../engine/assets/ParticleSystemLoader.cpp \
	../engine/assets/SpriteLoader.cpp \
	../engine/assets/TtfLoader.cpp \
	../engine/assets/VorbisLoader.cpp \
	../engine/assets/WaveLoader.cpp \
	../engine/audio/empty/EmptyAudioDevice.cpp \
	../engine/audio/mixer/Bus.cpp \
	../engine/audio/mixer/Mixer.cpp \
	../engine/audio/Audio.cpp \
	../engine/audio/AudioDevice.cpp \
	../engine/audio/Containers.cpp \
	../engine/audio/Cue.cpp \
	../engine/audio/Effect.cpp \
	../engine/audio/Effects.cpp \
	../engine/audio/Listener.cpp \
	../engine/audio/Mix.cpp \
	../engine/audio/Node.cpp \
	../engine/audio/Oscillator.cpp \
	../engine/audio/PcmClip.cpp \
	../engine/audio/SilenceSound.cpp \
	../engine/audio/Sound.cpp \
	../engine/audio/Submix.cpp \
	../engine/audio/Voice.cpp \
	../engine/audio/VorbisClip.cpp \
	../engine/core/Engine.cpp \
	../engine/core/System.cpp \
	../engine/core/NativeWindow.cpp \
	../engine/core/Window.cpp \
	../engine/events/EventDispatcher.cpp \
	../engine/graphics/opengl/OGLBlendState.cpp \
	../engine/graphics/opengl/OGLBuffer.cpp \
	../engine/graphics/opengl/OGLDepthStencilState.cpp \
	../engine/graphics/opengl/OGLRenderDevice.cpp \
	../engine/graphics/opengl/OGLRenderTarget.cpp \
	../engine/graphics/opengl/OGLShader.cpp \
	../engine/graphics/opengl/OGLTexture.cpp \
	../engine/graphics/BlendState.cpp \
	../engine/graphics/Buffer.cpp \
	../engine/graphics/DepthStencilState.cpp \
	../engine/graphics/Renderer.cpp \
	../engine/graphics/RenderDevice.cpp \
	../engine/graphics/RenderTarget.cpp \
	../engine/graphics/Shader.cpp \
	../engine/graphics/Texture.cpp \
	../engine/gui/BMFont.cpp \
	../engine/gui/TTFont.cpp \
	../engine/gui/Widget.cpp \
	../engine/gui/Widgets.cpp \
	../engine/input/Cursor.cpp \
	../engine/input/Gamepad.cpp \
	../engine/input/GamepadDevice.cpp \
	../engine/input/InputDevice.cpp \
	../engine/input/InputManager.cpp \
	../engine/input/InputSystem.cpp \
	../engine/input/Keyboard.cpp \
	../engine/input/KeyboardDevice.cpp \
	../engine/input/Mouse.cpp \
	../engine/input/MouseDevice.cpp \
	../engine/input/Touchpad.cpp \
	../engine/input/TouchpadDevice.cpp \
	../engine/localization/Localization.cpp \
	../engine/math/MathUtils.cpp \
	../engine/math/Matrix.cpp \
	../engine/network/Client.cpp \
	../engine/network/Network.cpp \
	../engine/network/Server.cpp \
	../engine/scene/Actor.cpp \
	../engine/scene/Animator.cpp \
	../engine/scene/Animators.cpp \
	../engine/scene/Camera.cpp \
	../engine/scene/Component.cpp \
	../engine/scene/Layer.cpp \
	../engine/scene/Light.cpp \
	../engine/scene/ParticleSystem.cpp \
	../engine/scene/Scene.cpp \
	../engine/scene/SceneManager.cpp \
	../engine/scene/ShapeRenderer.cpp \
	../engine/scene/SkinnedMeshRenderer.cpp \
	../engine/scene/SpriteRenderer.cpp \
	../engine/scene/StaticMeshRenderer.cpp \
	../engine/scene/TextRenderer.cpp \
	../engine/storage/FileSystem.cpp \
	../engine/utils/Log.cpp \
	../engine/utils/Obf.cpp \
	../engine/utils/Utils.cpp
ifeq ($(PLATFORM),windows)
SOURCES+=../engine/audio/dsound/DSAudioDevice.cpp \
	../engine/core/windows/EngineWin.cpp \
	../engine/core/windows/main.cpp \
	../engine/core/windows/NativeWindowWin.cpp \
	../engine/core/windows/SystemWin.cpp \
	../engine/graphics/direct3d11/D3D11BlendState.cpp \
	../engine/graphics/direct3d11/D3D11Buffer.cpp \
	../engine/graphics/direct3d11/D3D11DepthStencilState.cpp \
	../engine/graphics/direct3d11/D3D11RenderDevice.cpp \
	../engine/graphics/direct3d11/D3D11Shader.cpp \
	../engine/graphics/direct3d11/D3D11Texture.cpp \
	../engine/graphics/opengl/windows/OGLRenderDeviceWin.cpp \
	../engine/input/windows/GamepadDeviceDI.cpp \
	../engine/input/windows/GamepadDeviceXI.cpp \
	../engine/input/windows/InputSystemWin.cpp \
	../engine/input/windows/KeyboardDeviceWin.cpp \
	../engine/input/windows/MouseDeviceWin.cpp \
	../engine/input/windows/CursorWin.cpp
CPPFLAGS+=-DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(PLATFORM),linux)
SOURCES+=../engine/audio/alsa/ALSAAudioDevice.cpp \
	../engine/audio/openal/OALAudioDevice.cpp \
	../engine/core/linux/EngineLinux.cpp \
	../engine/core/linux/main.cpp \
	../engine/core/linux/NativeWindowLinux.cpp \
	../engine/core/linux/SystemLinux.cpp \
	../engine/graphics/opengl/linux/OGLRenderDeviceLinux.cpp \
	../engine/input/linux/EventDevice.cpp \
	../engine/input/linux/InputSystemLinux.cpp \
	../engine/input/linux/KeyboardDeviceLinux.cpp \
	../engine/input/linux/MouseDeviceLinux.cpp \
	../engine/input/linux/CursorLinux.cpp
ifneq ($(filter arm%,$(architecture)),) # ARM Linux
VC_DIR=/opt/vc
CFLAGS+=-I$(VC_DIR)/include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
CXXFLAGS+=-I$(VC_DIR)include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
endif
else ifeq ($(PLATFORM),macos)
SOURCES+=../engine/audio/coreaudio/CAAudioDevice.mm \
	../engine/audio/openal/OALAudioDevice.cpp \
	../engine/core/macos/EngineMacOS.mm \
	../engine/core/macos/main.cpp \
	../engine/core/macos/NativeWindowMacOS.mm \
	../engine/core/macos/SystemMacOS.mm \
	../engine/core/macos/ViewMacOS.mm \
	../engine/graphics/metal/macos/MetalView.m \
	../engine/graphics/metal/macos/MetalRenderDeviceMacOS.mm \
	../engine/graphics/metal/MetalBlendState.mm \
	../engine/graphics/metal/MetalBuffer.mm \
	../engine/graphics/metal/MetalDepthStencilState.mm \
	../engine/graphics/metal/MetalRenderDevice.mm \
	../engine/graphics/metal/MetalRenderTarget.mm \
	../engine/graphics/metal/MetalShader.mm \
	../engine/graphics/metal/MetalTexture.mm \
	../engine/graphics/opengl/macos/OGLRenderDeviceMacOS.mm \
	../engine/graphics/opengl/macos/OpenGLView.m \
	../engine/input/macos/GamepadDeviceGC.mm \
	../engine/input/macos/GamepadDeviceIOKit.cpp \
	../engine/input/macos/InputSystemMacOS.mm \
	../engine/input/macos/MouseDeviceMacOS.mm \
	../engine/input/macos/CursorMacOS.cpp
else ifeq ($(PLATFORM),ios)
SOURCES+=../engine/audio/coreaudio/CAAudioDevice.mm \
	../engine/audio/openal/OALAudioDevice.cpp \
	../engine/core/ios/DisplayLink.mm \
	../engine/core/ios/EngineIOS.mm \
	../engine/core/ios/main.cpp \
	../engine/core/ios/NativeWindowIOS.mm \
	../engine/core/ios/SystemIOS.mm \
	../engine/core/ios/ViewIOS.mm \
	../engine/graphics/metal/ios/MetalView.m \
	../engine/graphics/metal/ios/MetalRenderDeviceIOS.mm \
	../engine/graphics/metal/MetalBlendState.mm \
	../engine/graphics/metal/MetalBuffer.mm \
	../engine/graphics/metal/MetalDepthStencilState.mm \
	../engine/graphics/metal/MetalRenderDevice.mm \
	../engine/graphics/metal/MetalRenderTarget.mm \
	../engine/graphics/metal/MetalShader.mm \
	../engine/graphics/metal/MetalTexture.mm \
	../engine/graphics/opengl/ios/OGLRenderDeviceIOS.mm \
	../engine/graphics/opengl/ios/OpenGLView.m \
	../engine/input/ios/GamepadDeviceIOS.mm \
	../engine/input/ios/InputSystemIOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
else ifeq ($(PLATFORM),tvos)
SOURCES+=../engine/audio/coreaudio/CAAudioDevice.mm \
	../engine/audio/openal/OALAudioDevice.cpp \
	../engine/core/tvos/DisplayLink.mm \
	../engine/core/tvos/EngineTVOS.mm \
	../engine/core/tvos/main.cpp \
	../engine/core/tvos/NativeWindowTVOS.mm \
	../engine/core/tvos/SystemTVOS.mm \
	../engine/core/tvos/ViewTVOS.mm \
	../engine/graphics/metal/tvos/MetalView.m \
	../engine/graphics/metal/tvos/MetalRenderDeviceTVOS.mm \
	../engine/graphics/metal/MetalBlendState.mm \
	../engine/graphics/metal/MetalBuffer.mm \
	../engine/graphics/metal/MetalDepthStencilState.mm \
	../engine/graphics/metal/MetalRenderDevice.mm \
	../engine/graphics/metal/MetalRenderTarget.mm \
	../engine/graphics/metal/MetalShader.mm \
	../engine/graphics/metal/MetalTexture.mm \
	../engine/graphics/opengl/tvos/OGLRenderDeviceTVOS.mm \
	../engine/graphics/opengl/tvos/OpenGLView.m \
	../engine/input/tvos/GamepadDeviceTVOS.mm \
	../engine/input/tvos/InputSystemTVOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
else ifeq ($(PLATFORM),emscripten)
SOURCES+=../engine/audio/openal/OALAudioDevice.cpp \
	../engine/core/emscripten/EngineEm.cpp \
	../engine/core/emscripten/main.cpp \
	../engine/core/emscripten/NativeWindowEm.cpp \
	../engine/core/emscripten/SystemEm.cpp \
	../engine/graphics/opengl/emscripten/OGLRenderDeviceEm.cpp \
	../engine/input/emscripten/GamepadDeviceEm.cpp \
	../engine/input/emscripten/InputSystemEm.cpp \
	../engine/input/emscripten/MouseDeviceEm.cpp
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
../engine/Config.h:
ifeq ($(PLATFORM),windows)
	if not exist ..\engine\Config.h copy ..\engine\DefaultConfig.h ..\engine\Config.h
else
	if [ ! -f ../engine/Config.h ]; then \
		cp ../engine/DefaultConfig.h ../engine/Config.h; \
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
%.o: %.cpp pch/cpp/Prefix.h.gch ../engine/Config.h
	$(CXX) -c -x c++ $(CPPFLAGS) $(CXXFLAGS) -include pch/cpp/Prefix.h -MMD -MP $< -o $@

%.o: %.m pch/objc/Prefix.h.gch ../engine/Config.h
	$(CC) -c -x objective-c $(CPPFLAGS) $(OBJCFLAGS) -include pch/objc/Prefix.h -MMD -MP $< -o $@

%.o: %.mm pch/objcpp/Prefix.h.gch ../engine/Config.h
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