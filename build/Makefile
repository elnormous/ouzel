ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
ifeq ($(OS),Windows_NT)
    platform=windows
else
    UNAME := $(shell uname -s)
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
CXXFLAGS=-c -std=c++11 -Wall -O2 -I$(ROOT_DIR)/../ouzel -I$(ROOT_DIR)/../external/stb -I$(ROOT_DIR)/../external/rapidjson/include
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
	$(ROOT_DIR)/../ouzel/audio/empty/SoundEmpty.cpp \
	$(ROOT_DIR)/../ouzel/audio/empty/SoundDataEmpty.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/AudioAL.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/SoundAL.cpp \
	$(ROOT_DIR)/../ouzel/audio/openal/SoundDataAL.cpp \
	$(ROOT_DIR)/../ouzel/audio/Audio.cpp \
	$(ROOT_DIR)/../ouzel/audio/Sound.cpp \
	$(ROOT_DIR)/../ouzel/audio/SoundData.cpp \
	$(ROOT_DIR)/../ouzel/core/Application.cpp \
	$(ROOT_DIR)/../ouzel/core/Cache.cpp \
	$(ROOT_DIR)/../ouzel/core/Engine.cpp \
	$(ROOT_DIR)/../ouzel/core/UpdateCallback.cpp \
	$(ROOT_DIR)/../ouzel/core/Window.cpp \
	$(ROOT_DIR)/../ouzel/events/EventDispatcher.cpp \
	$(ROOT_DIR)/../ouzel/events/EventHandler.cpp \
	$(ROOT_DIR)/../ouzel/files/FileSystem.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/BlendStateEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/IndexBufferEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/MeshBufferEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/RendererEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/RenderTargetEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/ShaderEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/TextureEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/empty/VertexBufferEmpty.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/BlendStateOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/IndexBufferOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/MeshBufferOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/RendererOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/RenderTargetOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/ShaderOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/TextureOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/opengl/VertexBufferOGL.cpp \
	$(ROOT_DIR)/../ouzel/graphics/BlendState.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Color.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Image.cpp \
	$(ROOT_DIR)/../ouzel/graphics/IndexBuffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/MeshBuffer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Renderer.cpp \
	$(ROOT_DIR)/../ouzel/graphics/RenderTarget.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Shader.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Texture.cpp \
	$(ROOT_DIR)/../ouzel/graphics/Vertex.cpp \
	$(ROOT_DIR)/../ouzel/graphics/VertexBuffer.cpp \
	$(ROOT_DIR)/../ouzel/gui/BMFont.cpp \
	$(ROOT_DIR)/../ouzel/gui/Button.cpp \
	$(ROOT_DIR)/../ouzel/gui/CheckBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/ComboBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/EditBox.cpp \
	$(ROOT_DIR)/../ouzel/gui/Label.cpp \
	$(ROOT_DIR)/../ouzel/gui/Menu.cpp \
	$(ROOT_DIR)/../ouzel/gui/Popup.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButton.cpp \
	$(ROOT_DIR)/../ouzel/gui/RadioButtonGroup.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollArea.cpp \
	$(ROOT_DIR)/../ouzel/gui/ScrollBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/SlideBar.cpp \
	$(ROOT_DIR)/../ouzel/gui/Widget.cpp \
	$(ROOT_DIR)/../ouzel/input/Gamepad.cpp \
	$(ROOT_DIR)/../ouzel/input/Input.cpp \
	$(ROOT_DIR)/../ouzel/localization/Language.cpp \
	$(ROOT_DIR)/../ouzel/localization/Localization.cpp \
	$(ROOT_DIR)/../ouzel/math/AABB2.cpp \
	$(ROOT_DIR)/../ouzel/math/MathUtils.cpp \
	$(ROOT_DIR)/../ouzel/math/Matrix3.cpp \
	$(ROOT_DIR)/../ouzel/math/Matrix4.cpp \
	$(ROOT_DIR)/../ouzel/math/Rectangle.cpp \
	$(ROOT_DIR)/../ouzel/math/Size2.cpp \
	$(ROOT_DIR)/../ouzel/math/Size3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector2.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector3.cpp \
	$(ROOT_DIR)/../ouzel/math/Vector4.cpp \
	$(ROOT_DIR)/../ouzel/scene/Camera.cpp \
	$(ROOT_DIR)/../ouzel/scene/Component.cpp \
	$(ROOT_DIR)/../ouzel/scene/Layer.cpp \
	$(ROOT_DIR)/../ouzel/scene/Node.cpp \
	$(ROOT_DIR)/../ouzel/scene/NodeContainer.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleDefinition.cpp \
	$(ROOT_DIR)/../ouzel/scene/ParticleSystem.cpp \
	$(ROOT_DIR)/../ouzel/scene/Scene.cpp \
	$(ROOT_DIR)/../ouzel/scene/SceneManager.cpp \
	$(ROOT_DIR)/../ouzel/scene/ShapeDrawable.cpp \
	$(ROOT_DIR)/../ouzel/scene/Sprite.cpp \
	$(ROOT_DIR)/../ouzel/scene/SpriteFrame.cpp \
	$(ROOT_DIR)/../ouzel/scene/TextDrawable.cpp \
	$(ROOT_DIR)/../ouzel/utils/Log.cpp \
	$(ROOT_DIR)/../ouzel/utils/Utils.cpp
ifeq ($(platform),raspbian)
SOURCES+=$(ROOT_DIR)/../ouzel/core/raspbian/ApplicationRasp.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/main.cpp \
	$(ROOT_DIR)/../ouzel/core/raspbian/WindowRasp.cpp \
	$(ROOT_DIR)/../ouzel/graphics/raspbian/RendererOGLRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/GamepadRasp.cpp \
	$(ROOT_DIR)/../ouzel/input/raspbian/InputRasp.cpp
CXXFLAGS+=-DRASPBIAN -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else ifeq ($(platform),linux)
SOURCES+=$(ROOT_DIR)/../ouzel/core/linux/ApplicationLinux.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/main.cpp \
	$(ROOT_DIR)/../ouzel/core/linux/WindowLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/GamepadLinux.cpp \
	$(ROOT_DIR)/../ouzel/input/linux/InputLinux.cpp \
	$(ROOT_DIR)/../ouzel/graphics/linux/RendererOGLLinux.cpp
else ifeq ($(platform),macos)
SOURCES+=$(ROOT_DIR)/../ouzel/audio/apple/AudioALApple.mm \
	$(ROOT_DIR)/../ouzel/core/macos/AppDelegate.mm \
	$(ROOT_DIR)/../ouzel/core/macos/ApplicationMacOS.mm \
	$(ROOT_DIR)/../ouzel/core/macos/main.cpp \
	$(ROOT_DIR)/../ouzel/core/macos/WindowMacOS.mm \
	$(ROOT_DIR)/../ouzel/input/apple/GamepadApple.mm \
	$(ROOT_DIR)/../ouzel/input/apple/InputApple.mm \
	$(ROOT_DIR)/../ouzel/graphics/macos/MetalView.mm \
	$(ROOT_DIR)/../ouzel/graphics/macos/OpenGLView.mm \
	$(ROOT_DIR)/../ouzel/graphics/macos/RendererOGLMacOS.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/BlendStateMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/IndexBufferMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/MeshBufferMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RendererMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/RenderTargetMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/ShaderMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/TextureMetal.mm \
	$(ROOT_DIR)/../ouzel/graphics/metal/VertexBufferMetal.mm
else ifeq ($(platform),emscripten)
SOURCES+=$(ROOT_DIR)/../ouzel/core/emscripten/ApplicationEm.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/main.cpp \
	$(ROOT_DIR)/../ouzel/core/emscripten/WindowEm.cpp \
	$(ROOT_DIR)/../ouzel/graphics/emscripten/RendererOGLEm.cpp \
	$(ROOT_DIR)/../ouzel/input/emscripten/InputEm.cpp
endif
TMP=$(SOURCES:.cpp=.o)
OBJECTS=$(TMP:.mm=.o)
LIBRARY=libouzel.a

all: $(SOURCES) $(LIBRARY)

debug: CXXFLAGS+=-DDEBUG -g
debug: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) rs $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.mm
	$(CXX) -fno-objc-arc $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(LIBRARY) \
	$(ROOT_DIR)/../ouzel/audio/*.o \
	$(ROOT_DIR)/../ouzel/audio/apple/*.o \
	$(ROOT_DIR)/../ouzel/audio/openal/*.o \
	$(ROOT_DIR)/../ouzel/audio/empty/*.o \
	$(ROOT_DIR)/../ouzel/animators/*.o \
	$(ROOT_DIR)/../ouzel/core/*.o \
	$(ROOT_DIR)/../ouzel/core/apple/*.o \
	$(ROOT_DIR)/../ouzel/core/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/core/linux/*.o \
	$(ROOT_DIR)/../ouzel/core/macos/*.o \
	$(ROOT_DIR)/../ouzel/core/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/events/*.o \
	$(ROOT_DIR)/../ouzel/files/*.o \
	$(ROOT_DIR)/../ouzel/graphics/*.o \
	$(ROOT_DIR)/../ouzel/graphics/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/graphics/linux/*.o \
	$(ROOT_DIR)/../ouzel/graphics/macos/*.o \
	$(ROOT_DIR)/../ouzel/graphics/metal/*.o \
	$(ROOT_DIR)/../ouzel/graphics/opengl/*.o \
	$(ROOT_DIR)/../ouzel/graphics/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/graphics/empty/*.o \
	$(ROOT_DIR)/../ouzel/gui/*.o \
	$(ROOT_DIR)/../ouzel/input/*.o \
	$(ROOT_DIR)/../ouzel/input/apple/*.o \
	$(ROOT_DIR)/../ouzel/input/emscripten/*.o \
	$(ROOT_DIR)/../ouzel/input/linux/*.o \
	$(ROOT_DIR)/../ouzel/input/raspbian/*.o \
	$(ROOT_DIR)/../ouzel/localization/*.o \
	$(ROOT_DIR)/../ouzel/math/*.o \
	$(ROOT_DIR)/../ouzel/scene/*.o \
	$(ROOT_DIR)/../ouzel/utils/*.o \
	$(ROOT_DIR)/../ouzel/*.o
