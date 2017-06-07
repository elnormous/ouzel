MAKEFILE_PATH:=$(abspath $(lastword $(MAKEFILE_LIST)))
ROOT_DIR:=$(dir MAKEFILE_PATH)
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
	-I$(ROOT_DIR)../ouzel \
	-I$(ROOT_DIR)../external/stb \
	-I$(ROOT_DIR)../external/rapidjson/include \
	-I$(ROOT_DIR)../external/khronos
SOURCES=$(ROOT_DIR)../ouzel/animators/Animator.cpp \
	$(ROOT_DIR)../ouzel/animators/Ease.cpp \
	$(ROOT_DIR)../ouzel/animators/Fade.cpp \
	$(ROOT_DIR)../ouzel/animators/Move.cpp \
	$(ROOT_DIR)../ouzel/animators/Parallel.cpp \
	$(ROOT_DIR)../ouzel/animators/Repeat.cpp \
	$(ROOT_DIR)../ouzel/animators/Rotate.cpp \
	$(ROOT_DIR)../ouzel/animators/Scale.cpp \
	$(ROOT_DIR)../ouzel/animators/Sequence.cpp \
	$(ROOT_DIR)../ouzel/animators/Shake.cpp \
	$(ROOT_DIR)../ouzel/audio/empty/AudioEmpty.cpp \
	$(ROOT_DIR)../ouzel/audio/empty/SoundEmpty.cpp \
	$(ROOT_DIR)../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)../ouzel/audio/openal/SoundAL.cpp \
	$(ROOT_DIR)../ouzel/audio/Audio.cpp \
	$(ROOT_DIR)../ouzel/audio/Sound.cpp \
	$(ROOT_DIR)../ouzel/audio/SoundData.cpp \
	$(ROOT_DIR)../ouzel/audio/SoundDataWave.cpp \
	$(ROOT_DIR)../ouzel/audio/SoundResource.cpp \
	$(ROOT_DIR)../ouzel/core/Application.cpp \
	$(ROOT_DIR)../ouzel/core/Cache.cpp \
	$(ROOT_DIR)../ouzel/core/Engine.cpp \
	$(ROOT_DIR)../ouzel/core/Timer.cpp \
	$(ROOT_DIR)../ouzel/core/UpdateCallback.cpp \
	$(ROOT_DIR)../ouzel/core/Window.cpp \
	$(ROOT_DIR)../ouzel/events/EventDispatcher.cpp \
	$(ROOT_DIR)../ouzel/events/EventHandler.cpp \
	$(ROOT_DIR)../ouzel/files/Archive.cpp \
	$(ROOT_DIR)../ouzel/files/FileSystem.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/BlendStateResourceEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/BufferResourceEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/MeshBufferResourceEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/RendererEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/ShaderResourceEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/empty/TextureResourceEmpty.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/BlendStateResourceOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/BufferResourceOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/MeshBufferResourceOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/RendererOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/ShaderResourceOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/TextureResourceOGL.cpp \
	$(ROOT_DIR)../ouzel/graphics/BlendState.cpp \
	$(ROOT_DIR)../ouzel/graphics/BlendStateResource.cpp \
	$(ROOT_DIR)../ouzel/graphics/Buffer.cpp \
	$(ROOT_DIR)../ouzel/graphics/BufferResource.cpp \
	$(ROOT_DIR)../ouzel/graphics/ImageData.cpp \
	$(ROOT_DIR)../ouzel/graphics/ImageDataSTB.cpp \
	$(ROOT_DIR)../ouzel/graphics/Material.cpp \
	$(ROOT_DIR)../ouzel/graphics/MeshBuffer.cpp \
	$(ROOT_DIR)../ouzel/graphics/MeshBufferResource.cpp \
	$(ROOT_DIR)../ouzel/graphics/Renderer.cpp \
	$(ROOT_DIR)../ouzel/graphics/Shader.cpp \
	$(ROOT_DIR)../ouzel/graphics/ShaderResource.cpp \
	$(ROOT_DIR)../ouzel/graphics/Texture.cpp \
	$(ROOT_DIR)../ouzel/graphics/TextureResource.cpp \
	$(ROOT_DIR)../ouzel/graphics/Vertex.cpp \
	$(ROOT_DIR)../ouzel/gui/BMFont.cpp \
	$(ROOT_DIR)../ouzel/gui/Button.cpp \
	$(ROOT_DIR)../ouzel/gui/CheckBox.cpp \
	$(ROOT_DIR)../ouzel/gui/ComboBox.cpp \
	$(ROOT_DIR)../ouzel/gui/EditBox.cpp \
	$(ROOT_DIR)../ouzel/gui/Label.cpp \
	$(ROOT_DIR)../ouzel/gui/Menu.cpp \
	$(ROOT_DIR)../ouzel/gui/RadioButton.cpp \
	$(ROOT_DIR)../ouzel/gui/RadioButtonGroup.cpp \
	$(ROOT_DIR)../ouzel/gui/ScrollArea.cpp \
	$(ROOT_DIR)../ouzel/gui/ScrollBar.cpp \
	$(ROOT_DIR)../ouzel/gui/SlideBar.cpp \
	$(ROOT_DIR)../ouzel/gui/Widget.cpp \
	$(ROOT_DIR)../ouzel/input/Cursor.cpp \
	$(ROOT_DIR)../ouzel/input/CursorResource.cpp \
	$(ROOT_DIR)../ouzel/input/Gamepad.cpp \
	$(ROOT_DIR)../ouzel/input/Input.cpp \
	$(ROOT_DIR)../ouzel/localization/Language.cpp \
	$(ROOT_DIR)../ouzel/localization/Localization.cpp \
	$(ROOT_DIR)../ouzel/math/Box2.cpp \
	$(ROOT_DIR)../ouzel/math/Box3.cpp \
	$(ROOT_DIR)../ouzel/math/Color.cpp \
	$(ROOT_DIR)../ouzel/math/ConvexVolume.cpp \
	$(ROOT_DIR)../ouzel/math/MathUtils.cpp \
	$(ROOT_DIR)../ouzel/math/Matrix3.cpp \
	$(ROOT_DIR)../ouzel/math/Matrix4.cpp \
	$(ROOT_DIR)../ouzel/math/Plane.cpp \
	$(ROOT_DIR)../ouzel/math/Quaternion.cpp \
	$(ROOT_DIR)../ouzel/math/Rectangle.cpp \
	$(ROOT_DIR)../ouzel/math/Size2.cpp \
	$(ROOT_DIR)../ouzel/math/Size3.cpp \
	$(ROOT_DIR)../ouzel/math/Vector2.cpp \
	$(ROOT_DIR)../ouzel/math/Vector3.cpp \
	$(ROOT_DIR)../ouzel/math/Vector4.cpp \
	$(ROOT_DIR)../ouzel/scene/Camera.cpp \
	$(ROOT_DIR)../ouzel/scene/Component.cpp \
	$(ROOT_DIR)../ouzel/scene/Layer.cpp \
	$(ROOT_DIR)../ouzel/scene/MeshRenderer.cpp \
	$(ROOT_DIR)../ouzel/scene/Node.cpp \
	$(ROOT_DIR)../ouzel/scene/NodeContainer.cpp \
	$(ROOT_DIR)../ouzel/scene/ParticleDefinition.cpp \
	$(ROOT_DIR)../ouzel/scene/ParticleSystem.cpp \
	$(ROOT_DIR)../ouzel/scene/Scene.cpp \
	$(ROOT_DIR)../ouzel/scene/SceneManager.cpp \
	$(ROOT_DIR)../ouzel/scene/ShapeRenderer.cpp \
	$(ROOT_DIR)../ouzel/scene/SoundListener.cpp \
	$(ROOT_DIR)../ouzel/scene/SoundPlayer.cpp \
	$(ROOT_DIR)../ouzel/scene/Sprite.cpp \
	$(ROOT_DIR)../ouzel/scene/SpriteFrame.cpp \
	$(ROOT_DIR)../ouzel/scene/TextRenderer.cpp \
	$(ROOT_DIR)../ouzel/utils/Log.cpp \
	$(ROOT_DIR)../ouzel/utils/OBF.cpp \
	$(ROOT_DIR)../ouzel/utils/Utils.cpp
ifeq ($(platform),windows)
SOURCES+=$(ROOT_DIR)../ouzel/core/windows/ApplicationWin.cpp \
	$(ROOT_DIR)../ouzel/core/windows/main.cpp \
	$(ROOT_DIR)../ouzel/core/windows/WindowWin.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/BlendStateResourceD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/BufferResourceD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/MeshBufferResourceD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/RendererD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/ShaderResourceD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/direct3d11/TextureResourceD3D11.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/windows/RendererOGLWin.cpp \
	$(ROOT_DIR)../ouzel/input/windows/GamepadDI.cpp \
	$(ROOT_DIR)../ouzel/input/windows/GamepadXI.cpp \
	$(ROOT_DIR)../ouzel/input/windows/InputWin.cpp \
	$(ROOT_DIR)../ouzel/input/windows/CursorResourceWin.cpp
else ifeq ($(platform),raspbian)
SOURCES+=$(ROOT_DIR)../ouzel/core/raspbian/ApplicationRasp.cpp \
	$(ROOT_DIR)../ouzel/core/raspbian/main.cpp \
	$(ROOT_DIR)../ouzel/core/raspbian/WindowRasp.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/raspbian/RendererOGLRasp.cpp \
	$(ROOT_DIR)../ouzel/input/raspbian/GamepadRasp.cpp \
	$(ROOT_DIR)../ouzel/input/raspbian/InputRasp.cpp
CXXFLAGS+=-DRASPBIAN -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else ifeq ($(platform),linux)
SOURCES+=$(ROOT_DIR)../ouzel/core/linux/ApplicationLinux.cpp \
	$(ROOT_DIR)../ouzel/core/linux/main.cpp \
	$(ROOT_DIR)../ouzel/core/linux/WindowLinux.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/linux/RendererOGLLinux.cpp \
	$(ROOT_DIR)../ouzel/input/linux/CursorResourceLinux.cpp \
	$(ROOT_DIR)../ouzel/input/linux/GamepadLinux.cpp \
	$(ROOT_DIR)../ouzel/input/linux/InputLinux.cpp
else ifeq ($(platform),macos)
SOURCES+=$(ROOT_DIR)../ouzel/core/macos/ApplicationMacOS.mm \
	$(ROOT_DIR)../ouzel/core/macos/main.cpp \
	$(ROOT_DIR)../ouzel/core/macos/WindowMacOS.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/macos/MetalView.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/macos/RendererMetalMacOS.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/BlendStateResourceMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/BufferResourceMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/MeshBufferResourceMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/RendererMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/ShaderResourceMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/metal/TextureResourceMetal.mm \
	$(ROOT_DIR)../ouzel/graphics/opengl/macos/OpenGLView.mm \
	$(ROOT_DIR)../ouzel/graphics/opengl/macos/RendererOGLMacOS.mm \
	$(ROOT_DIR)../ouzel/input/macos/CursorResourceMacOS.cpp \
	$(ROOT_DIR)../ouzel/input/macos/GamepadMacOS.mm \
	$(ROOT_DIR)../ouzel/input/macos/InputMacOS.mm
else ifeq ($(platform),emscripten)
SOURCES+=$(ROOT_DIR)../ouzel/core/emscripten/ApplicationEm.cpp \
	$(ROOT_DIR)../ouzel/core/emscripten/main.cpp \
	$(ROOT_DIR)../ouzel/core/emscripten/WindowEm.cpp \
	$(ROOT_DIR)../ouzel/graphics/opengl/emscripten/RendererOGLEm.cpp \
	$(ROOT_DIR)../ouzel/input/emscripten/InputEm.cpp \
	$(ROOT_DIR)../ouzel/input/emscripten/GamepadEm.cpp
endif
BASE_NAMES=$(basename $(SOURCES))
OBJECTS=$(BASE_NAMES:=.o)
LIBRARY=libouzel.a

.PHONY: all
all: $(LIBRARY)

.PHONY: debug
debug: CXXFLAGS+=-DDEBUG -g
debug: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
ifeq ($(platform),windows)
else
	rm -rf $(LIBRARY) \
	$(ROOT_DIR)../ouzel/audio/*.o \
	$(ROOT_DIR)../ouzel/audio/openal/*.o \
	$(ROOT_DIR)../ouzel/audio/openal/apple/*.o \
	$(ROOT_DIR)../ouzel/audio/empty/*.o \
	$(ROOT_DIR)../ouzel/animators/*.o \
	$(ROOT_DIR)../ouzel/core/*.o \
	$(ROOT_DIR)../ouzel/core/apple/*.o \
	$(ROOT_DIR)../ouzel/core/emscripten/*.o \
	$(ROOT_DIR)../ouzel/core/linux/*.o \
	$(ROOT_DIR)../ouzel/core/macos/*.o \
	$(ROOT_DIR)../ouzel/core/raspbian/*.o \
	$(ROOT_DIR)../ouzel/events/*.o \
	$(ROOT_DIR)../ouzel/files/*.o \
	$(ROOT_DIR)../ouzel/graphics/*.o \
	$(ROOT_DIR)../ouzel/graphics/empty/*.o \
	$(ROOT_DIR)../ouzel/graphics/opengl/*.o \
	$(ROOT_DIR)../ouzel/graphics/opengl/emscripten/*.o \
	$(ROOT_DIR)../ouzel/graphics/opengl/linux/*.o \
	$(ROOT_DIR)../ouzel/graphics/opengl/macos/*.o \
	$(ROOT_DIR)../ouzel/graphics/opengl/raspbian/*.o \
	$(ROOT_DIR)../ouzel/graphics/metal/*.o \
	$(ROOT_DIR)../ouzel/graphics/metal/macos/*.o \
	$(ROOT_DIR)../ouzel/gui/*.o \
	$(ROOT_DIR)../ouzel/input/*.o \
	$(ROOT_DIR)../ouzel/input/apple/*.o \
	$(ROOT_DIR)../ouzel/input/emscripten/*.o \
	$(ROOT_DIR)../ouzel/input/linux/*.o \
	$(ROOT_DIR)../ouzel/input/raspbian/*.o \
	$(ROOT_DIR)../ouzel/localization/*.o \
	$(ROOT_DIR)../ouzel/math/*.o \
	$(ROOT_DIR)../ouzel/scene/*.o \
	$(ROOT_DIR)../ouzel/utils/*.o \
	$(ROOT_DIR)../ouzel/*.o
endif