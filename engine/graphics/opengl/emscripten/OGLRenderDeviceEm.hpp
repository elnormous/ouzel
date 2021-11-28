// Ouzel by Elviss Strazdins

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
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void start() final {}

        void handleContextLost();
        void handleContextRestored();

    private:
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEEM_HPP
