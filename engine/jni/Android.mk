LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(shell (if [ ! -f $(LOCAL_PATH)/../Config.h ]; then \
    cp $(LOCAL_PATH)/../DefaultConfig.h $(LOCAL_PATH)/../Config.h; \
fi))

LOCAL_MODULE := ouzel
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -Wall -Wpedantic -Wextra -Wshadow -Wdouble-promotion
LOCAL_WHOLE_STATIC_LIBRARIES += cpufeatures
LOCAL_CPPFLAGS += -Wold-style-cast -std=c++17 -fexceptions
LOCAL_PCH := ../Prefix.pch
LOCAL_C_INCLUDES += $(LOCAL_PATH)/.. \
    $(LOCAL_PATH)/../../shaders \
    $(LOCAL_PATH)/../../external/khronos \
    $(LOCAL_PATH)/../../external/smbPitchShift \
    $(LOCAL_PATH)/../../external/stb

LOCAL_SRC_FILES := ../assets/Bundle.cpp \
    ../assets/Cache.cpp \
    ../assets/ImageLoader.cpp \
    ../assets/MtlLoader.cpp \
    ../assets/ObjLoader.cpp \
    ../assets/WaveLoader.cpp \
    ../audio/mixer/Bus.cpp \
    ../audio/mixer/Mixer.cpp \
    ../audio/opensl/OSLAudioDevice.cpp \
    ../audio/Audio.cpp \
    ../audio/AudioDevice.cpp \
    ../audio/Containers.cpp \
    ../audio/Cue.cpp \
    ../audio/Effect.cpp \
    ../audio/Effects.cpp \
    ../audio/Listener.cpp \
    ../audio/Mix.cpp \
    ../audio/Node.cpp \
    ../audio/Oscillator.cpp \
    ../audio/PcmClip.cpp \
    ../audio/SilenceSound.cpp \
    ../audio/Sound.cpp \
    ../audio/Submix.cpp \
    ../audio/Voice.cpp \
    ../audio/VorbisClip.cpp \
    ../core/android/EngineAndroid.cpp \
    ../core/android/NativeWindowAndroid.cpp \
    ../core/android/SystemAndroid.cpp \
    ../core/Engine.cpp \
    ../core/NativeWindow.cpp \
    ../core/System.cpp \
    ../core/Window.cpp \
    ../events/EventDispatcher.cpp \
    ../graphics/opengl/android/OGLRenderDeviceAndroid.cpp \
    ../graphics/opengl/OGLBlendState.cpp \
    ../graphics/opengl/OGLBuffer.cpp \
    ../graphics/opengl/OGLDepthStencilState.cpp \
    ../graphics/opengl/OGLRenderDevice.cpp \
    ../graphics/opengl/OGLRenderTarget.cpp \
    ../graphics/opengl/OGLShader.cpp \
    ../graphics/opengl/OGLTexture.cpp \
    ../graphics/renderer/Renderer.cpp \
    ../graphics/BlendState.cpp \
    ../graphics/Buffer.cpp \
    ../graphics/DepthStencilState.cpp \
    ../graphics/Graphics.cpp \
    ../graphics/RenderDevice.cpp \
    ../graphics/RenderTarget.cpp \
    ../graphics/Shader.cpp \
    ../graphics/Texture.cpp \
    ../gui/BMFont.cpp \
    ../gui/TTFont.cpp \
    ../gui/Widgets.cpp \
    ../input/android/GamepadDeviceAndroid.cpp \
    ../input/android/InputSystemAndroid.cpp \
    ../input/Cursor.cpp \
    ../input/Gamepad.cpp \
    ../input/GamepadDevice.cpp \
    ../input/InputDevice.cpp \
    ../input/InputManager.cpp \
    ../input/InputSystem.cpp \
    ../input/Keyboard.cpp \
    ../input/KeyboardDevice.cpp \
    ../input/Mouse.cpp \
    ../input/MouseDevice.cpp \
    ../input/Touchpad.cpp \
    ../input/TouchpadDevice.cpp \
    ../localization/Localization.cpp \
    ../network/Client.cpp \
    ../network/Network.cpp \
    ../network/Server.cpp \
    ../scene/Actor.cpp \
    ../scene/Animator.cpp \
    ../scene/Animators.cpp \
    ../scene/Camera.cpp \
    ../scene/Component.cpp \
    ../scene/Layer.cpp \
    ../scene/Light.cpp \
    ../scene/ParticleSystem.cpp \
    ../scene/Scene.cpp \
    ../scene/SceneManager.cpp \
    ../scene/ShapeRenderer.cpp \
    ../scene/SkinnedMeshRenderer.cpp \
    ../scene/SpriteRenderer.cpp \
    ../scene/StaticMeshRenderer.cpp \
    ../scene/TextRenderer.cpp \
    ../storage/FileSystem.cpp \
    ../utils/Log.cpp

include $(BUILD_STATIC_LIBRARY)
$(call import-module, android/cpufeatures)