CFLAGS=-c -std=c++11 -Wall -I../external/rapidjson/include -I../external/stb -I../ouzel
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
	../ouzel/core/Cache.cpp \
	../ouzel/core/Engine.cpp \
	../ouzel/core/Window.cpp \
	../ouzel/events/EventDispatcher.cpp \
	../ouzel/files/FileSystem.cpp \
	../ouzel/graphics/BlendState.cpp \
	../ouzel/graphics/Color.cpp \
	../ouzel/graphics/Image.cpp \
	../ouzel/graphics/MeshBuffer.cpp \
	../ouzel/graphics/Renderer.cpp \
	../ouzel/graphics/RenderTarget.cpp \
	../ouzel/graphics/Shader.cpp \
	../ouzel/graphics/Texture.cpp \
	../ouzel/graphics/Vertex.cpp \
	../ouzel/gui/BMFont.cpp \
	../ouzel/gui/Button.cpp \
	../ouzel/gui/CheckBox.cpp \
	../ouzel/gui/ComboBox.cpp \
	../ouzel/gui/EditBox.cpp \
	../ouzel/gui/Label.cpp \
	../ouzel/gui/Menu.cpp \
	../ouzel/gui/Popup.cpp \
	../ouzel/gui/RadioButton.cpp \
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
	../ouzel/opengl/BlendStateOGL.cpp \
	../ouzel/opengl/MeshBufferOGL.cpp \
	../ouzel/opengl/RendererOGL.cpp \
	../ouzel/opengl/RenderTargetOGL.cpp \
	../ouzel/opengl/ShaderOGL.cpp \
	../ouzel/opengl/TextureOGL.cpp \
	../ouzel/scene/Camera.cpp \
	../ouzel/scene/DebugDrawable.cpp \
	../ouzel/scene/Drawable.cpp \
	../ouzel/scene/Layer.cpp \
	../ouzel/scene/Node.cpp \
	../ouzel/scene/NodeContainer.cpp \
	../ouzel/scene/ParticleDefinition.cpp \
	../ouzel/scene/ParticleSystem.cpp \
	../ouzel/scene/Scene.cpp \
	../ouzel/scene/SceneManager.cpp \
	../ouzel/scene/Sprite.cpp \
	../ouzel/scene/SpriteFrame.cpp \
	../ouzel/scene/TextDrawable.cpp \
	../ouzel/utils/Utils.cpp
ifeq ($(platform),raspbian)
SOURCES+=../ouzel/rpi/main.cpp \
	../ouzel/rpi/InputRPI.cpp \
	../ouzel/rpi/RendererOGLRPI.cpp \
	../ouzel/rpi/WindowRPI.cpp
CFLAGS+=-DRASPBIAN -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
else
SOURCES+=../ouzel/linux/main.cpp \
	../ouzel/linux/RendererOGLLinux.cpp \
	../ouzel/linux/WindowLinux.cpp
endif
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libouzel.a

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar -r -s $@ $^

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(LIBRARY) \
	../ouzel/animators/*.o \
	../ouzel/core/*.o \
	../ouzel/events/*.o \
	../ouzel/files/*.o \
	../ouzel/graphics/*.o \
	../ouzel/gui/*.o \
	../ouzel/input/*.o \
	../ouzel/linux/*.o \
	../ouzel/localization/*.o \
	../ouzel/math/*.o \
	../ouzel/metal/*.o \
	../ouzel/opengl/*.o \
	../ouzel/rpi/*.o \
	../ouzel/scene/*.o \
	../ouzel/utils/*.o
