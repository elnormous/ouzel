LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += ../../../ouzel \
    ../../../external/stb \
    ../../../external/rapidjson/include

LOCAL_SRC_FILES := ../../../ouzel/AABB2.cpp \
	../../../ouzel/Animator.cpp \
    ../../../ouzel/BMFont.cpp \
    ../../../ouzel/Button.cpp \
    ../../../ouzel/Cache.cpp \
    ../../../ouzel/Camera.cpp \
    ../../../ouzel/CheckBox.cpp \
    ../../../ouzel/Color.cpp \
    ../../../ouzel/ComboBox.cpp \
    ../../../ouzel/DrawNode.cpp \
    ../../../ouzel/Ease.cpp \
    ../../../ouzel/EditBox.cpp \
    ../../../ouzel/Engine.cpp \
    ../../../ouzel/EventDispatcher.cpp \
	../../../ouzel/Fade.cpp \
    ../../../ouzel/File.cpp \
    ../../../ouzel/FileSystem.cpp \
    ../../../ouzel/Gamepad.cpp \
    ../../../ouzel/Image.cpp \
    ../../../ouzel/Input.cpp \
    ../../../ouzel/Label.cpp \
    ../../../ouzel/Layer.cpp \
    ../../../ouzel/MathUtils.cpp \
    ../../../ouzel/Matrix3.cpp \
    ../../../ouzel/Matrix4.cpp \
    ../../../ouzel/Menu.cpp \
    ../../../ouzel/MeshBuffer.cpp \
    ../../../ouzel/Move.cpp \
    ../../../ouzel/Node.cpp \
    ../../../ouzel/NodeContainer.cpp \
    ../../../ouzel/Parallel.cpp \
    ../../../ouzel/ParticleSystem.cpp \
    ../../../ouzel/Rectangle.cpp \
    ../../../ouzel/Renderer.cpp \
    ../../../ouzel/RenderTarget.cpp \
    ../../../ouzel/Popup.cpp \
    ../../../ouzel/Repeat.cpp \
	../../../ouzel/Rotate.cpp \
	../../../ouzel/Scale.cpp \
    ../../../ouzel/Scene.cpp \
    ../../../ouzel/SceneManager.cpp \
    ../../../ouzel/ScrollArea.cpp \
    ../../../ouzel/ScrollBar.cpp \
    ../../../ouzel/Sequence.cpp \
    ../../../ouzel/Shader.cpp \
    ../../../ouzel/Size2.cpp \
    ../../../ouzel/Sprite.cpp \
    ../../../ouzel/Texture.cpp \
    ../../../ouzel/Utils.cpp \
    ../../../ouzel/Vector2.cpp \
    ../../../ouzel/Vector3.cpp \
    ../../../ouzel/Vector4.cpp \
    ../../../ouzel/Vertex.cpp \
    ../../../ouzel/Widget.cpp \
    ../../../ouzel/Window.cpp \
    ../../../ouzel/android/WindowAndroid.cpp \
    ../../../ouzel/opengl/MeshBufferOGL.cpp \
    ../../../ouzel/opengl/RendererOGL.cpp \
    ../../../ouzel/opengl/RenderTargetOGL.cpp \
    ../../../ouzel/opengl/ShaderOGL.cpp \
    ../../../ouzel/opengl/TextureOGL.cpp

include $(BUILD_STATIC_LIBRARY)