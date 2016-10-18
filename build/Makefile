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
CXXFLAGS=-c -std=c++11 -Wall -O2 -I../ouzel -I../external/stb -I../external/rapidjson/include
SOURCES=../ouzel/animators/Animator.cpp \
	../ouzel/animators/Ease.cpp \
	../ouzel/animators/Fade.cpp \
	../ouzel/animators/Move.cpp \
	../ouzel/animators/Parallel.cpp \
	../ouzel/animators/Repeat.cpp \
	../ouzel/animators/Rotate.cpp \
	../ouzel/animators/Scale.cpp \
	../ouzel/animators/Sequence.cpp \
	../ouzel/animators/Shake.cpp \
	../ouzel/audio/empty/AudioEmpty.cpp \
	../ouzel/audio/empty/SoundEmpty.cpp \
	../ouzel/audio/empty/SoundDataEmpty.cpp \
	../ouzel/audio/openal/AudioAL.cpp \
	../ouzel/audio/openal/SoundAL.cpp \
	../ouzel/audio/openal/SoundDataAL.cpp \
	../ouzel/audio/Audio.cpp \
	../ouzel/audio/Sound.cpp \
	../ouzel/audio/SoundData.cpp \
	../ouzel/core/Application.cpp \
	../ouzel/core/Cache.cpp \
	../ouzel/core/Engine.cpp \
	../ouzel/core/UpdateCallback.cpp \
	../ouzel/core/Window.cpp \
	../ouzel/events/EventDispatcher.cpp \
	../ouzel/events/EventHandler.cpp \
	../ouzel/files/FileSystem.cpp \
	../ouzel/graphics/empty/BlendStateEmpty.cpp \
	../ouzel/graphics/empty/IndexBufferEmpty.cpp \
	../ouzel/graphics/empty/MeshBufferEmpty.cpp \
	../ouzel/graphics/empty/RendererEmpty.cpp \
	../ouzel/graphics/empty/RenderTargetEmpty.cpp \
	../ouzel/graphics/empty/ShaderEmpty.cpp \
	../ouzel/graphics/empty/TextureEmpty.cpp \
	../ouzel/graphics/empty/VertexBufferEmpty.cpp \
	../ouzel/graphics/opengl/BlendStateOGL.cpp \
	../ouzel/graphics/opengl/IndexBufferOGL.cpp \
	../ouzel/graphics/opengl/MeshBufferOGL.cpp \
	../ouzel/graphics/opengl/RendererOGL.cpp \
	../ouzel/graphics/opengl/RenderTargetOGL.cpp \
	../ouzel/graphics/opengl/ShaderOGL.cpp \
	../ouzel/graphics/opengl/TextureOGL.cpp \
	../ouzel/graphics/opengl/VertexBufferOGL.cpp \
	../ouzel/graphics/BlendState.cpp \
	../ouzel/graphics/Color.cpp \
	../ouzel/graphics/Image.cpp \
	../ouzel/graphics/IndexBuffer.cpp \
	../ouzel/graphics/MeshBuffer.cpp \
	../ouzel/graphics/Renderer.cpp \
	../ouzel/graphics/RenderTarget.cpp \
	../ouzel/graphics/Shader.cpp \
	../ouzel/graphics/Texture.cpp \
	../ouzel/graphics/Vertex.cpp \
	../ouzel/graphics/VertexBuffer.cpp \
	../ouzel/gui/BMFont.cpp \
	../ouzel/gui/Button.cpp \
	../ouzel/gui/CheckBox.cpp \
	../ouzel/gui/ComboBox.cpp \
	../ouzel/gui/EditBox.cpp \
	../ouzel/gui/Label.cpp \
	../ouzel/gui/Menu.cpp \
	../ouzel/gui/Popup.cpp \
	../ouzel/gui/RadioButton.cpp \
	../ouzel/gui/RadioButtonGroup.cpp \
	../ouzel/gui/ScrollArea.cpp \
	../ouzel/gui/ScrollBar.cpp \
	../ouzel/gui/SlideBar.cpp \
	../ouzel/gui/Widget.cpp \
	../ouzel/input/Gamepad.cpp \
	../ouzel/input/Input.cpp \
	../ouzel/localization/Language.cpp \
	../ouzel/localization/Localization.cpp \
	../ouzel/math/AABB2.cpp \
	../ouzel/math/MathUtils.cpp \
	../ouzel/math/Matrix3.cpp \
	../ouzel/math/Matrix4.cpp \
	../ouzel/math/Rectangle.cpp \
	../ouzel/math/Size2.cpp \
	../ouzel/math/Size3.cpp \
	../ouzel/math/Vector2.cpp \
	../ouzel/math/Vector3.cpp \
	../ouzel/math/Vector4.cpp \
	../ouzel/scene/Camera.cpp \
	../ouzel/scene/Component.cpp \
	../ouzel/scene/Layer.cpp \
	../ouzel/scene/Node.cpp \
	../ouzel/scene/NodeContainer.cpp \
	../ouzel/scene/ParticleDefinition.cpp \
	../ouzel/scene/ParticleSystem.cpp \
	../ouzel/scene/Scene.cpp \
	../ouzel/scene/SceneManager.cpp \
	../ouzel/scene/ShapeDrawable.cpp \
	../ouzel/scene/Sprite.cpp \
	../ouzel/scene/SpriteFrame.cpp \
	../ouzel/scene/TextDrawable.cpp \
	../ouzel/utils/Log.cpp \
	../ouzel/utils/Utils.cpp
ifeq ($(platform),raspbian)
SOURCES+=../ouzel/core/raspbian/ApplicationRasp.cpp \
	../ouzel/core/raspbian/main.cpp \
	../ouzel/core/raspbian/WindowRasp.cpp \
	../ouzel/graphics/raspbian/RendererOGLRasp.cpp \
	../ouzel/input/raspbian/GamepadRasp.cpp \
	../ouzel/input/raspbian/InputRasp.cpp
CXXFLAGS+=-DRASPBIAN -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else ifeq ($(platform),linux)
SOURCES+=../ouzel/core/linux/ApplicationLinux.cpp \
	../ouzel/core/linux/main.cpp \
	../ouzel/core/linux/WindowLinux.cpp \
	../ouzel/input/linux/GamepadLinux.cpp \
	../ouzel/input/linux/InputLinux.cpp \
	../ouzel/graphics/linux/RendererOGLLinux.cpp
else ifeq ($(platform),macos)
SOURCES+=../ouzel/audio/apple/AudioALApple.mm \
	../ouzel/core/macos/AppDelegate.mm \
	../ouzel/core/macos/ApplicationMacOS.mm \
	../ouzel/core/macos/main.cpp \
	../ouzel/core/macos/WindowMacOS.mm \
	../ouzel/input/apple/GamepadApple.mm \
	../ouzel/input/apple/InputApple.mm \
	../ouzel/graphics/macos/MetalView.mm \
	../ouzel/graphics/macos/OpenGLView.mm \
	../ouzel/graphics/macos/RendererOGLMacOS.mm \
	../ouzel/graphics/metal/BlendStateMetal.mm \
	../ouzel/graphics/metal/IndexBufferMetal.mm \
	../ouzel/graphics/metal/MeshBufferMetal.mm \
	../ouzel/graphics/metal/RendererMetal.mm \
	../ouzel/graphics/metal/RenderTargetMetal.mm \
	../ouzel/graphics/metal/ShaderMetal.mm \
	../ouzel/graphics/metal/TextureMetal.mm \
	../ouzel/graphics/metal/VertexBufferMetal.mm
else ifeq ($(platform),emscripten)
SOURCES+=../ouzel/core/emscripten/ApplicationEm.cpp \
	../ouzel/core/emscripten/main.cpp \
	../ouzel/core/emscripten/WindowEm.cpp \
	../ouzel/graphics/emscripten/RendererOGLEm.cpp \
	../ouzel/input/emscripten/InputEm.cpp
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
	../ouzel/audio/*.o \
	../ouzel/audio/apple/*.o \
	../ouzel/audio/openal/*.o \
	../ouzel/audio/empty/*.o \
	../ouzel/animators/*.o \
	../ouzel/core/*.o \
	../ouzel/core/apple/*.o \
	../ouzel/core/emscripten/*.o \
	../ouzel/core/linux/*.o \
	../ouzel/core/macos/*.o \
	../ouzel/core/raspbian/*.o \
	../ouzel/events/*.o \
	../ouzel/files/*.o \
	../ouzel/graphics/*.o \
	../ouzel/graphics/emscripten/*.o \
	../ouzel/graphics/linux/*.o \
	../ouzel/graphics/macos/*.o \
	../ouzel/graphics/metal/*.o \
	../ouzel/graphics/opengl/*.o \
	../ouzel/graphics/raspbian/*.o \
	../ouzel/graphics/empty/*.o \
	../ouzel/gui/*.o \
	../ouzel/input/*.o \
	../ouzel/input/apple/*.o \
	../ouzel/input/emscripten/*.o \
	../ouzel/input/linux/*.o \
	../ouzel/input/raspbian/*.o \
	../ouzel/localization/*.o \
	../ouzel/math/*.o \
	../ouzel/scene/*.o \
	../ouzel/utils/*.o \
	../ouzel/*.o
