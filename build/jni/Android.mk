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
LOCAL_PCH := ../Prefix.pch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../ouzel \
    $(LOCAL_PATH)/../../shaders \
    $(LOCAL_PATH)/../../external/khronos \
	$(LOCAL_PATH)/../../external/smbPitchShift \
	$(LOCAL_PATH)/../../external/stb

LOCAL_SRC_FILES := ../../ouzel/assets/BmfLoader.cpp \
	../../ouzel/assets/Bundle.cpp \
	../../ouzel/assets/Cache.cpp \
    ../../ouzel/assets/ColladaLoader.cpp \
	../../ouzel/assets/GltfLoader.cpp \
    ../../ouzel/assets/ImageLoader.cpp \
	../../ouzel/assets/Loader.cpp \
    ../../ouzel/assets/MtlLoader.cpp \
    ../../ouzel/assets/ObjLoader.cpp \
    ../../ouzel/assets/ParticleSystemLoader.cpp \
    ../../ouzel/assets/SpriteLoader.cpp \
    ../../ouzel/assets/TtfLoader.cpp \
    ../../ouzel/assets/VorbisLoader.cpp \
    ../../ouzel/assets/WaveLoader.cpp \
    ../../ouzel/audio/empty/EmptyAudioDevice.cpp \
    ../../ouzel/audio/mixer/Bus.cpp \
	../../ouzel/audio/mixer/Mixer.cpp \
    ../../ouzel/audio/mixer/Processor.cpp \
    ../../ouzel/audio/mixer/Stream.cpp \
    ../../ouzel/audio/mixer/Source.cpp \
    ../../ouzel/audio/opensl/OSLAudioDevice.cpp \
    ../../ouzel/audio/Audio.cpp \
    ../../ouzel/audio/AudioDevice.cpp \
	../../ouzel/audio/Containers.cpp \
	../../ouzel/audio/Effect.cpp \
	../../ouzel/audio/Effects.cpp \
    ../../ouzel/audio/Listener.cpp \
	../../ouzel/audio/Mix.cpp \
    ../../ouzel/audio/Oscillator.cpp \
    ../../ouzel/audio/PcmSound.cpp \
	../../ouzel/audio/SilenceSound.cpp \
    ../../ouzel/audio/Sound.cpp \
    ../../ouzel/audio/Submix.cpp \
	../../ouzel/audio/Voice.cpp \
    ../../ouzel/audio/VorbisClip.cpp \
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
    ../../ouzel/graphics/empty/EmptyRenderDevice.cpp \
    ../../ouzel/graphics/opengl/android/OGLRenderDeviceAndroid.cpp \
    ../../ouzel/graphics/opengl/OGLBlendState.cpp \
    ../../ouzel/graphics/opengl/OGLBuffer.cpp \
	../../ouzel/graphics/opengl/OGLDepthStencilState.cpp \
    ../../ouzel/graphics/opengl/OGLRenderDevice.cpp \
	../../ouzel/graphics/opengl/OGLRenderTarget.cpp \
    ../../ouzel/graphics/opengl/OGLShader.cpp \
    ../../ouzel/graphics/opengl/OGLTexture.cpp \
    ../../ouzel/graphics/BlendState.cpp \
    ../../ouzel/graphics/Buffer.cpp \
	../../ouzel/graphics/DepthStencilState.cpp \
	../../ouzel/graphics/GraphicsResource.cpp \
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
    ../../ouzel/math/MathUtils.cpp \
    ../../ouzel/math/Matrix.cpp \
    ../../ouzel/network/Client.cpp \
    ../../ouzel/network/Network.cpp \
	../../ouzel/network/Server.cpp \
    ../../ouzel/network/Socket.cpp \
    ../../ouzel/scene/Actor.cpp \
    ../../ouzel/scene/ActorContainer.cpp \
    ../../ouzel/scene/Animator.cpp \
	../../ouzel/scene/Animators.cpp \
    ../../ouzel/scene/Camera.cpp \
    ../../ouzel/scene/Component.cpp \
    ../../ouzel/scene/Layer.cpp \
    ../../ouzel/scene/Light.cpp \
    ../../ouzel/scene/ParticleSystem.cpp \
    ../../ouzel/scene/Scene.cpp \
    ../../ouzel/scene/SceneManager.cpp \
    ../../ouzel/scene/ShapeRenderer.cpp \
    ../../ouzel/scene/SkinnedMeshRenderer.cpp \
    ../../ouzel/scene/Sprite.cpp \
    ../../ouzel/scene/StaticMeshRenderer.cpp \
    ../../ouzel/scene/TextRenderer.cpp \
	../../ouzel/storage/Archive.cpp \
    ../../ouzel/storage/File.cpp \
    ../../ouzel/storage/FileSystem.cpp \
    ../../ouzel/utils/Log.cpp \
    ../../ouzel/utils/Obf.cpp \
    ../../ouzel/utils/Utils.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)