// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "../OGLRenderDevice.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDeviceEm final: public RenderDevice
            {
                friend Renderer;
            public:
                explicit RenderDeviceEm(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceEm() override;

            private:
                void init(Window* newWindow,
                          const Size2U& newSize,
                          std::uint32_t newSampleCount,
                          bool newSrgb,
                          bool newVerticalSync,
                          bool newDepth,
                          bool newStencil,
                          bool newDebugRenderer) final;

                EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
