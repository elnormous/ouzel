add_executable(samples
      AnimationsSample.cpp 
      GameSample.cpp 
      GUISample.cpp 
      InputSample.cpp 
      main.cpp 
      MainMenu.cpp 
      PerspectiveSample.cpp 
      SoundSample.cpp 
      SpritesSample.cpp 
      RTSample.cpp
)

target_link_libraries(samples PRIVATE ouzel)

set(SAMPLE_RESOURCES
      24-bit.wav
      8-bit.json
      8-bit.wav
      ambient.wav
      AmosisTechnik.ttf
      arrow.png
      assets.json
      ball.png
      cube.mtl
      cube.obj
      cube.png
      cursor.png
      fire.json
      fire.png
      flame.json
      flame.png
      floor.jpg
      gui.zip
      jump.wav
      lv.mo
      music.ogg
      run.json
      run.png
      triangle.json
      triangle.png
      witch.png
)

foreach(RESOURCE ${SAMPLE_RESOURCES})
      add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Resources/${RESOURCE}
            COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_LIST_DIR}/Resources/${RESOURCE} ${CMAKE_CURRENT_BINARY_DIR}/Resources/${RESOURCE}
            DEPENDS ${CMAKE_CURRENT_LIST_DIR}/Resources/${RESOURCE}
      )
      add_custom_target(ouzel-sample-copy-${RESOURCE}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Resources/${RESOURCE}
      )
      add_dependencies(samples ouzel-sample-copy-${RESOURCE})
endforeach()

if(WIN32)
      target_link_libraries(samples PRIVATE
            -ld3d11 
            -lopengl32 
            -ldxguid 
            -lxinput9_1_0 
            -lshlwapi 
            -lversion 
            -ldinput8 
            -luser32 
            -lgdi32 
            -lshell32 
            -lole32 
            -loleaut32 
            -luuid 
            -lws2_32 
      )
      target_link_options(samples PRIVATE "-entry WinMain")
elseif(UNIX AND NOT APPLE)
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm") # arm linux
            target_link_directories(samples PRIVATE /opt/vc/lib)
            target_link_libraries(samples PRIVATE 
                  -lbrcmGLESv2 
                  -lbrcmEGL 
                  -lbcm_host
            )
      else()
            target_link_libraries(samples PRIVATE 
                  -lGL 
                  -lEGL 
                  -lX11 
                  -lXcursor 
                  -lXss 
                  -lXi 
                  -lXxf86vm 
                  -lXrandr
            )
      endif()
      target_link_libraries(samples PRIVATE
            -lopenal 
            -lpthread 
            -lasound 
            -ldl
      )
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
      target_link_libraries(samples PRIVATE
            "-framework AudioToolbox"
            "-framework AudioUnit"
            "-framework Cocoa"
            "-framework CoreAudio"
            "-framework CoreVideo"
            "-framework GameController"
            "-framework IOKit"
            "-framework Metal"
            "-framework OpenAL"
            "-framework OpenGL"
            "-framework QuartzCore"
      )
elseif(IOS)
      target_link_libraries(samples PRIVATE
            "-framework AudioToolbox"
            "-framework AVFoundation"
            "-framework Foundation"
            "-framework GameController"
            "-framework Metal"
            "-framework OpenAL"
            "-framework OpenGLES"
            "-framework QuartzCore"
            "-framework UIKit"
      )
      execute_process(
            COMMAND xcrun --sdk iphoneos --show-sdk-path
            OUTPUT_VARIABLE IOS_SYS_ROOT
      )
      target_compile_options(samples 
            PRIVATE 
            -arch arm64 
            -isysroot 
            ${IOS_SYS_ROOT}
            -miphoneos-version-min=8.0)
elseif(CMAKE_SYSTEM_NAME STREQUAL "tvos")
      target_link_libraries(samples PRIVATE
            "-framework AudioToolbox"
            "-framework AVFoundation"
            "-framework Foundation"
            "-framework GameController"
            "-framework Metal"
            "-framework OpenAL"
            "-framework OpenGLES"
            "-framework QuartzCore"
            "-framework UIKit"
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
      target_link_options(samples PRIVATE
            -lopenal 
            --embed-file settings.ini 
            --embed-file Resources 
            -s DISABLE_EXCEPTION_CATCHING=0 
            -s TOTAL_MEMORY=134217728
      )
endif()
