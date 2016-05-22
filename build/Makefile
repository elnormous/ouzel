CC=g++
CFLAGS=-c -std=c++11 -Wall -I../external/rapidjson/include -I../external/stb -I../ouzel
SOURCES=../ouzel/AABB2.cpp \
	../ouzel/Animator.cpp \
	../ouzel/BlendState.cpp \
	../ouzel/BMFont.cpp \
	../ouzel/Button.cpp \
	../ouzel/Cache.cpp \
	../ouzel/Camera.cpp \
	../ouzel/CheckBox.cpp \
	../ouzel/Color.cpp \
	../ouzel/ComboBox.cpp \
	../ouzel/DebugDrawable.cpp \
	../ouzel/Drawable.cpp \
	../ouzel/Ease.cpp \
	../ouzel/EditBox.cpp \
	../ouzel/Engine.cpp \
	../ouzel/EventDispatcher.cpp \
	../ouzel/Fade.cpp \
	../ouzel/FileSystem.cpp \
	../ouzel/Gamepad.cpp \
	../ouzel/Image.cpp \
	../ouzel/Input.cpp \
	../ouzel/Label.cpp \
	../ouzel/Language.cpp \
	../ouzel/Layer.cpp \
	../ouzel/Localization.cpp \
	../ouzel/MathUtils.cpp \
	../ouzel/Matrix3.cpp \
	../ouzel/Matrix4.cpp \
	../ouzel/Menu.cpp \
	../ouzel/MeshBuffer.cpp \
	../ouzel/Move.cpp \
	../ouzel/Node.cpp \
	../ouzel/NodeContainer.cpp \
	../ouzel/Parallel.cpp \
	../ouzel/ParticleDefinition.cpp \
	../ouzel/ParticleSystem.cpp \
	../ouzel/Popup.cpp \
	../ouzel/RadioButton.cpp \
	../ouzel/Rectangle.cpp \
	../ouzel/Renderer.cpp \
	../ouzel/RenderTarget.cpp \
	../ouzel/Repeat.cpp \
	../ouzel/Rotate.cpp \
	../ouzel/Scale.cpp \
	../ouzel/Scene.cpp \
	../ouzel/SceneManager.cpp \
	../ouzel/ScrollArea.cpp \
	../ouzel/ScrollBar.cpp \
	../ouzel/Sequence.cpp \
	../ouzel/Shader.cpp \
	../ouzel/Shake.cpp \
	../ouzel/Size2.cpp \
	../ouzel/Size3.cpp \
	../ouzel/SlideBar.cpp \
	../ouzel/Sprite.cpp \
	../ouzel/SpriteFrame.cpp \
	../ouzel/TextDrawable.cpp \
	../ouzel/Texture.cpp \
	../ouzel/Utils.cpp \
	../ouzel/Vector2.cpp \
	../ouzel/Vector3.cpp \
	../ouzel/Vector4.cpp \
	../ouzel/Vertex.cpp \
	../ouzel/Widget.cpp \
	../ouzel/Window.cpp \
	../ouzel/linux/main.cpp \
	../ouzel/linux/WindowLinux.cpp \
	../ouzel/opengl/BlendStateOGL.cpp \
	../ouzel/opengl/MeshBufferOGL.cpp \
	../ouzel/opengl/RendererOGL.cpp \
	../ouzel/opengl/RenderTargetOGL.cpp \
	../ouzel/opengl/ShaderOGL.cpp \
	../ouzel/opengl/TextureOGL.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libouzel.a

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar -r -s $@ $^

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(LIBRARY) ../ouzel/*.o ../ouzel/linux/*.o ../ouzel/opengl/*.o
