// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP

#include "core/Setup.h"

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

#include "graphics/opengl/OGLRenderDevice.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDeviceMacOS final: public OGLRenderDevice
        {
            friend Renderer;
        public:
            OGLRenderDeviceMacOS(const std::function<void(const Event&)>& initCallback);
            ~OGLRenderDeviceMacOS();

            std::vector<Size2<uint32_t>> getSupportedResolutions() const override;

            inline NSOpenGLContextPtr getOpenGLContext() const { return openGLContext; }

            void renderCallback();

        protected:
            void init(Window* newWindow,
                      const Size2<uint32_t>& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            void setSize(const Size2<uint32_t>& newSize) override;
            void present() override;

            bool handleWindow(const WindowEvent& event);

            NSOpenGLContextPtr openGLContext = nil;
            NSOpenGLPixelFormatPtr pixelFormat = nil;

            CVDisplayLinkRef displayLink = nullptr;
            EventHandler eventHandler;

            std::atomic_bool running{false};
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEMACOS_HPP
