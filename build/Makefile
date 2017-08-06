MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(realpath $(dir $(MAKEFILE_PATH)))
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
AR=emar
endif
CXXFLAGS=-c -std=c++11 -Wall -O2 \
	-I$(ROOT_DIR)/../ouzel \
	-I$(ROOT_DIR)/../external/stb \
	-I$(ROOT_DIR)/../external/json/src \
	-I$(ROOT_DIR)/../external/khronos
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
	$(ROOT_DIR)/../ouzel/audio/empty/AudioEmpty.cpp \
	$(ROOT_DIR)/../ouzel/audio/Audio.cpp \
	$(ROOT_DIR)/../ouzel/audio/Sound.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundData.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundDataVorbis.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundDataWave.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundResource.cpp \
	$(ROOT_DIR)/../ouzel/audio/Stream.cpp \
	$(ROOT_DIR)/../ouzel/audio/StreamVorbis.cpp \
	$(ROOT_DIR)/../ouzel/audio/StreamWave.cpp \
	$(ROOT_DIR)/../ouzel/core/Cache.cpp \
	$(ROOT_DIR)/../ouzel/core/Engine.cpp \
	$(ROOT_DIR)/../ouzel/core/Timer.cpp \
	$(ROOT_DIR)/../ouzel/core/UpdateCallback.cpp \
	$(ROOT_DIR)/../ouzel/core/Window.cpp \
	$(ROOT_DIR)/../ouzel/events/EventDispatcher.cpp \
	$(ROOT_DIR)/../ouzel/events/EventHandler.cpp \
	$(ROOT_DIR)/../ouzel/files/Archive.cpp \
	$(ROOT_DIR)/../ouzel/files/FileSystem.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/BlendStateInterfaceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/BufferInterfaceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/MeshBufferInterfaceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/RendererEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/ShaderInterfaceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/TextureInterfaceEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/BlendStateInterfaceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/BufferInterfaceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/MeshBufferInterfaceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/RendererOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/ShaderInterfaceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/TextureInterfaceOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BlendState.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BlendStateInterface.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Buffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BufferInterface.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ImageData.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ImageDataSTB.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Material.cpp \
	$(ROOT_DIR)/../ouzel/graphics/MeshBuffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/MeshBufferInterface.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Renderer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Shader.cpp \
	$(ROOT_DIR)/../ouzel/graphics/ShaderInterface.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Texture.cpp \
	$(ROOT_DIR)/../ouzel/graphics/TextureInterface.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Vertex.cpp \
	$(ROOT_DIR)/../ouzel/gui/BMFont.cpp \
	$(ROOT_DIR)/../ouzel/gui/Button.cpp \
	$(ROOT_DIR)/../ouzel/gui/CheckBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/ComboBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/EditBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/Label.cpp \
	$(ROOT_DIR)/../ouzel/gui/Menu.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButton.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButtonGroup.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollArea.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/SlideBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/Widget.cpp \
	$(ROOT_DIR)/../ouzel/input/Cursor.cpp \
	$(ROOT_DIR)/../ouzel/input/CursorInterface.cpp \
	$(ROOT_DIR)/../ouzel/input/Gamepad.cpp \
	$(ROOT_DIR)/../ouzel/input/Input.cpp \
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
	$(ROOT_DIR)/../ouzel/math/Rectangle.cpp \
	$(ROOT_DIR)/../ouzel/math/Size2.cpp \
	$(ROOT_DIR)/../ouzel/math/Size3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector2.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector4.cpp \
	$(ROOT_DIR)/../ouzel/scene/Camera.cpp \
	$(ROOT_DIR)/../ouzel/scene/Component.cpp \
	$(ROOT_DIR)/../ouzel/scene/Layer.cpp \
	$(ROOT_DIR)/../ouzel/scene/MeshRenderer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Node.cpp \
	$(ROOT_DIR)/../ouzel/scene/NodeContainer.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleDefinition.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleSystem.cpp \
	$(ROOT_DIR)/../ouzel/scene/Scene.cpp \
	$(ROOT_DIR)/../ouzel/scene/SceneManager.cpp \
	$(ROOT_DIR)/../ouzel/scene/ShapeRenderer.cpp \
	$(ROOT_DIR)/../ouzel/scene/SoundListener.cpp \
	$(ROOT_DIR)/../ouzel/scene/SoundPlayer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Sprite.cpp \
	$(ROOT_DIR)/../ouzel/scene/SpriteFrame.cpp \
	$(ROOT_DIR)/../ouzel/scene/TextRenderer.cpp \
	$(ROOT_DIR)/../ouzel/utils/INI.cpp \
	$(ROOT_DIR)/../ouzel/utils/Log.cpp \
	$(ROOT_DIR)/../ouzel/utils/OBF.cpp \
	$(ROOT_DIR)/../ouzel/utils/Utils.cpp
ifeq ($(platform),windows)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/dsound/AudioDS.cpp \
	$(ROOT_DIR)/../ouzel/audio/dsound/SoundResourceDS.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/EngineWin.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/main.cpp \
	$(ROOT_DIR)/../ouzel/core/windows/WindowWin.cpp \
	$(ROOT_DIR)/../ouzel/files/windows/FileSystemWin.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/BlendStateInterfaceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/BufferInterfaceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/MeshBufferInterfaceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/RendererD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/ShaderInterfaceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/TextureInterfaceD3D11.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/windows/RendererOGLWin.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/GamepadDI.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/GamepadXI.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/InputWin.cpp \
	$(ROOT_DIR)/../ouzel/input/windows/CursorInterfaceWin.cpp
CXXFLAGS+=-DNOMINMAX -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
else ifeq ($(platform),raspbian)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/EngineRasp.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/main.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/WindowRasp.cpp \
	$(ROOT_DIR)/../ouzel/files/raspbian/FileSystemRasp.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/raspbian/RendererOGLRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/GamepadRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/InputRasp.cpp
CXXFLAGS+=-DRASPBIAN -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else ifeq ($(platform),linux)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/alsa/AudioALSA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/EngineLinux.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/main.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/WindowLinux.cpp \
	$(ROOT_DIR)/../ouzel/files/linux/FileSystemLinux.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/linux/RendererOGLLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/CursorInterfaceLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/GamepadLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/InputLinux.cpp
else ifeq ($(platform),macos)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/coreaudio/AudioCA.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)/../ouzel/core/macos/EngineMacOS.mm \
	$(ROOT_DIR)/../ouzel/core/macos/main.cpp \
	$(ROOT_DIR)/../ouzel/core/macos/ViewMacOS.mm \
	$(ROOT_DIR)/../ouzel/core/macos/WindowMacOS.mm \
	$(ROOT_DIR)/../ouzel/files/macos/FileSystemMacOS.cpp \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/MetalView.m \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/RendererMetalMacOS.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BlendStateInterfaceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BufferInterfaceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/MeshBufferInterfaceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RendererMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/ShaderInterfaceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/TextureInterfaceMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/OpenGLView.m \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/RendererOGLMacOS.mm \
	$(ROOT_DIR)/../ouzel/input/macos/CursorInterfaceMacOS.cpp \
	$(ROOT_DIR)/../ouzel/input/macos/GamepadGC.mm \
	$(ROOT_DIR)/../ouzel/input/macos/GamepadIOKit.cpp \
	$(ROOT_DIR)/../ouzel/input/macos/InputMacOS.mm
else ifeq ($(platform),emscripten)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/EngineEm.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/main.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/WindowEm.cpp \
	$(ROOT_DIR)/../ouzel/files/emscripten/FileSystemEm.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/emscripten/RendererOGLEm.cpp \
	$(ROOT_DIR)/../ouzel/input/emscripten/InputEm.cpp \
	$(ROOT_DIR)/../ouzel/input/emscripten/GamepadEm.cpp
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
LIBRARY=libouzel.a
DEPENDENCIES=$(OBJECTS:.o=.d)

.PHONY: all
all: $(LIBRARY)

.PHONY: debug
debug: CXXFLAGS+=-DDEBUG -g
debug: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

-include $(DEPENDENCIES)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) -MMD $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
	-del /f /q "$(ROOT_DIR)\..\ouzel\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\alsa\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\coreaudio\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\dsound\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\empty\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\openal\*.o" \
	"$(ROOT_DIR)\..\ouzel\audio\xaudio2\*.o" \
	"$(ROOT_DIR)\..\ouzel\animators\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\emscripten\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\linux\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\macos\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\raspbian\*.o" \
	"$(ROOT_DIR)\..\ouzel\core\windows\*.o" \
	"$(ROOT_DIR)\..\ouzel\events\*.o" \
	"$(ROOT_DIR)\..\ouzel\files\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\direct3d11\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\empty\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\emscripten\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\linux\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\macos\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\raspbian\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\windows\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\metal\*.o" \
	"$(ROOT_DIR)\..\ouzel\graphics\metal\macos\*.o" \
	"$(ROOT_DIR)\..\ouzel\gui\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\emscripten\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\linux\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\macos\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\raspbian\*.o" \
	"$(ROOT_DIR)\..\ouzel\input\windows\*.o" \
	"$(ROOT_DIR)\..\ouzel\localization\*.o" \
	"$(ROOT_DIR)\..\ouzel\math\*.o" \
	"$(ROOT_DIR)\..\ouzel\scene\*.o" \
	"$(ROOT_DIR)\..\ouzel\utils\*.o" \
	"$(ROOT_DIR)\..\ouzel\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\alsa\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\coreaudio\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\dsound\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\empty\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\openal\*.d" \
	"$(ROOT_DIR)\..\ouzel\audio\xaudio2\*.d" \
	"$(ROOT_DIR)\..\ouzel\animators\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\emscripten\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\linux\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\macos\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\raspbian\*.d" \
	"$(ROOT_DIR)\..\ouzel\core\windows\*.d" \
	"$(ROOT_DIR)\..\ouzel\events\*.d" \
	"$(ROOT_DIR)\..\ouzel\files\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\direct3d11\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\empty\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\emscripten\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\linux\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\macos\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\raspbian\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\opengl\windows\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\metal\*.d" \
	"$(ROOT_DIR)\..\ouzel\graphics\metal\macos\*.d" \
	"$(ROOT_DIR)\..\ouzel\gui\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\emscripten\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\linux\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\macos\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\raspbian\*.d" \
	"$(ROOT_DIR)\..\ouzel\input\windows\*.d" \
	"$(ROOT_DIR)\..\ouzel\localization\*.d" \
	"$(ROOT_DIR)\..\ouzel\math\*.d" \
	"$(ROOT_DIR)\..\ouzel\scene\*.d" \
	"$(ROOT_DIR)\..\ouzel\utils\*.d"
else
	$(RM) $(LIBRARY) \
	$(ROOT_DIR)/../ouzel/*.o \
	$(ROOT_DIR)/../ouzel/audio/*.o \
	$(ROOT_DIR)/../ouzel/audio/alsa/*.o \
	$(ROOT_DIR)/../ouzel/audio/coreaudio/*.o \
	$(ROOT_DIR)/../ouzel/audio/dsound/*.o \
	$(ROOT_DIR)/../ouzel/audio/empty/*.o \
	$(ROOT_DIR)/../ouzel/audio/openal/*.o \
	$(ROOT_DIR)/../ouzel/audio/openal/ios/*.o \
	$(ROOT_DIR)/../ouzel/audio/openal/tvos/*.o \
	$(ROOT_DIR)/../ouzel/audio/xaudio2/*.o \
	$(ROOT_DIR)/../ouzel/animators/*.o \
	$(ROOT_DIR)/../ouzel/core/*.o \
	$(ROOT_DIR)/../ouzel/core/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/core/linux/*.o \
	$(ROOT_DIR)/../ouzel/core/macos/*.o \
	$(ROOT_DIR)/../ouzel/core/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/core/windows/*.o \
	$(ROOT_DIR)/../ouzel/events/*.o \
	$(ROOT_DIR)/../ouzel/files/*.o \
	$(ROOT_DIR)/../ouzel/graphics/*.o \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/*.o \
	$(ROOT_DIR)/../ouzel/graphics/empty/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/linux/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/windows/*.o \
	$(ROOT_DIR)/../ouzel/graphics/metal/*.o \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/*.o \
	$(ROOT_DIR)/../ouzel/gui/*.o \
	$(ROOT_DIR)/../ouzel/input/*.o \
	$(ROOT_DIR)/../ouzel/input/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/input/linux/*.o \
	$(ROOT_DIR)/../ouzel/input/macos/*.o \
	$(ROOT_DIR)/../ouzel/input/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/input/windows/*.o \
	$(ROOT_DIR)/../ouzel/localization/*.o \
	$(ROOT_DIR)/../ouzel/math/*.o \
	$(ROOT_DIR)/../ouzel/scene/*.o \
	$(ROOT_DIR)/../ouzel/utils/*.o \
	$(ROOT_DIR)/../ouzel/*.d \
	$(ROOT_DIR)/../ouzel/audio/*.d \
	$(ROOT_DIR)/../ouzel/audio/alsa/*.d \
	$(ROOT_DIR)/../ouzel/audio/coreaudio/*.d \
	$(ROOT_DIR)/../ouzel/audio/dsound/*.d \
	$(ROOT_DIR)/../ouzel/audio/empty/*.d \
	$(ROOT_DIR)/../ouzel/audio/openal/*.d \
	$(ROOT_DIR)/../ouzel/audio/openal/ios/*.d \
	$(ROOT_DIR)/../ouzel/audio/openal/tvos/*.d \
	$(ROOT_DIR)/../ouzel/audio/xaudio2/*.d \
	$(ROOT_DIR)/../ouzel/animators/*.d \
	$(ROOT_DIR)/../ouzel/core/*.d \
	$(ROOT_DIR)/../ouzel/core/emscripten/*.d \
	$(ROOT_DIR)/../ouzel/core/linux/*.d \
	$(ROOT_DIR)/../ouzel/core/macos/*.d \
	$(ROOT_DIR)/../ouzel/core/raspbian/*.d \
	$(ROOT_DIR)/../ouzel/core/windows/*.d \
	$(ROOT_DIR)/../ouzel/events/*.d \
	$(ROOT_DIR)/../ouzel/files/*.d \
	$(ROOT_DIR)/../ouzel/graphics/*.d \
	$(ROOT_DIR)/../ouzel/graphics/direct3d11/*.d \
	$(ROOT_DIR)/../ouzel/graphics/empty/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/emscripten/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/linux/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/macos/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/raspbian/*.d \
	$(ROOT_DIR)/../ouzel/graphics/opengl/windows/*.d \
	$(ROOT_DIR)/../ouzel/graphics/metal/*.d \
	$(ROOT_DIR)/../ouzel/graphics/metal/macos/*.d \
	$(ROOT_DIR)/../ouzel/gui/*.d \
	$(ROOT_DIR)/../ouzel/input/*.d \
	$(ROOT_DIR)/../ouzel/input/emscripten/*.d \
	$(ROOT_DIR)/../ouzel/input/linux/*.d \
	$(ROOT_DIR)/../ouzel/input/macos/*.d \
	$(ROOT_DIR)/../ouzel/input/raspbian/*.d \
	$(ROOT_DIR)/../ouzel/input/windows/*.d \
	$(ROOT_DIR)/../ouzel/localization/*.d \
	$(ROOT_DIR)/../ouzel/math/*.d \
	$(ROOT_DIR)/../ouzel/scene/*.d \
	$(ROOT_DIR)/../ouzel/utils/*.d
endif