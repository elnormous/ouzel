// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#if defined(__OBJC__)
#  import <CoreVideo/CoreVideo.h>
#  import <AppKit/NSOpenGL.h>
typedef NSOpenGLContext* NSOpenGLContextPtr;
typedef NSOpenGLPixelFormat* NSOpenGLPixelFormatPtr;
#else
#  include <objc/objc.h>
typedef void* CVDisplayLinkRef;
typedef id NSOpenGLContextPtr;
typedef id NSOpenGLPixelFormatPtr;
#endif

#include "../OGLRenderDevice.hpp"
#include "../../../events/EventHandler.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            class RenderDeviceMacOS final: public RenderDevice
            {
                friend Renderer;
            public:
                explicit RenderDeviceMacOS(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceMacOS() override;

                std::vector<Size2U> getSupportedResolutions() const final;

                inline auto getOpenGLContext() const noexcept { return openGLContext; }

                void renderCallback();

            private:
                void init(Window* newWindow,
                          const Size2U& newSize,
                          std::uint32_t newSampleCount,
                          SamplerFilter newTextureFilter,
                          std::uint32_t newMaxAnisotropy,
                          bool newSrgb,
                          bool newVerticalSync,
                          bool newDepth,
                          bool newStencil,
                          bool newDebugRenderer) final;

                void resizeFrameBuffer() final;
                void present() final;

                bool handleWindow(const WindowEvent& event);

                NSOpenGLContextPtr openGLContext = nil;
                NSOpenGLPixelFormatPtr pixelFormat = nil;

                CVDisplayLinkRef displayLink = nullptr;
                EventHandler eventHandler;

                std::atomic_bool running{false};
            };
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
