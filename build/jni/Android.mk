LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ouzel
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../external/stb \
    $(LOCAL_PATH)/../../external/rapidjson/include \
    $(LOCAL_PATH)/../../external/khronos

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
    ../../ouzel/audio/opensl/AudioSL.cpp \
    ../../ouzel/audio/Audio.cpp \
    ../../ouzel/audio/Sound.cpp \
    ../../ouzel/audio/SoundData.cpp \
    ../../ouzel/audio/SoundDataVorbis.cpp \
    ../../ouzel/audio/SoundDataWave.cpp \
    ../../ouzel/audio/Stream.cpp \
    ../../ouzel/audio/StreamVorbis.cpp \
    ../../ouzel/audio/StreamWave.cpp \
    ../../ouzel/audio/SoundResource.cpp \
    ../../ouzel/core/android/EngineAndroid.cpp \
    ../../ouzel/core/android/main.cpp \
    ../../ouzel/core/android/WindowAndroid.cpp \
    ../../ouzel/core/Cache.cpp \
    ../../ouzel/core/Engine.cpp \
    ../../ouzel/core/Timer.cpp \
    ../../ouzel/core/UpdateCallback.cpp \
    ../../ouzel/core/Window.cpp \
    ../../ouzel/events/EventDispatcher.cpp \
    ../../ouzel/events/EventHandler.cpp \
    ../../ouzel/files/android/FileSystemAndroid.cpp \
    ../../ouzel/files/Archive.cpp \
    ../../ouzel/files/FileSystem.cpp \
    ../../ouzel/graphics/empty/BlendStateInterfaceEmpty.cpp \
    ../../ouzel/graphics/empty/BufferInterfaceEmpty.cpp \
    ../../ouzel/graphics/empty/MeshBufferInterfaceEmpty.cpp \
    ../../ouzel/graphics/empty/RendererEmpty.cpp \
    ../../ouzel/graphics/empty/ShaderInterfaceEmpty.cpp \
    ../../ouzel/graphics/empty/TextureInterfaceEmpty.cpp \
    ../../ouzel/graphics/opengl/android/RendererOGLAndroid.cpp \
    ../../ouzel/graphics/opengl/BlendStateInterfaceOGL.cpp \
    ../../ouzel/graphics/opengl/BufferInterfaceOGL.cpp \
    ../../ouzel/graphics/opengl/MeshBufferInterfaceOGL.cpp \
    ../../ouzel/graphics/opengl/RendererOGL.cpp \
    ../../ouzel/graphics/opengl/ShaderInterfaceOGL.cpp \
    ../../ouzel/graphics/opengl/TextureInterfaceOGL.cpp \
    ../../ouzel/graphics/BlendState.cpp \
    ../../ouzel/graphics/BlendStateInterface.cpp \
    ../../ouzel/graphics/Buffer.cpp \
    ../../ouzel/graphics/BufferInterface.cpp \
    ../../ouzel/graphics/ImageData.cpp \
    ../../ouzel/graphics/ImageDataSTB.cpp \
    ../../ouzel/graphics/Material.cpp \
    ../../ouzel/graphics/MeshBuffer.cpp \
    ../../ouzel/graphics/MeshBufferInterface.cpp \
    ../../ouzel/graphics/Renderer.cpp \
    ../../ouzel/graphics/Shader.cpp \
    ../../ouzel/graphics/ShaderInterface.cpp \
    ../../ouzel/graphics/Texture.cpp \
    ../../ouzel/graphics/TextureInterface.cpp \
    ../../ouzel/graphics/Vertex.cpp \
    ../../ouzel/gui/BMFont.cpp \
    ../../ouzel/gui/Button.cpp \
    ../../ouzel/gui/CheckBox.cpp \
    ../../ouzel/gui/ComboBox.cpp \
    ../../ouzel/gui/EditBox.cpp \
    ../../ouzel/gui/Label.cpp \
    ../../ouzel/gui/Menu.cpp \
    ../../ouzel/gui/RadioButton.cpp \
    ../../ouzel/gui/RadioButtonGroup.cpp \
    ../../ouzel/gui/ScrollArea.cpp \
    ../../ouzel/gui/ScrollBar.cpp \
    ../../ouzel/gui/SlideBar.cpp \
    ../../ouzel/gui/Widget.cpp \
    ../../ouzel/input/android/GamepadAndroid.cpp \
    ../../ouzel/input/android/InputAndroid.cpp \
    ../../ouzel/input/Cursor.cpp \
    ../../ouzel/input/CursorResource.cpp \
    ../../ouzel/input/Gamepad.cpp \
    ../../ouzel/input/Input.cpp \
    ../../ouzel/localization/Language.cpp \
    ../../ouzel/localization/Localization.cpp \
    ../../ouzel/math/Box2.cpp \
    ../../ouzel/math/Box3.cpp \
    ../../ouzel/math/Color.cpp \
    ../../ouzel/math/ConvexVolume.cpp \
    ../../ouzel/math/MathUtils.cpp \
    ../../ouzel/math/Matrix3.cpp \
    ../../ouzel/math/Matrix4.cpp \
    ../../ouzel/math/Plane.cpp \
    ../../ouzel/math/Quaternion.cpp \
    ../../ouzel/math/Rectangle.cpp \
    ../../ouzel/math/Size2.cpp \
    ../../ouzel/math/Size3.cpp \
    ../../ouzel/math/Vector2.cpp \
    ../../ouzel/math/Vector3.cpp \
    ../../ouzel/math/Vector4.cpp \
    ../../ouzel/scene/Camera.cpp \
    ../../ouzel/scene/Component.cpp \
    ../../ouzel/scene/Layer.cpp \
    ../../ouzel/scene/MeshRenderer.cpp \
    ../../ouzel/scene/Node.cpp \
    ../../ouzel/scene/NodeContainer.cpp \
    ../../ouzel/scene/ParticleDefinition.cpp \
    ../../ouzel/scene/ParticleSystem.cpp \
    ../../ouzel/scene/Scene.cpp \
    ../../ouzel/scene/SceneManager.cpp \
    ../../ouzel/scene/ShapeRenderer.cpp \
    ../../ouzel/scene/SoundListener.cpp \
    ../../ouzel/scene/SoundPlayer.cpp \
    ../../ouzel/scene/Sprite.cpp \
    ../../ouzel/scene/SpriteFrame.cpp \
    ../../ouzel/scene/TextRenderer.cpp \
    ../../ouzel/utils/INI.cpp \
    ../../ouzel/utils/Log.cpp \
    ../../ouzel/utils/OBF.cpp \
    ../../ouzel/utils/Utils.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)