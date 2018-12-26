// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RENDERDEVICEOGLEM_HPP
#define OUZEL_GRAPHICS_RENDERDEVICEOGLEM_HPP

#include "core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "graphics/opengl/RenderDeviceOGL.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLEm final: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            RenderDeviceOGLEm(const std::function<void(const Event&)>& initCallback);
            ~RenderDeviceOGLEm();

        private:
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_RENDERDEVICEOGLEM_HPP
