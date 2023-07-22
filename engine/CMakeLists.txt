add_library(ouzel
      assets/Bundle.cpp 
      assets/Cache.cpp 
      assets/ImageLoader.cpp 
      assets/MtlLoader.cpp 
      assets/ObjLoader.cpp 
      assets/WaveLoader.cpp 
      audio/mixer/Bus.cpp 
      audio/mixer/Mixer.cpp 
      audio/Audio.cpp 
      audio/AudioDevice.cpp 
      audio/Containers.cpp 
      audio/Cue.cpp 
      audio/Effect.cpp 
      audio/Effects.cpp 
      audio/Listener.cpp 
      audio/Mix.cpp 
      audio/Node.cpp 
      audio/Oscillator.cpp 
      audio/PcmClip.cpp 
      audio/SilenceSound.cpp 
      audio/Sound.cpp 
      audio/Submix.cpp 
      audio/Voice.cpp 
      audio/VorbisClip.cpp 
      core/Engine.cpp 
      core/System.cpp 
      core/NativeWindow.cpp 
      core/Window.cpp 
      events/EventDispatcher.cpp 
      graphics/opengl/OGLBlendState.cpp 
      graphics/opengl/OGLBuffer.cpp 
      graphics/opengl/OGLDepthStencilState.cpp 
      graphics/opengl/OGLRenderDevice.cpp 
      graphics/opengl/OGLRenderTarget.cpp 
      graphics/opengl/OGLShader.cpp 
      graphics/opengl/OGLTexture.cpp 
      graphics/BlendState.cpp 
      graphics/Buffer.cpp 
      graphics/DepthStencilState.cpp 
      graphics/Graphics.cpp 
      graphics/RenderDevice.cpp 
      graphics/RenderPass.cpp 
      graphics/RenderTarget.cpp 
      graphics/Shader.cpp 
      graphics/Texture.cpp 
      gui/BMFont.cpp 
      gui/TTFont.cpp 
      gui/Widgets.cpp 
      input/Cursor.cpp 
      input/Gamepad.cpp 
      input/GamepadDevice.cpp 
      input/InputDevice.cpp 
      input/InputManager.cpp 
      input/InputSystem.cpp 
      input/Keyboard.cpp 
      input/KeyboardDevice.cpp 
      input/Mouse.cpp 
      input/MouseDevice.cpp 
      input/Touchpad.cpp 
      input/TouchpadDevice.cpp 
      localization/Localization.cpp 
      network/Client.cpp 
      network/Network.cpp 
      network/Server.cpp 
      scene/Actor.cpp 
      scene/Animator.cpp 
      scene/Animators.cpp 
      scene/Camera.cpp 
      scene/Component.cpp 
      scene/Layer.cpp 
      scene/Light.cpp 
      scene/ParticleSystem.cpp 
      scene/Scene.cpp 
      scene/SceneManager.cpp 
      scene/ShapeRenderer.cpp 
      scene/SkinnedMeshRenderer.cpp 
      scene/SpriteRenderer.cpp 
      scene/StaticMeshRenderer.cpp 
      scene/TextRenderer.cpp 
      storage/FileSystem.cpp 
      utils/Log.cpp
)

if(WIN32)
      target_precompile_headers(ouzel PUBLIC stdafx.pch)
else()
      target_precompile_headers(ouzel PUBLIC Prefix.pch)
endif()
target_include_directories(ouzel PUBLIC
      ${EXTERNAL_LIB_INCLUDE_DIRS}
      ${SHADER_DIRS}
      ${CMAKE_CURRENT_LIST_DIR}
)

if(WIN32)
      target_sources(ouzel
            PRIVATE
            core/windows/EngineWin.cpp 
            core/windows/NativeWindowWin.cpp 
            core/windows/SystemWin.cpp 
            graphics/direct3d11/D3D11BlendState.cpp 
            graphics/direct3d11/D3D11Buffer.cpp 
            graphics/direct3d11/D3D11DepthStencilState.cpp 
            graphics/direct3d11/D3D11RenderDevice.cpp 
            graphics/direct3d11/D3D11Shader.cpp 
            graphics/direct3d11/D3D11Texture.cpp 
            graphics/opengl/windows/OGLRenderDeviceWin.cpp 
            input/windows/GamepadDeviceDI.cpp 
            input/windows/GamepadDeviceWin.cpp 
            input/windows/GamepadDeviceXI.cpp 
            input/windows/InputSystemWin.cpp 
            input/windows/KeyboardDeviceWin.cpp 
            input/windows/MouseDeviceWin.cpp 
            input/windows/CursorWin.cpp
      )
      target_compile_definitions(ouzel PRIVATE -DWINVER=0x0601 -D_WIN32_WINNT=0x0601)
elseif(UNIX AND NOT APPLE) # linux
      target_sources(ouzel PRIVATE
            audio/alsa/ALSAAudioDevice.cpp 
            audio/openal/OALAudioDevice.cpp 
            core/linux/EngineLinux.cpp 
            core/linux/NativeWindowLinux.cpp 
            core/linux/SystemLinux.cpp 
            graphics/opengl/linux/OGLRenderDeviceLinux.cpp 
            input/linux/EventDevice.cpp 
            input/linux/InputSystemLinux.cpp 
            input/linux/KeyboardDeviceLinux.cpp 
            input/linux/MouseDeviceLinux.cpp 
            input/linux/CursorLinux.cpp
      )
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
            set(VC_DIR /opt/vc)
            target_include_directories(ouzel PUBLIC 
                  ${VC_DIR}/include
                  ${VC_DIR}/include/interface/vcos/pthreads
                  ${VC_DIR}/interface/vmcs_host/linux
            )
      endif()
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin") # macos
      target_sources(ouzel PRIVATE 
            audio/coreaudio/CAAudioDevice.mm 
            audio/openal/OALAudioDevice.cpp 
            core/macos/EngineMacOS.mm 
            core/macos/NativeWindowMacOS.mm 
            core/macos/SystemMacOS.mm 
            core/macos/ViewMacOS.mm 
            graphics/metal/macos/MetalView.m 
            graphics/metal/macos/MetalRenderDeviceMacOS.mm 
            graphics/metal/MetalBlendState.mm 
            graphics/metal/MetalBuffer.mm 
            graphics/metal/MetalDepthStencilState.mm 
            graphics/metal/MetalRenderDevice.mm 
            graphics/metal/MetalRenderTarget.mm 
            graphics/metal/MetalShader.mm 
            graphics/metal/MetalTexture.mm 
            graphics/opengl/macos/OGLRenderDeviceMacOS.mm 
            graphics/opengl/macos/OpenGLView.m 
            input/macos/GamepadDeviceGC.mm 
            input/macos/GamepadDeviceIOKit.cpp 
            input/macos/GamepadDeviceMacOS.cpp 
            input/macos/InputSystemMacOS.mm 
            input/macos/MouseDeviceMacOS.mm 
            input/macos/CursorMacOS.mm
            platform/cocoa/Window.mm 
            platform/foundation/RunLoop.mm
      )
elseif(IOS)
      target_sources(ouzel PRIVATE 
            audio/coreaudio/CAAudioDevice.mm 
            audio/openal/OALAudioDevice.cpp 
            core/ios/EngineIOS.mm 
            core/ios/NativeWindowIOS.mm 
            core/ios/SystemIOS.mm 
            core/ios/ViewIOS.mm 
            graphics/metal/ios/MetalView.m 
            graphics/metal/ios/MetalRenderDeviceIOS.mm 
            graphics/metal/MetalBlendState.mm 
            graphics/metal/MetalBuffer.mm 
            graphics/metal/MetalDepthStencilState.mm 
            graphics/metal/MetalRenderDevice.mm 
            graphics/metal/MetalRenderTarget.mm 
            graphics/metal/MetalShader.mm 
            graphics/metal/MetalTexture.mm 
            graphics/opengl/ios/OGLRenderDeviceIOS.mm 
            graphics/opengl/ios/OpenGLView.m 
            input/ios/GamepadDeviceIOS.mm 
            input/ios/InputSystemIOS.mm 
            platform/foundation/RunLoop.mm 
            platform/quartzcore/DisplayLink.mm
      )
      execute_process(
            COMMAND xcrun --sdk iphoneos --show-sdk-path
            OUTPUT_VARIABLE IOS_SYS_ROOT
      )
      target_compile_options(ouzel 
            PRIVATE 
            -arch arm64 
            -isysroot 
            ${IOS_SYS_ROOT}
            -miphoneos-version-min=8.0)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "tvos")
      target_sources(ouzel PRIVATE 
            audio/coreaudio/CAAudioDevice.mm 
            audio/openal/OALAudioDevice.cpp 
            core/tvos/EngineTVOS.mm 
            core/tvos/NativeWindowTVOS.mm 
            core/tvos/SystemTVOS.mm 
            core/tvos/ViewTVOS.mm 
            graphics/metal/tvos/MetalView.m 
            graphics/metal/tvos/MetalRenderDeviceTVOS.mm 
            graphics/metal/MetalBlendState.mm 
            graphics/metal/MetalBuffer.mm 
            graphics/metal/MetalDepthStencilState.mm 
            graphics/metal/MetalRenderDevice.mm 
            graphics/metal/MetalRenderTarget.mm 
            graphics/metal/MetalShader.mm 
            graphics/metal/MetalTexture.mm 
            graphics/opengl/tvos/OGLRenderDeviceTVOS.mm 
            graphics/opengl/tvos/OpenGLView.m 
            input/tvos/GamepadDeviceTVOS.mm 
            input/tvos/InputSystemTVOS.mm 
            platform/foundation/RunLoop.mm 
            platform/quartzcore/DisplayLink.mm
      )
      execute_process(
            COMMAND xcrun --sdk appletvos --show-sdk-path
            OUTPUT_VARIABLE TVOS_SYS_ROOT
      )
      target_compile_options(ouzel 
            PRIVATE 
            -arch arm64 
            -isysroot 
            ${TVOS_SYS_ROOT}
            -mtvos-version-min=9.0)
elseif(EMSCRIPTEN)
      target_sources(ouzel PRIVATE 
            audio/openal/OALAudioDevice.cpp 
            core/emscripten/EngineEm.cpp 
            core/emscripten/NativeWindowEm.cpp 
            core/emscripten/SystemEm.cpp 
            graphics/opengl/emscripten/OGLRenderDeviceEm.cpp 
            input/emscripten/GamepadDeviceEm.cpp 
            input/emscripten/InputSystemEm.cpp 
            input/emscripten/MouseDeviceEm.cpp
      )
      target_compile_options(ouzel PRIVATE -s DISABLE_EXCEPTION_CATCHING=0)
endif()
