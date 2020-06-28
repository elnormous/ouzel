// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "../OGLRenderDevice.hpp"

namespace ouzel::graphics::opengl::emscripten
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Renderer;
    public:
        explicit RenderDevice(const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

    private:
        void init(core::Window* newWindow,
                  const Size2U& newSize,
                  std::uint32_t newSampleCount,
                  bool newSrgb,
                  bool newVerticalSync,
                  bool newDepth,
                  bool newStencil,
                  bool newDebugRenderer) final;

        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
