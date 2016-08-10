LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../external/stb \
    $(LOCAL_PATH)/../../external/rapidjson/include

LOCAL_SRC_FILES :=$(LOCAL_PATH)/../../ouzel/android/ApplicationAndroid.cpp \
    $(LOCAL_PATH)/../../ouzel/android/GamepadAndroid.cpp \
    $(LOCAL_PATH)/../../ouzel/android/InputAndroid.cpp \
    $(LOCAL_PATH)/../../ouzel/android/WindowAndroid.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Animator.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Ease.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Fade.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Move.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Parallel.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Repeat.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Rotate.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Scale.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Sequence.cpp \
    $(LOCAL_PATH)/../../ouzel/animators/Shake.cpp \
    $(LOCAL_PATH)/../../ouzel/audio/Audio.cpp \
    $(LOCAL_PATH)/../../ouzel/audio/Sound.cpp \
    $(LOCAL_PATH)/../../ouzel/audio/SoundData.cpp \
    $(LOCAL_PATH)/../../ouzel/core/Application.cpp \
    $(LOCAL_PATH)/../../ouzel/core/Cache.cpp \
    $(LOCAL_PATH)/../../ouzel/core/Engine.cpp \
    $(LOCAL_PATH)/../../ouzel/core/Window.cpp \
    $(LOCAL_PATH)/../../ouzel/events/EventDispatcher.cpp \
    $(LOCAL_PATH)/../../ouzel/files/FileSystem.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/BlendState.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Color.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Image.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/MeshBuffer.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Renderer.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/RenderTarget.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Shader.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Texture.cpp \
    $(LOCAL_PATH)/../../ouzel/graphics/Vertex.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/BMFont.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/Button.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/CheckBox.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/ComboBox.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/EditBox.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/Label.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/Menu.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/Popup.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/RadioButton.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/ScrollArea.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/ScrollBar.cpp \
	$(LOCAL_PATH)/../../ouzel/gui/SlideBar.cpp \
    $(LOCAL_PATH)/../../ouzel/gui/Widget.cpp \
    $(LOCAL_PATH)/../../ouzel/input/Gamepad.cpp \
    $(LOCAL_PATH)/../../ouzel/input/Input.cpp \
    $(LOCAL_PATH)/../../ouzel/localization/Language.cpp \
    $(LOCAL_PATH)/../../ouzel/localization/Localization.cpp \
    $(LOCAL_PATH)/../../ouzel/math/AABB2.cpp \
    $(LOCAL_PATH)/../../ouzel/math/MathUtils.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Matrix3.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Matrix4.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Rectangle.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Size2.cpp \
	$(LOCAL_PATH)/../../ouzel/math/Size3.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Vector2.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Vector3.cpp \
    $(LOCAL_PATH)/../../ouzel/math/Vector4.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/BlendStateOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/MeshBufferOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/RendererOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/RenderTargetOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/ShaderOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opengl/TextureOGL.cpp \
    $(LOCAL_PATH)/../../ouzel/opensl/AudioSL.cpp \
    $(LOCAL_PATH)/../../ouzel/opensl/SoundSL.cpp \
    $(LOCAL_PATH)/../../ouzel/opensl/SoundDataSL.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Camera.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Drawable.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Layer.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Node.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/NodeContainer.cpp \
	$(LOCAL_PATH)/../../ouzel/scene/ParticleDefinition.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/ParticleSystem.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Scene.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/SceneManager.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/ShapeDrawable.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/Sprite.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/SpriteFrame.cpp \
    $(LOCAL_PATH)/../../ouzel/scene/TextDrawable.cpp \
    $(LOCAL_PATH)/../../ouzel/utils/Utils.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)