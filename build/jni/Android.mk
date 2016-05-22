LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../external/stb \
    $(LOCAL_PATH)/../../external/rapidjson/include

LOCAL_SRC_FILES := $(LOCAL_PATH)/../../ouzel/AABB2.cpp \
	$(LOCAL_PATH)/../../ouzel/Animator.cpp \
    $(LOCAL_PATH)/../../ouzel/BlendState.cpp \
    $(LOCAL_PATH)/../../ouzel/BMFont.cpp \
    $(LOCAL_PATH)/../../ouzel/Button.cpp \
    $(LOCAL_PATH)/../../ouzel/Cache.cpp \
    $(LOCAL_PATH)/../../ouzel/Camera.cpp \
    $(LOCAL_PATH)/../../ouzel/CheckBox.cpp \
    $(LOCAL_PATH)/../../ouzel/Color.cpp \
    $(LOCAL_PATH)/../../ouzel/ComboBox.cpp \
    $(LOCAL_PATH)/../../ouzel/DebugDrawable.cpp \
    $(LOCAL_PATH)/../../ouzel/Drawable.cpp \
    $(LOCAL_PATH)/../../ouzel/Ease.cpp \
    $(LOCAL_PATH)/../../ouzel/EditBox.cpp \
    $(LOCAL_PATH)/../../ouzel/Engine.cpp \
    $(LOCAL_PATH)/../../ouzel/EventDispatcher.cpp \
	$(LOCAL_PATH)/../../ouzel/Fade.cpp \
    $(LOCAL_PATH)/../../ouzel/FileSystem.cpp \
    $(LOCAL_PATH)/../../ouzel/Gamepad.cpp \
    $(LOCAL_PATH)/../../ouzel/Image.cpp \
    $(LOCAL_PATH)/../../ouzel/Input.cpp \
    $(LOCAL_PATH)/../../ouzel/Label.cpp \
    $(LOCAL_PATH)/../../ouzel/Language.cpp \
    $(LOCAL_PATH)/../../ouzel/Layer.cpp \
    $(LOCAL_PATH)/../../ouzel/Localization.cpp \
    $(LOCAL_PATH)/../../ouzel/MathUtils.cpp \
    $(LOCAL_PATH)/../../ouzel/Matrix3.cpp \
    $(LOCAL_PATH)/../../ouzel/Matrix4.cpp \
    $(LOCAL_PATH)/../../ouzel/Menu.cpp \
    $(LOCAL_PATH)/../../ouzel/MeshBuffer.cpp \
    $(LOCAL_PATH)/../../ouzel/Move.cpp \
    $(LOCAL_PATH)/../../ouzel/Node.cpp \
    $(LOCAL_PATH)/../../ouzel/NodeContainer.cpp \
    $(LOCAL_PATH)/../../ouzel/Parallel.cpp \
	$(LOCAL_PATH)/../../ouzel/ParticleDefinition.cpp \
    $(LOCAL_PATH)/../../ouzel/ParticleSystem.cpp \
    $(LOCAL_PATH)/../../ouzel/Popup.cpp \
    $(LOCAL_PATH)/../../ouzel/RadioButton.cpp \
    $(LOCAL_PATH)/../../ouzel/Rectangle.cpp \
    $(LOCAL_PATH)/../../ouzel/Renderer.cpp \
    $(LOCAL_PATH)/../../ouzel/RenderTarget.cpp \
    $(LOCAL_PATH)/../../ouzel/Repeat.cpp \
	$(LOCAL_PATH)/../../ouzel/Rotate.cpp \
	$(LOCAL_PATH)/../../ouzel/Scale.cpp \
    $(LOCAL_PATH)/../../ouzel/Scene.cpp \
    $(LOCAL_PATH)/../../ouzel/SceneManager.cpp \
    $(LOCAL_PATH)/../../ouzel/ScrollArea.cpp \
    $(LOCAL_PATH)/../../ouzel/ScrollBar.cpp \
    $(LOCAL_PATH)/../../ouzel/Sequence.cpp \
    $(LOCAL_PATH)/../../ouzel/Shader.cpp \
    $(LOCAL_PATH)/../../ouzel/Shake.cpp \
    $(LOCAL_PATH)/../../ouzel/Size2.cpp \
	$(LOCAL_PATH)/../../ouzel/Size3.cpp \
	$(LOCAL_PATH)/../../ouzel/SlideBar.cpp \
    $(LOCAL_PATH)/../../ouzel/Sprite.cpp \
    $(LOCAL_PATH)/../../ouzel/SpriteFrame.cpp \
    $(LOCAL_PATH)/../../ouzel/TextDrawable.cpp \
    $(LOCAL_PATH)/../../ouzel/Texture.cpp \
    $(LOCAL_PATH)/../../ouzel/Utils.cpp \
    $(LOCAL_PATH)/../../ouzel/Vector2.cpp \
    $(LOCAL_PATH)/../../ouzel/Vector3.cpp \
    $(LOCAL_PATH)/../../ouzel/Vector4.cpp \
    $(LOCAL_PATH)/../../ouzel/Vertex.cpp \
    $(LOCAL_PATH)/../../ouzel/Widget.cpp \
    $(LOCAL_PATH)/../../ouzel/Window.cpp \
    $(LOCAL_PATH)/../../ouzel/android/WindowAndroid.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/BlendStateOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/MeshBufferOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/RendererOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/RenderTargetOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/ShaderOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/TextureOGL.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)