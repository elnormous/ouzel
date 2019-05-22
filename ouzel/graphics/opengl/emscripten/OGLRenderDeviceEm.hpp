// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP

#include "core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "graphics/opengl/OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDeviceEm final: public OGLRenderDevice
        {
            friend Renderer;
        public:
            OGLRenderDeviceEm(const std::function<void(const Event&)>& initCallback);
            ~OGLRenderDeviceEm();

        private:
            void init(Window* newWindow,
                      const Size2U& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newSrgb,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newStencil,
                      bool newDebugRenderer) final;

            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
