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
	-I"../ouzel" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
CXXFLAGS=-std=c++14 \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Wold-style-cast \
	-I"../ouzel" \
	-I"../shaders" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCFLAGS=-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion \
	-I"../ouzel" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
OBJCXXFLAGS=-std=c++14 \
	-fno-objc-arc \
	-Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion -Wold-style-cast \
	-I"../ouzel" \
	-I"../external/khronos" \
	-I"../external/smbPitchShift" \
	-I"../external/stb"
SOURCES=../ouzel/assets/BmfLoader.cpp \
	../ouzel/assets/Bundle.cpp \
	../ouzel/assets/Cache.cpp \
	../ouzel/assets/ColladaLoader.cpp \
	../ouzel/assets/CueLoader.cpp \
	../ouzel/assets/GltfLoader.cpp \
	../ouzel/assets/ImageLoader.cpp \
	../ouzel/assets/MtlLoader.cpp \
	../ouzel/assets/ObjLoader.cpp \
	../ouzel/assets/ParticleSystemLoader.cpp \
	../ouzel/assets/SpriteLoader.cpp \
	../ouzel/assets/TtfLoader.cpp \
	../ouzel/assets/VorbisLoader.cpp \
	../ouzel/assets/WaveLoader.cpp \
	../ouzel/assets/KtxLoader.cpp \
	../ouzel/audio/empty/EmptyAudioDevice.cpp \
	../ouzel/audio/mixer/Bus.cpp \
	../ouzel/audio/mixer/Mixer.cpp \
	../ouzel/audio/Audio.cpp \
	../ouzel/audio/AudioDevice.cpp \
	../ouzel/audio/Containers.cpp \
	../ouzel/audio/Cue.cpp \
	../ouzel/audio/Effect.cpp \
	../ouzel/audio/Effects.cpp \
	../ouzel/audio/Listener.cpp \
	../ouzel/audio/Mix.cpp \
	../ouzel/audio/Node.cpp \
	../ouzel/audio/Oscillator.cpp \
	../ouzel/audio/PcmClip.cpp \
	../ouzel/audio/SilenceSound.cpp \
	../ouzel/audio/Sound.cpp \
	../ouzel/audio/Submix.cpp \
	../ouzel/audio/Voice.cpp \
	../ouzel/audio/VorbisClip.cpp \
	../ouzel/core/Engine.cpp \
	../ouzel/core/System.cpp \
	../ouzel/core/NativeWindow.cpp \
	../ouzel/core/Window.cpp \
	../ouzel/events/EventDispatcher.cpp \
	../ouzel/graphics/opengl/OGLBlendState.cpp \
	../ouzel/graphics/opengl/OGLBuffer.cpp \
	../ouzel/graphics/opengl/OGLDepthStencilState.cpp \
	../ouzel/graphics/opengl/OGLRenderDevice.cpp \
	../ouzel/graphics/opengl/OGLRenderTarget.cpp \
	../ouzel/graphics/opengl/OGLShader.cpp \
	../ouzel/graphics/opengl/OGLTexture.cpp \
	../ouzel/graphics/BlendState.cpp \
	../ouzel/graphics/Buffer.cpp \
	../ouzel/graphics/DepthStencilState.cpp \
	../ouzel/graphics/Renderer.cpp \
	../ouzel/graphics/RenderDevice.cpp \
	../ouzel/graphics/RenderTarget.cpp \
	../ouzel/graphics/Shader.cpp \
	../ouzel/graphics/Texture.cpp \
	../ouzel/gui/BMFont.cpp \
	../ouzel/gui/TTFont.cpp \
	../ouzel/gui/Widget.cpp \
	../ouzel/gui/Widgets.cpp \
	../ouzel/input/Cursor.cpp \
	../ouzel/input/Gamepad.cpp \
	../ouzel/input/GamepadDevice.cpp \
	../ouzel/input/InputDevice.cpp \
	../ouzel/input/InputManager.cpp \
	../ouzel/input/InputSystem.cpp \
	../ouzel/input/Keyboard.cpp \
	../ouzel/input/KeyboardDevice.cpp \
	../ouzel/input/Mouse.cpp \
	../ouzel/input/MouseDevice.cpp \
	../ouzel/input/Touchpad.cpp \
	../ouzel/input/TouchpadDevice.cpp \
	../ouzel/localization/Localization.cpp \
	../ouzel/math/MathUtils.cpp \
	../ouzel/math/Matrix.cpp \
	../ouzel/network/Client.cpp \
	../ouzel/network/Network.cpp \
	../ouzel/network/Server.cpp \
	../ouzel/scene/Actor.cpp \
	../ouzel/scene/Animator.cpp \
	../ouzel/scene/Animators.cpp \
	../ouzel/scene/Camera.cpp \
	../ouzel/scene/Component.cpp \
	../ouzel/scene/Layer.cpp \
	../ouzel/scene/Light.cpp \
	../ouzel/scene/ParticleSystem.cpp \
	../ouzel/scene/Scene.cpp \
	../ouzel/scene/SceneManager.cpp \
	../ouzel/scene/ShapeRenderer.cpp \
	../ouzel/scene/SkinnedMeshRenderer.cpp \
	../ouzel/scene/SpriteRenderer.cpp \
	../ouzel/scene/StaticMeshRenderer.cpp \
	../ouzel/scene/TextRenderer.cpp \
	../ouzel/storage/FileSystem.cpp \
	../ouzel/utils/Log.cpp \
	../ouzel/utils/Obf.cpp \
	../ouzel/utils/Utils.cpp
ifeq ($(PLATFORM),windows)
SOURCES+=../ouzel/audio/dsound/DSAudioDevice.cpp \
	../ouzel/core/windows/EngineWin.cpp \
	../ouzel/core/windows/main.cpp \
	../ouzel/core/windows/NativeWindowWin.cpp \
	../ouzel/core/windows/SystemWin.cpp \
	../ouzel/graphics/direct3d11/D3D11BlendState.cpp \
	../ouzel/graphics/direct3d11/D3D11Buffer.cpp \
	../ouzel/graphics/direct3d11/D3D11DepthStencilState.cpp \
	../ouzel/graphics/direct3d11/D3D11RenderDevice.cpp \
	../ouzel/graphics/direct3d11/D3D11Shader.cpp \
	../ouzel/graphics/direct3d11/D3D11Texture.cpp \
	../ouzel/graphics/opengl/windows/OGLRenderDeviceWin.cpp \
	../ouzel/input/windows/GamepadDeviceDI.cpp \
	../ouzel/input/windows/GamepadDeviceXI.cpp \
	../ouzel/input/windows/InputSystemWin.cpp \
	../ouzel/input/windows/KeyboardDeviceWin.cpp \
	../ouzel/input/windows/MouseDeviceWin.cpp \
	../ouzel/input/windows/CursorWin.cpp
CPPFLAGS+=-DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(PLATFORM),linux)
SOURCES+=../ouzel/audio/alsa/ALSAAudioDevice.cpp \
	../ouzel/audio/openal/OALAudioDevice.cpp \
	../ouzel/core/linux/EngineLinux.cpp \
	../ouzel/core/linux/main.cpp \
	../ouzel/core/linux/NativeWindowLinux.cpp \
	../ouzel/core/linux/SystemLinux.cpp \
	../ouzel/graphics/opengl/linux/OGLRenderDeviceLinux.cpp \
	../ouzel/input/linux/EventDevice.cpp \
	../ouzel/input/linux/InputSystemLinux.cpp \
	../ouzel/input/linux/KeyboardDeviceLinux.cpp \
	../ouzel/input/linux/MouseDeviceLinux.cpp \
	../ouzel/input/linux/CursorLinux.cpp
ifneq ($(filter arm%,$(architecture)),) # ARM Linux
VC_DIR=/opt/vc
CFLAGS+=-I$(VC_DIR)/include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
CXXFLAGS+=-I$(VC_DIR)include -I$(VC_DIR)/include/interface/vcos/pthreads -I$(VC_DIR)/include/interface/vmcs_host/linux
endif
else ifeq ($(PLATFORM),macos)
SOURCES+=../ouzel/audio/coreaudio/CAAudioDevice.mm \
	../ouzel/audio/openal/OALAudioDevice.cpp \
	../ouzel/core/macos/EngineMacOS.mm \
	../ouzel/core/macos/main.cpp \
	../ouzel/core/macos/NativeWindowMacOS.mm \
	../ouzel/core/macos/SystemMacOS.mm \
	../ouzel/core/macos/ViewMacOS.mm \
	../ouzel/graphics/metal/macos/MetalView.m \
	../ouzel/graphics/metal/macos/MetalRenderDeviceMacOS.mm \
	../ouzel/graphics/metal/MetalBlendState.mm \
	../ouzel/graphics/metal/MetalBuffer.mm \
	../ouzel/graphics/metal/MetalDepthStencilState.mm \
	../ouzel/graphics/metal/MetalRenderDevice.mm \
	../ouzel/graphics/metal/MetalRenderTarget.mm \
	../ouzel/graphics/metal/MetalShader.mm \
	../ouzel/graphics/metal/MetalTexture.mm \
	../ouzel/graphics/opengl/macos/OGLRenderDeviceMacOS.mm \
	../ouzel/graphics/opengl/macos/OpenGLView.m \
	../ouzel/input/macos/GamepadDeviceGC.mm \
	../ouzel/input/macos/GamepadDeviceIOKit.cpp \
	../ouzel/input/macos/InputSystemMacOS.mm \
	../ouzel/input/macos/MouseDeviceMacOS.mm \
	../ouzel/input/macos/CursorMacOS.cpp
else ifeq ($(PLATFORM),ios)
SOURCES+=../ouzel/audio/coreaudio/CAAudioDevice.mm \
	../ouzel/audio/openal/OALAudioDevice.cpp \
	../ouzel/core/ios/DisplayLink.mm \
	../ouzel/core/ios/EngineIOS.mm \
	../ouzel/core/ios/main.cpp \
	../ouzel/core/ios/NativeWindowIOS.mm \
	../ouzel/core/ios/SystemIOS.mm \
	../ouzel/core/ios/ViewIOS.mm \
	../ouzel/graphics/metal/ios/MetalView.m \
	../ouzel/graphics/metal/ios/MetalRenderDeviceIOS.mm \
	../ouzel/graphics/metal/MetalBlendState.mm \
	../ouzel/graphics/metal/MetalBuffer.mm \
	../ouzel/graphics/metal/MetalDepthStencilState.mm \
	../ouzel/graphics/metal/MetalRenderDevice.mm \
	../ouzel/graphics/metal/MetalRenderTarget.mm \
	../ouzel/graphics/metal/MetalShader.mm \
	../ouzel/graphics/metal/MetalTexture.mm \
	../ouzel/graphics/opengl/ios/OGLRenderDeviceIOS.mm \
	../ouzel/graphics/opengl/ios/OpenGLView.m \
	../ouzel/input/ios/GamepadDeviceIOS.mm \
	../ouzel/input/ios/InputSystemIOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
else ifeq ($(PLATFORM),tvos)
SOURCES+=../ouzel/audio/coreaudio/CAAudioDevice.mm \
	../ouzel/audio/openal/OALAudioDevice.cpp \
	../ouzel/core/tvos/DisplayLink.mm \
	../ouzel/core/tvos/EngineTVOS.mm \
	../ouzel/core/tvos/main.cpp \
	../ouzel/core/tvos/NativeWindowTVOS.mm \
	../ouzel/core/tvos/SystemTVOS.mm \
	../ouzel/core/tvos/ViewTVOS.mm \
	../ouzel/graphics/metal/tvos/MetalView.m \
	../ouzel/graphics/metal/tvos/MetalRenderDeviceTVOS.mm \
	../ouzel/graphics/metal/MetalBlendState.mm \
	../ouzel/graphics/metal/MetalBuffer.mm \
	../ouzel/graphics/metal/MetalDepthStencilState.mm \
	../ouzel/graphics/metal/MetalRenderDevice.mm \
	../ouzel/graphics/metal/MetalRenderTarget.mm \
	../ouzel/graphics/metal/MetalShader.mm \
	../ouzel/graphics/metal/MetalTexture.mm \
	../ouzel/graphics/opengl/tvos/OGLRenderDeviceTVOS.mm \
	../ouzel/graphics/opengl/tvos/OpenGLView.m \
	../ouzel/input/tvos/GamepadDeviceTVOS.mm \
	../ouzel/input/tvos/InputSystemTVOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
OBJCXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
else ifeq ($(PLATFORM),emscripten)
SOURCES+=../ouzel/audio/openal/OALAudioDevice.cpp \
	../ouzel/core/emscripten/EngineEm.cpp \
	../ouzel/core/emscripten/main.cpp \
	../ouzel/core/emscripten/NativeWindowEm.cpp \
	../ouzel/core/emscripten/SystemEm.cpp \
	../ouzel/graphics/opengl/emscripten/OGLRenderDeviceEm.cpp \
	../ouzel/input/emscripten/GamepadDeviceEm.cpp \
	../ouzel/input/emscripten/InputSystemEm.cpp \
	../ouzel/input/emscripten/MouseDeviceEm.cpp
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
../ouzel/Config.h:
ifeq ($(PLATFORM),windows)
	if not exist ..\ouzel\Config.h copy ..\ouzel\DefaultConfig.h ..\ouzel\Config.h
else
	if [ ! -f ../ouzel/Config.h ]; then \
		cp ../ouzel/DefaultConfig.h ../ouzel/Config.h; \
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
%.o: %.cpp pch/cpp/Prefix.h.gch ../ouzel/Config.h
	$(CXX) -c -x c++ $(CPPFLAGS) $(CXXFLAGS) -include pch/cpp/Prefix.h -MMD -MP $< -o $@

%.o: %.m pch/objc/Prefix.h.gch ../ouzel/Config.h
	$(CC) -c -x objective-c $(CPPFLAGS) $(OBJCFLAGS) -include pch/objc/Prefix.h -MMD -MP $< -o $@

%.o: %.mm pch/objcpp/Prefix.h.gch ../ouzel/Config.h
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