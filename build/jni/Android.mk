LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell (if [ ! -f $(LOCAL_PATH)/../../engine/Config.h ]; then \
    cp $(LOCAL_PATH)/../../engine/DefaultConfig.h $(LOCAL_PATH)/../../engine/Config.h; \
fi))

LOCAL_MODULE := ouzel
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -Wold-style-cast -std=c++14 -fexceptions
LOCAL_PCH := ../Prefix.pch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../engine \
    $(LOCAL_PATH)/../../shaders \
    $(LOCAL_PATH)/../../external/khronos \
	$(LOCAL_PATH)/../../external/smbPitchShift \
	$(LOCAL_PATH)/../../external/stb

LOCAL_SRC_FILES := ../../engine/assets/BmfLoader.cpp \
	../../engine/assets/Bundle.cpp \
	../../engine/assets/Cache.cpp \
    ../../engine/assets/ColladaLoader.cpp \
    ../../engine/assets/CueLoader.cpp \
	../../engine/assets/GltfLoader.cpp \
    ../../engine/assets/ImageLoader.cpp \
    ../../engine/assets/MtlLoader.cpp \
    ../../engine/assets/ObjLoader.cpp \
    ../../engine/assets/ParticleSystemLoader.cpp \
    ../../engine/assets/SpriteLoader.cpp \
    ../../engine/assets/TtfLoader.cpp \
    ../../engine/assets/VorbisLoader.cpp \
    ../../engine/assets/WaveLoader.cpp \
    ../../engine/audio/empty/EmptyAudioDevice.cpp \
    ../../engine/audio/mixer/Bus.cpp \
	../../engine/audio/mixer/Mixer.cpp \
    ../../engine/audio/opensl/OSLAudioDevice.cpp \
    ../../engine/audio/Audio.cpp \
    ../../engine/audio/AudioDevice.cpp \
	../../engine/audio/Containers.cpp \
    ../../engine/audio/Cue.cpp \
	../../engine/audio/Effect.cpp \
	../../engine/audio/Effects.cpp \
    ../../engine/audio/Listener.cpp \
	../../engine/audio/Mix.cpp \
    ../../engine/audio/Node.cpp \
    ../../engine/audio/Oscillator.cpp \
    ../../engine/audio/PcmClip.cpp \
	../../engine/audio/SilenceSound.cpp \
    ../../engine/audio/Sound.cpp \
    ../../engine/audio/Submix.cpp \
	../../engine/audio/Voice.cpp \
    ../../engine/audio/VorbisClip.cpp \
    ../../engine/core/android/EngineAndroid.cpp \
    ../../engine/core/android/NativeWindowAndroid.cpp \
	../../engine/core/android/SystemAndroid.cpp \
    ../../engine/core/Engine.cpp \
	../../engine/core/NativeWindow.cpp \
	../../engine/core/System.cpp \
    ../../engine/core/Window.cpp \
    ../../engine/events/EventDispatcher.cpp \
    ../../engine/graphics/opengl/android/OGLRenderDeviceAndroid.cpp \
    ../../engine/graphics/opengl/OGLBlendState.cpp \
    ../../engine/graphics/opengl/OGLBuffer.cpp \
	../../engine/graphics/opengl/OGLDepthStencilState.cpp \
    ../../engine/graphics/opengl/OGLRenderDevice.cpp \
	../../engine/graphics/opengl/OGLRenderTarget.cpp \
    ../../engine/graphics/opengl/OGLShader.cpp \
    ../../engine/graphics/opengl/OGLTexture.cpp \
    ../../engine/graphics/BlendState.cpp \
    ../../engine/graphics/Buffer.cpp \
	../../engine/graphics/DepthStencilState.cpp \
    ../../engine/graphics/Renderer.cpp \
    ../../engine/graphics/RenderDevice.cpp \
	../../engine/graphics/RenderTarget.cpp \
    ../../engine/graphics/Shader.cpp \
    ../../engine/graphics/Texture.cpp \
    ../../engine/gui/BMFont.cpp \
    ../../engine/gui/TTFont.cpp \
    ../../engine/gui/Widget.cpp \
	../../engine/gui/Widgets.cpp \
    ../../engine/input/android/GamepadDeviceAndroid.cpp \
	../../engine/input/android/InputSystemAndroid.cpp \
    ../../engine/input/Cursor.cpp \
    ../../engine/input/Gamepad.cpp \
	../../engine/input/GamepadDevice.cpp \
	../../engine/input/InputDevice.cpp \
    ../../engine/input/InputManager.cpp \
    ../../engine/input/InputSystem.cpp \
	../../engine/input/Keyboard.cpp \
	../../engine/input/KeyboardDevice.cpp \
	../../engine/input/Mouse.cpp \
	../../engine/input/MouseDevice.cpp \
	../../engine/input/Touchpad.cpp \
	../../engine/input/TouchpadDevice.cpp \
    ../../engine/localization/Localization.cpp \
    ../../engine/math/MathUtils.cpp \
    ../../engine/math/Matrix.cpp \
    ../../engine/network/Client.cpp \
    ../../engine/network/Network.cpp \
	../../engine/network/Server.cpp \
    ../../engine/scene/Actor.cpp \
    ../../engine/scene/Animator.cpp \
	../../engine/scene/Animators.cpp \
    ../../engine/scene/Camera.cpp \
    ../../engine/scene/Component.cpp \
    ../../engine/scene/Layer.cpp \
    ../../engine/scene/Light.cpp \
    ../../engine/scene/ParticleSystem.cpp \
    ../../engine/scene/Scene.cpp \
    ../../engine/scene/SceneManager.cpp \
    ../../engine/scene/ShapeRenderer.cpp \
    ../../engine/scene/SkinnedMeshRenderer.cpp \
    ../../engine/scene/SpriteRenderer.cpp \
    ../../engine/scene/StaticMeshRenderer.cpp \
    ../../engine/scene/TextRenderer.cpp \
    ../../engine/storage/FileSystem.cpp \
    ../../engine/utils/Log.cpp \
    ../../engine/utils/Obf.cpp \
    ../../engine/utils/Utils.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)