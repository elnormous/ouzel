// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "RendererOGLEm.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLEm::~RendererOGLEm()
        {
            if (webGLContext)
            {
                if (emscripten_webgl_destroy_context(webGLContext) != EMSCRIPTEN_RESULT_SUCCESS)
                {
                    Log(Log::Level::ERR) << "Failed to destroy WebGL context";
                }
            }
        }

        bool RendererOGLEm::init(Window* newWindow,
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

            return RendererOGL::init(newWindow,
                                     newSize,
                                     newSampleCount,
                                     newTextureFilter,
                                     newMaxAnisotropy,
                                     newVerticalSync,
                                     newDepth,
                                     newDebugRenderer);
        }

        bool RendererOGLEm::lockContext()
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
