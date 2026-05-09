#ifndef FLUXION_GRAPHICS_OGLRENDERDEVICEEM_HPP
#define FLUXION_GRAPHICS_OGLRENDERDEVICEEM_HPP

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && FLUXION_COMPILE_OPENGL

#include <emscripten.h>
#include <emscripten/html5.h>
#include "../OGLRenderDevice.hpp"

namespace fluxion::graphics::opengl::emscripten
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        void start() final {}

        void handleContextLost();
        void handleContextRestored();

    private:
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webGLContext = 0;
    };
}
#endif

#endif // FLUXION_GRAPHICS_OGLRENDERDEVICEEM_HPP
