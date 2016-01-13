LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += ../../../external/stb \
    ../../../external/rapidjson/include

LOCAL_SRC_FILES := ../../../ouzel/android/jni.cpp \
    ../../../ouzel/AABB2.cpp \
    ../../../ouzel/BMFont.cpp \
    ../../../ouzel/Button.cpp \
    ../../../ouzel/Camera.cpp \
    ../../../ouzel/Color.cpp \
    ../../../ouzel/Engine.cpp \
    ../../../ouzel/EventDispatcher.cpp \
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
    ../../../ouzel/MeshBufferOGL.cpp \
    ../../../ouzel/Node.cpp \
    ../../../ouzel/NodeContainer.cpp \
    ../../../ouzel/ParticleSystem.cpp \
    ../../../ouzel/Rectangle.cpp \
    ../../../ouzel/Renderer.cpp \
    ../../../ouzel/RendererOGL.cpp \
    ../../../ouzel/RenderTarget.cpp \
    ../../../ouzel/RenderTargetOGL.cpp \
    ../../../ouzel/Scene.cpp \
    ../../../ouzel/SceneManager.cpp \
    ../../../ouzel/Shader.cpp \
    ../../../ouzel/ShaderOGL.cpp \
    ../../../ouzel/Size2.cpp \
    ../../../ouzel/Sprite.cpp \
    ../../../ouzel/Texture.cpp \
    ../../../ouzel/TextureOGL.cpp \
    ../../../ouzel/Utils.cpp \
    ../../../ouzel/Vector2.cpp \
    ../../../ouzel/Vector3.cpp \
    ../../../ouzel/Vector4.cpp \
    ../../../ouzel/Vertex.cpp \
    ../../../ouzel/Widget.cpp

include $(BUILD_STATIC_LIBRARY)