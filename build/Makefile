MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(realpath $(dir $(MAKEFILE_PATH)))
ifeq ($(OS),Windows_NT)
	platform=windows
else
	architecture=$(shell uname -m)

	ifeq ($(shell uname -s),Linux)
		ifneq ($(filter arm%,$(architecture)),)
			platform=raspbian
		else
			platform=linux
		endif
	endif
	ifeq ($(shell uname -s),Darwin)
		platform=macos
	endif
endif
ifeq ($(platform),emscripten)
CC=emcc
CXX=em++
AR=emar
else ifeq ($(platform),ios)
else ifeq ($(platform),tvos)
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 \
	-I"$(ROOT_DIR)/../ouzel" \
	-I"$(ROOT_DIR)/../shaders" \
	-I"$(ROOT_DIR)/../external/stb" \
	-I"$(ROOT_DIR)/../external/khronos"
CFLAGS=-c -Wall -O2 \
	-I"$(ROOT_DIR)/../ouzel"
SOURCES=$(ROOT_DIR)/../ouzel/animators/Animator.cpp \
	$(ROOT_DIR)/../ouzel/animators/Ease.cpp \
	$(ROOT_DIR)/../ouzel/animators/Fade.cpp \
	$(ROOT_DIR)/../ouzel/animators/Move.cpp \
	$(ROOT_DIR)/../ouzel/animators/Parallel.cpp \
	$(ROOT_DIR)/../ouzel/animators/Repeat.cpp \
	$(ROOT_DIR)/../ouzel/animators/Rotate.cpp \
	$(ROOT_DIR)/../ouzel/animators/Scale.cpp \
	$(ROOT_DIR)/../ouzel/animators/Sequence.cpp \
	$(ROOT_DIR)/../ouzel/animators/Shake.cpp \
	$(ROOT_DIR)/../ouzel/assets/Cache.cpp \
	$(ROOT_DIR)/../ouzel/assets/Loader.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderBMF.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderCollada.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderImage.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderMTL.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderOBJ.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderParticleSystem.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderSprite.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderTTF.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderVorbis.cpp \
	$(ROOT_DIR)/../ouzel/assets/LoaderWave.cpp \
	$(ROOT_DIR)/../ouzel/audio/empty/AudioDeviceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/audio/Audio.cpp \
	$(ROOT_DIR)/../ouzel/audio/AudioDevice.cpp \
	$(ROOT_DIR)/../ouzel/audio/Listener.cpp \
	$(ROOT_DIR)/../ouzel/audio/Mixer.cpp \
	$(ROOT_DIR)/../ouzel/audio/Sound.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundData.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundDataVorbis.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundDataWave.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundInput.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundOutput.cpp \
	$(ROOT_DIR)/../ouzel/audio/Stream.cpp \
	$(ROOT_DIR)/../ouzel/audio/StreamVorbis.cpp \
	$(ROOT_DIR)/../ouzel/audio/StreamWave.cpp \
	$(ROOT_DIR)/../ouzel/core/Engine.cpp \
	$(ROOT_DIR)/../ouzel/core/Timer.cpp \
	$(ROOT_DIR)/../ouzel/core/Window.cpp \
	$(ROOT_DIR)/../ouzel/core/WindowResource.cpp \
	$(ROOT_DIR)/../ouzel/events/EventDispatcher.cpp \
	$(ROOT_DIR)/../ouzel/events/EventHandler.cpp \
	$(ROOT_DIR)/../ouzel/files/Archive.cpp \
	$(ROOT_DIR)/../ouzel/files/File.cpp \
	$(ROOT_DIR)/../ouzel/files/FileSystem.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/BlendStateResourceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/BufferResourceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/MeshBufferResourceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/RenderDeviceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/ShaderResourceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/TextureResourceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/BlendStateResourceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/BufferResourceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/MeshBufferResourceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/RenderDeviceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/ShaderResourceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/TextureResourceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BlendState.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BlendStateResource.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Buffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BufferResource.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ImageData.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ImageDataSTB.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Material.cpp \
	$(ROOT_DIR)/../ouzel/graphics/MeshBuffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/MeshBufferResource.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Renderer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/RenderDevice.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Shader.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ShaderResource.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Texture.cpp \
	$(ROOT_DIR)/../ouzel/graphics/TextureResource.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Vertex.cpp \
	$(ROOT_DIR)/../ouzel/gui/BMFont.cpp \
	$(ROOT_DIR)/../ouzel/gui/Button.cpp \
	$(ROOT_DIR)/../ouzel/gui/CheckBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/ComboBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/EditBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/Font.cpp \
	$(ROOT_DIR)/../ouzel/gui/Label.cpp \
	$(ROOT_DIR)/../ouzel/gui/Menu.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButton.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButtonGroup.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollArea.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/SlideBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/TTFont.cpp \
	$(ROOT_DIR)/../ouzel/gui/Widget.cpp \
	$(ROOT_DIR)/../ouzel/input/Cursor.cpp \
	$(ROOT_DIR)/../ouzel/input/CursorResource.cpp \
	$(ROOT_DIR)/../ouzel/input/Gamepad.cpp \
	$(ROOT_DIR)/../ouzel/input/Input.cpp \
	$(ROOT_DIR)/../ouzel/input/InputSystem.cpp \
	$(ROOT_DIR)/../ouzel/localization/Language.cpp \
	$(ROOT_DIR)/../ouzel/localization/Localization.cpp \
	$(ROOT_DIR)/../ouzel/math/Box2.cpp \
	$(ROOT_DIR)/../ouzel/math/Box3.cpp \
	$(ROOT_DIR)/../ouzel/math/Color.cpp \
	$(ROOT_DIR)/../ouzel/math/ConvexVolume.cpp \
	$(ROOT_DIR)/../ouzel/math/MathUtils.cpp \
	$(ROOT_DIR)/../ouzel/math/Matrix3.cpp \
	$(ROOT_DIR)/../ouzel/math/Matrix4.cpp \
	$(ROOT_DIR)/../ouzel/math/Plane.cpp \
	$(ROOT_DIR)/../ouzel/math/Quaternion.cpp \
	$(ROOT_DIR)/../ouzel/math/Rect.cpp \
	$(ROOT_DIR)/../ouzel/math/Size2.cpp \
	$(ROOT_DIR)/../ouzel/math/Size3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector2.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector4.cpp \
	$(ROOT_DIR)/../ouzel/network/Client.cpp \
	$(ROOT_DIR)/../ouzel/network/Network.cpp \
	$(ROOT_DIR)/../ouzel/scene/Actor.cpp \
	$(ROOT_DIR)/../ouzel/scene/ActorContainer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Camera.cpp \
	$(ROOT_DIR)/../ouzel/scene/Component.cpp \
	$(ROOT_DIR)/../ouzel/scene/Layer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Light.cpp \
	$(ROOT_DIR)/../ouzel/scene/ModelData.cpp \
	$(ROOT_DIR)/../ouzel/scene/ModelRenderer.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleSystem.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleSystemData.cpp \
	$(ROOT_DIR)/../ouzel/scene/Scene.cpp \
	$(ROOT_DIR)/../ouzel/scene/SceneManager.cpp \
	$(ROOT_DIR)/../ouzel/scene/ShapeRenderer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Sprite.cpp \
	$(ROOT_DIR)/../ouzel/scene/SpriteData.cpp \
	$(ROOT_DIR)/../ouzel/scene/TextRenderer.cpp \
	$(ROOT_DIR)/../ouzel/scene/UpdateCallback.cpp \
	$(ROOT_DIR)/../ouzel/thread/Condition.cpp \
	$(ROOT_DIR)/../ouzel/thread/Mutex.cpp \
	$(ROOT_DIR)/../ouzel/thread/Thread.cpp \
	$(ROOT_DIR)/../ouzel/utils/INI.cpp \
	$(ROOT_DIR)/../ouzel/utils/JSON.cpp \
	$(ROOT_DIR)/../ouzel/utils/Log.cpp \
	$(ROOT_DIR)/../ouzel/utils/OBF.cpp \
	$(ROOT_DIR)/../ouzel/utils/Utils.cpp \
	$(ROOT_DIR)/../ouzel/utils/XML.cpp
ifeq ($(platform),windows)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/dsound/AudioDeviceDS.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/EngineWin.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/main.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/WindowResourceWin.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/BlendStateResourceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/BufferResourceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/MeshBufferResourceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/RenderDeviceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/ShaderResourceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/TextureResourceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/windows/RenderDeviceOGLWin.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/GamepadDI.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/GamepadXI.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/InputWin.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/CursorResourceWin.cpp
CXXFLAGS+=-DNOMINMAX -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(platform),raspbian)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/alsa/AudioDeviceALSA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/EngineRasp.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/main.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/WindowResourceRasp.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/raspbian/RenderDeviceOGLRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/EventDevice.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/GamepadRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/InputRasp.cpp
CXXFLAGS+=-I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else ifeq ($(platform),linux)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/alsa/AudioDeviceALSA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/EngineLinux.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/main.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/WindowResourceLinux.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/linux/RenderDeviceOGLLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/CursorResourceLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/EventDevice.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/GamepadLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/InputLinux.cpp
else ifeq ($(platform),macos)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/coreaudio/AudioDeviceCA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/macos/EngineMacOS.mm \
	$(ROOT_DIR)/../ouzel/core/macos/main.cpp \
	$(ROOT_DIR)/../ouzel/core/macos/ViewMacOS.mm \
	$(ROOT_DIR)/../ouzel/core/macos/WindowResourceMacOS.mm \
	$(ROOT_DIR)/../ouzel/files/apple/FileSystemApple.cpp \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/MetalView.m \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/RenderDeviceMetalMacOS.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BlendStateResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/MeshBufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RenderDeviceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/ShaderResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/TextureResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/OpenGLView.m \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/RenderDeviceOGLMacOS.mm \
	$(ROOT_DIR)/../ouzel/input/macos/CursorResourceMacOS.cpp \
	$(ROOT_DIR)/../ouzel/input/macos/GamepadGC.mm \
	$(ROOT_DIR)/../ouzel/input/macos/GamepadIOKit.cpp \
	$(ROOT_DIR)/../ouzel/input/macos/InputMacOS.mm
else ifeq ($(platform),ios)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/coreaudio/AudioDeviceCA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/ios/DisplayLinkHandler.mm \
	$(ROOT_DIR)/../ouzel/core/ios/EngineIOS.mm \
	$(ROOT_DIR)/../ouzel/core/ios/main.cpp \
	$(ROOT_DIR)/../ouzel/core/ios/ViewIOS.mm \
	$(ROOT_DIR)/../ouzel/core/ios/WindowResourceIOS.mm \
	$(ROOT_DIR)/../ouzel/files/apple/FileSystemApple.cpp \
	$(ROOT_DIR)/../ouzel/graphics/metal/ios/MetalView.m \
	$(ROOT_DIR)/../ouzel/graphics/metal/ios/RenderDeviceMetalIOS.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BlendStateResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/MeshBufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RenderDeviceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/ShaderResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/TextureResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/opengl/ios/OpenGLView.m \
	$(ROOT_DIR)/../ouzel/graphics/opengl/ios/RenderDeviceOGLIOS.mm \
	$(ROOT_DIR)/../ouzel/input/ios/GamepadIOS.mm \
	$(ROOT_DIR)/../ouzel/input/ios/InputIOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk iphoneos --show-sdk-path) -miphoneos-version-min=8.0
else ifeq ($(platform),tvos)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/coreaudio/AudioDeviceCA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/tvos/DisplayLinkHandler.mm \
	$(ROOT_DIR)/../ouzel/core/tvos/EngineTVOS.mm \
	$(ROOT_DIR)/../ouzel/core/tvos/main.cpp \
	$(ROOT_DIR)/../ouzel/core/tvos/ViewTVOS.mm \
	$(ROOT_DIR)/../ouzel/core/tvos/WindowResourceTVOS.mm \
	$(ROOT_DIR)/../ouzel/files/apple/FileSystemApple.cpp \
	$(ROOT_DIR)/../ouzel/graphics/metal/tvos/MetalView.m \
	$(ROOT_DIR)/../ouzel/graphics/metal/tvos/RenderDeviceMetalTVOS.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BlendStateResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/MeshBufferResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RenderDeviceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/ShaderResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/TextureResourceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/opengl/tvos/OpenGLView.m \
	$(ROOT_DIR)/../ouzel/graphics/opengl/tvos/RenderDeviceOGLTVOS.mm \
	$(ROOT_DIR)/../ouzel/input/tvos/GamepadTVOS.mm \
	$(ROOT_DIR)/../ouzel/input/tvos/InputTVOS.mm
CFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
CXXFLAGS+=-arch arm64 -isysroot $(shell xcrun --sdk appletvos --show-sdk-path) -mtvos-version-min=9.0
else ifeq ($(platform),emscripten)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/openal/AudioDeviceAL.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/EngineEm.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/main.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/WindowResourceEm.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/emscripten/RenderDeviceOGLEm.cpp \
	$(ROOT_DIR)/../ouzel/input/emscripten/InputEm.cpp \
	$(ROOT_DIR)/../ouzel/input/emscripten/GamepadEm.cpp
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
LIBRARY=libouzel.a
DEPENDENCIES=$(OBJECTS:.o=.d)

.PHONY: all
ifeq ($(debug),1)
all: CXXFLAGS+=-DDEBUG -g
all: CFLAGS+=-DDEBUG -g
endif
all: config
all: $(ROOT_DIR)/$(LIBRARY)

$(ROOT_DIR)/$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

-include $(DEPENDENCIES)

.PHONY: config
config:
ifeq ($(platform),windows)
	if not exist $(ROOT_DIR)\..\ouzel\Config.h copy $(ROOT_DIR)\..\ouzel\DefaultConfig.h $(ROOT_DIR)\..\ouzel\Config.h
else
	if [ ! -f $(ROOT_DIR)/../ouzel/Config.h ]; then \
		cp $(ROOT_DIR)/../ouzel/DefaultConfig.h $(ROOT_DIR)/../ouzel/Config.h; \
	fi
endif

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) -MMD -MP $< -o $@

%.o: %.m
	$(CC) -fno-objc-arc $(CFLAGS) -MMD -MP $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
	-del /f /q "$(ROOT_DIR)\$(LIBRARY)"
	-del /s "$(ROOT_DIR)\..\*.o"
	-del /s "$(ROOT_DIR)\..\*.d"
else
	$(RM) $(ROOT_DIR)/$(LIBRARY)
	find $(ROOT_DIR)/.. -name '*.o' -type f -delete
	find $(ROOT_DIR)/.. -name '*.d' -type f -delete
endif