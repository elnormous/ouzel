// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#ifdef __OBJC__
#  import <AppKit/NSOpenGL.h>
using NSOpenGLContextPtr = NSOpenGLContext*;
using NSOpenGLPixelFormatPtr = NSOpenGLPixelFormat*;
#else
#  include <objc/objc.h>
using NSOpenGLContextPtr = id;
using NSOpenGLPixelFormatPtr = id;
#endif

#include "../OGLRenderDevice.hpp"
#include "../../../events/EventHandler.hpp"
#include "../../../platform/corevideo/DisplayLink.hpp"

namespace ouzel::graphics::opengl::macos
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        std::vector<math::Size<std::uint32_t, 2>> getSupportedResolutions() const final;
        void start() final;

        auto getContext() const noexcept { return context; }

        void renderCallback();

    private:
        void resizeFrameBuffer() final;
        void present() final;

        void changeScreen(const std::uintptr_t) final;

        NSOpenGLContextPtr context = nil;
        NSOpenGLPixelFormatPtr pixelFormat = nil;

        platform::corevideo::DisplayLink displayLink;

        std::atomic_bool running{false};
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
