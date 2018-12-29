LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell (if [ ! -f $(LOCAL_PATH)/../../ouzel/Config.h ]; then \
    cp $(LOCAL_PATH)/../../ouzel/DefaultConfig.h $(LOCAL_PATH)/../../ouzel/Config.h; \
fi))

LOCAL_MODULE := ouzel
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -Wall -Wextra
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../shaders \
    $(LOCAL_PATH)/../../external/khronos \
	$(LOCAL_PATH)/../../external/smbPitchShift \
	$(LOCAL_PATH)/../../external/stb

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
	../../ouzel/assets/Bundle.cpp \
    ../../ouzel/assets/Cache.cpp \
    ../../ouzel/assets/Loader.cpp \
    ../../ouzel/assets/BmfLoader.cpp \
    ../../ouzel/assets/ColladaLoader.cpp \
	../../ouzel/assets/GltfLoader.cpp \
    ../../ouzel/assets/ImageLoader.cpp \
    ../../ouzel/assets/MtlLoader.cpp \
    ../../ouzel/assets/ObjLoader.cpp \
    ../../ouzel/assets/ParticleSystemLoader.cpp \
    ../../ouzel/assets/SpriteLoader.cpp \
    ../../ouzel/assets/TtfLoader.cpp \
    ../../ouzel/assets/VorbisLoader.cpp \
    ../../ouzel/assets/WaveLoader.cpp \
    ../../ouzel/audio/empty/AudioDeviceEmpty.cpp \
    ../../ouzel/audio/mixer/Bus.cpp \
	../../ouzel/audio/mixer/Mixer.cpp \
    ../../ouzel/audio/mixer/Processor.cpp \
    ../../ouzel/audio/mixer/Source.cpp \
    ../../ouzel/audio/mixer/SourceData.cpp \
    ../../ouzel/audio/opensl/AudioDeviceSL.cpp \
    ../../ouzel/audio/Audio.cpp \
    ../../ouzel/audio/AudioDevice.cpp \
	../../ouzel/audio/Containers.cpp \
	../../ouzel/audio/Filter.cpp \
	../../ouzel/audio/Filters.cpp \
    ../../ouzel/audio/Listener.cpp \
	../../ouzel/audio/Mix.cpp \
    ../../ouzel/audio/OscillatorSound.cpp \
    ../../ouzel/audio/PCMSound.cpp \
	../../ouzel/audio/SilenceSound.cpp \
    ../../ouzel/audio/Sound.cpp \
	../../ouzel/audio/SoundCue.cpp \
	../../ouzel/audio/Source.cpp \
    ../../ouzel/audio/Submix.cpp \
	../../ouzel/audio/Voice.cpp \
    ../../ouzel/audio/VorbisSound.cpp \
    ../../ouzel/core/android/EngineAndroid.cpp \
    ../../ouzel/core/android/main.cpp \
    ../../ouzel/core/android/NativeWindowAndroid.cpp \
	../../ouzel/core/android/SystemAndroid.cpp \
    ../../ouzel/core/Engine.cpp \
	../../ouzel/core/NativeWindow.cpp \
	../../ouzel/core/System.cpp \
    ../../ouzel/core/Timer.cpp \
    ../../ouzel/core/Window.cpp \
    ../../ouzel/events/EventDispatcher.cpp \
    ../../ouzel/events/EventHandler.cpp \
    ../../ouzel/files/Archive.cpp \
    ../../ouzel/files/File.cpp \
    ../../ouzel/files/FileSystem.cpp \
    ../../ouzel/graphics/empty/RenderDeviceEmpty.cpp \
    ../../ouzel/graphics/opengl/android/RenderDeviceOGLAndroid.cpp \
    ../../ouzel/graphics/opengl/BlendStateOGL.cpp \
    ../../ouzel/graphics/opengl/BufferOGL.cpp \
	../../ouzel/graphics/opengl/DepthStencilStateOGL.cpp \
    ../../ouzel/graphics/opengl/RenderDeviceOGL.cpp \
	../../ouzel/graphics/opengl/RenderTargetOGL.cpp \
    ../../ouzel/graphics/opengl/ShaderOGL.cpp \
    ../../ouzel/graphics/opengl/TextureOGL.cpp \
    ../../ouzel/graphics/BlendState.cpp \
    ../../ouzel/graphics/Buffer.cpp \
	../../ouzel/graphics/DepthStencilState.cpp \
    ../../ouzel/graphics/ImageData.cpp \
    ../../ouzel/graphics/Material.cpp \
    ../../ouzel/graphics/Renderer.cpp \
    ../../ouzel/graphics/RenderDevice.cpp \
	../../ouzel/graphics/RenderTarget.cpp \
    ../../ouzel/graphics/Shader.cpp \
    ../../ouzel/graphics/Texture.cpp \
    ../../ouzel/graphics/Vertex.cpp \
    ../../ouzel/gui/BMFont.cpp \
    ../../ouzel/gui/TTFont.cpp \
    ../../ouzel/gui/Button.cpp \
    ../../ouzel/gui/CheckBox.cpp \
    ../../ouzel/gui/ComboBox.cpp \
    ../../ouzel/gui/EditBox.cpp \
    ../../ouzel/gui/Font.cpp \
    ../../ouzel/gui/Label.cpp \
    ../../ouzel/gui/Menu.cpp \
    ../../ouzel/gui/RadioButton.cpp \
    ../../ouzel/gui/RadioButtonGroup.cpp \
    ../../ouzel/gui/ScrollArea.cpp \
    ../../ouzel/gui/ScrollBar.cpp \
    ../../ouzel/gui/SlideBar.cpp \
    ../../ouzel/gui/Widget.cpp \
    ../../ouzel/input/android/GamepadDeviceAndroid.cpp \
	../../ouzel/input/android/InputSystemAndroid.cpp \
    ../../ouzel/input/Cursor.cpp \
    ../../ouzel/input/Gamepad.cpp \
	../../ouzel/input/GamepadDevice.cpp \
	../../ouzel/input/InputDevice.cpp \
    ../../ouzel/input/InputManager.cpp \
    ../../ouzel/input/InputSystem.cpp \
	../../ouzel/input/Keyboard.cpp \
	../../ouzel/input/KeyboardDevice.cpp \
	../../ouzel/input/Mouse.cpp \
	../../ouzel/input/MouseDevice.cpp \
	../../ouzel/input/Touchpad.cpp \
	../../ouzel/input/TouchpadDevice.cpp \
    ../../ouzel/localization/Language.cpp \
    ../../ouzel/localization/Localization.cpp \
    ../../ouzel/math/Box2.cpp \
    ../../ouzel/math/Box3.cpp \
    ../../ouzel/math/Color.cpp \
    ../../ouzel/math/MathUtils.cpp \
    ../../ouzel/math/Matrix4.cpp \
    ../../ouzel/math/Plane.cpp \
    ../../ouzel/math/Quaternion.cpp \
    ../../ouzel/math/Rect.cpp \
    ../../ouzel/math/Size2.cpp \
    ../../ouzel/math/Size3.cpp \
    ../../ouzel/math/Vector2.cpp \
    ../../ouzel/math/Vector3.cpp \
    ../../ouzel/math/Vector4.cpp \
    ../../ouzel/network/Client.cpp \
    ../../ouzel/network/Network.cpp \
    ../../ouzel/network/Socket.cpp \
    ../../ouzel/scene/Actor.cpp \
    ../../ouzel/scene/ActorContainer.cpp \
    ../../ouzel/scene/Camera.cpp \
    ../../ouzel/scene/Component.cpp \
    ../../ouzel/scene/Layer.cpp \
    ../../ouzel/scene/Light.cpp \
    ../../ouzel/scene/ParticleSystem.cpp \
    ../../ouzel/scene/ParticleSystemData.cpp \
    ../../ouzel/scene/Scene.cpp \
    ../../ouzel/scene/SceneManager.cpp \
    ../../ouzel/scene/ShapeRenderer.cpp \
    ../../ouzel/scene/SkinnedMeshData.cpp \
    ../../ouzel/scene/SkinnedMeshRenderer.cpp \
    ../../ouzel/scene/Sprite.cpp \
    ../../ouzel/scene/SpriteData.cpp \
    ../../ouzel/scene/StaticMeshData.cpp \
    ../../ouzel/scene/StaticMeshRenderer.cpp \
    ../../ouzel/scene/TextRenderer.cpp \
    ../../ouzel/utils/INI.cpp \
    ../../ouzel/utils/JSON.cpp \
    ../../ouzel/utils/Log.cpp \
    ../../ouzel/utils/OBF.cpp \
    ../../ouzel/utils/Utils.cpp \
    ../../ouzel/utils/XML.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)