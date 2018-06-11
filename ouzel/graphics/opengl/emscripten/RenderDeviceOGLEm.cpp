// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_EMSCRIPTEN && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLEm.hpp"
#include "core/Engine.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLEm::~RenderDeviceOGLEm()
        {
            if (webGLContext)
            {
                if (emscripten_webgl_destroy_context(webGLContext) != EMSCRIPTEN_RESULT_SUCCESS)
                    Log(Log::Level::ERR) << "Failed to destroy WebGL context";
            }
        }

        bool RenderDeviceOGLEm::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            apiMajorVersion = 2;
            apiMinorVersion = 0;

            EmscriptenWebGLContextAttributes attrs;
            emscripten_webgl_init_context_attributes(&attrs);

            attrs.alpha = true;
            attrs.depth = newDepth;
            attrs.stencil = false;
            attrs.antialias = newSampleCount > 0;

            webGLContext = emscripten_webgl_create_context(0, &attrs);

            if (!webGLContext)
            {
                Log(Log::Level::ERR) << "Failed to create WebGL context";
                return false;
            }

            EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webGLContext);

            if (res != EMSCRIPTEN_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to make WebGL context current";
                return false;
            }

            emscripten_set_main_loop_timing(newVerticalSync ? EM_TIMING_RAF : EM_TIMING_SETTIMEOUT, 1);

            return RenderDeviceOGL::init(newWindow,
                                         newSize,
                                         newSampleCount,
                                         newTextureFilter,
                                         newMaxAnisotropy,
                                         newVerticalSync,
                                         newDepth,
                                         newDebugRenderer);
        }

        bool RenderDeviceOGLEm::lockContext()
        {
            EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webGLContext);

            if (res != EMSCRIPTEN_RESULT_SUCCESS)
            {
                Log(Log::Level::ERR) << "Failed to make WebGL context current";
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
