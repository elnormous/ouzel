// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GLES2/gl2.h>

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
                emscripten_webgl_destroy_context(webGLContext);
            }
        }

        bool RendererOGLEm::init(Window* newWindow,
                                 const Size2& newSize,
                                 uint32_t newSampleCount,
                                 Texture::Filter newTextureFilter,
                                 PixelFormat newBackBufferFormat,
                                 bool newVerticalSync,
                                 bool newDepth)
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
                return false;
            }

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth);
        }

        bool RendererOGLEm::lockContext()
        {
            EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(webGLContext);

            if (res != EMSCRIPTEN_RESULT_SUCCESS)
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
