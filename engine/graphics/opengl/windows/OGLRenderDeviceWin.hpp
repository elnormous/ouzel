// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP

#include "../../../core/Setup.h"

#if defined(_WIN32) && OUZEL_COMPILE_OPENGL

#include <atomic>
#include <thread>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "../OGLRenderDevice.hpp"
#include "../../../thread/Thread.hpp"

namespace ouzel::graphics::opengl::windows
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        void start() final;

    private:
        void present() final;
        void renderMain();

        HDC deviceContext = 0;
        HGLRC renderContext = 0;

        std::atomic_bool running{false};
        thread::Thread renderThread;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEWIN_HPP
