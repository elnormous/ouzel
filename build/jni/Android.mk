LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../external/stb \
    $(LOCAL_PATH)/../../external/rapidjson/include \
    $(LOCAL_PATH)/../../external/utfcpp/source

LOCAL_SRC_FILES :=../../ouzel/animators/Animator.cpp \
    ../../ouzel/animators/Ease.cpp \
    ../../ouzel/animators/Fade.cpp \
    ../../ouzel/animators/Move.cpp \
    ../../ouzel/animators/Parallel.cpp \
    ../../ouzel/animators/Repeat.cpp \
    ../../ouzel/animators/Rotate.cpp \
    ../../ouzel/animators/Scale.cpp \
    ../../ouzel/animators/Sequence.cpp \
    ../../ouzel/animators/Shake.cpp \
    ../../ouzel/audio/empty/AudioEmpty.cpp \
    ../../ouzel/audio/empty/SoundEmpty.cpp \
    ../../ouzel/audio/empty/SoundDataEmpty.cpp \
    ../../ouzel/audio/opensl/AudioSL.cpp \
    ../../ouzel/audio/opensl/SoundSL.cpp \
    ../../ouzel/audio/opensl/SoundDataSL.cpp \
    ../../ouzel/audio/Audio.cpp \
    ../../ouzel/audio/Sound.cpp \
    ../../ouzel/audio/SoundData.cpp \
    ../../ouzel/core/android/ApplicationAndroid.cpp \
    ../../ouzel/core/android/WindowAndroid.cpp \
    ../../ouzel/core/Application.cpp \
    ../../ouzel/core/Cache.cpp \
    ../../ouzel/core/Engine.cpp \
    ../../ouzel/core/UpdateCallback.cpp \
    ../../ouzel/core/Window.cpp \
    ../../ouzel/events/EventDispatcher.cpp \
    ../../ouzel/events/EventHandler.cpp \
    ../../ouzel/files/FileSystem.cpp \
    ../../ouzel/graphics/empty/BlendStateEmpty.cpp \
    ../../ouzel/graphics/empty/IndexBufferEmpty.cpp \
    ../../ouzel/graphics/empty/MeshBufferEmpty.cpp \
    ../../ouzel/graphics/empty/RendererEmpty.cpp \
    ../../ouzel/graphics/empty/RenderTargetEmpty.cpp \
    ../../ouzel/graphics/empty/ShaderEmpty.cpp \
    ../../ouzel/graphics/empty/TextureEmpty.cpp \
    ../../ouzel/graphics/empty/VertexBufferEmpty.cpp \
    ../../ouzel/graphics/opengl/BlendStateOGL.cpp \
    ../../ouzel/graphics/opengl/MeshBufferOGL.cpp \
    ../../ouzel/graphics/opengl/IndexBufferOGL.cpp \
    ../../ouzel/graphics/opengl/RendererOGL.cpp \
    ../../ouzel/graphics/opengl/RenderTargetOGL.cpp \
    ../../ouzel/graphics/opengl/ShaderOGL.cpp \
    ../../ouzel/graphics/opengl/TextureOGL.cpp \
    ../../ouzel/graphics/opengl/VertexBufferOGL.cpp \
    ../../ouzel/graphics/BlendState.cpp \
    ../../ouzel/graphics/Color.cpp \
    ../../ouzel/graphics/Image.cpp \
    ../../ouzel/graphics/IndexBuffer.cpp \
    ../../ouzel/graphics/MeshBuffer.cpp \
    ../../ouzel/graphics/Renderer.cpp \
    ../../ouzel/graphics/RenderTarget.cpp \
    ../../ouzel/graphics/Shader.cpp \
    ../../ouzel/graphics/Texture.cpp \
    ../../ouzel/graphics/Vertex.cpp \
    ../../ouzel/graphics/VertexBuffer.cpp \
    ../../ouzel/gui/BMFont.cpp \
    ../../ouzel/gui/Button.cpp \
    ../../ouzel/gui/CheckBox.cpp \
    ../../ouzel/gui/ComboBox.cpp \
    ../../ouzel/gui/EditBox.cpp \
    ../../ouzel/gui/Label.cpp \
    ../../ouzel/gui/Menu.cpp \
    ../../ouzel/gui/Popup.cpp \
    ../../ouzel/gui/RadioButton.cpp \
    ../../ouzel/gui/RadioButtonGroup.cpp \
    ../../ouzel/gui/ScrollArea.cpp \
    ../../ouzel/gui/ScrollBar.cpp \
	../../ouzel/gui/SlideBar.cpp \
    ../../ouzel/gui/Widget.cpp \
    ../../ouzel/input/android/GamepadAndroid.cpp \
    ../../ouzel/input/android/InputAndroid.cpp \
    ../../ouzel/input/Gamepad.cpp \
    ../../ouzel/input/Input.cpp \
    ../../ouzel/localization/Language.cpp \
    ../../ouzel/localization/Localization.cpp \
    ../../ouzel/math/AABB2.cpp \
    ../../ouzel/math/MathUtils.cpp \
    ../../ouzel/math/Matrix3.cpp \
    ../../ouzel/math/Matrix4.cpp \
    ../../ouzel/math/Rectangle.cpp \
    ../../ouzel/math/Size2.cpp \
	../../ouzel/math/Size3.cpp \
    ../../ouzel/math/Vector2.cpp \
    ../../ouzel/math/Vector3.cpp \
    ../../ouzel/math/Vector4.cpp \
    ../../ouzel/scene/Camera.cpp \
    ../../ouzel/scene/Component.cpp \
    ../../ouzel/scene/Layer.cpp \
    ../../ouzel/scene/Node.cpp \
    ../../ouzel/scene/NodeContainer.cpp \
	../../ouzel/scene/ParticleDefinition.cpp \
    ../../ouzel/scene/ParticleSystem.cpp \
    ../../ouzel/scene/Scene.cpp \
    ../../ouzel/scene/SceneManager.cpp \
    ../../ouzel/scene/ShapeDrawable.cpp \
    ../../ouzel/scene/Sprite.cpp \
    ../../ouzel/scene/SpriteFrame.cpp \
    ../../ouzel/scene/TextDrawable.cpp \
    ../../ouzel/utils/Log.cpp \
    ../../ouzel/utils/Utils.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)