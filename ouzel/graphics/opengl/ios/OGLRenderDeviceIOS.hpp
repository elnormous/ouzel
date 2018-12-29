// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICEIOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICEIOS_HPP

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGLRenderDevice.hpp"
#include "core/ios/DisplayLink.hpp"

#if defined(__OBJC__)
#  import <UIKit/UIKit.h>
typedef EAGLContext* EAGLContextPtr;
typedef CAEAGLLayer* CAEAGLLayerPtr;
#else
#  include <objc/objc.h>
typedef id EAGLContextPtr;
typedef id CAEAGLLayerPtr;
#endif

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDeviceIOS final: public OGLRenderDevice
        {
            friend Renderer;
        public:
            OGLRenderDeviceIOS(const std::function<void(const Event&)>& initCallback);
            ~OGLRenderDeviceIOS();

            void renderCallback();

        private:
            void init(Window* newWindow,
                      const Size2& newSize,
                      uint32_t newSampleCount,
                      Texture::Filter newTextureFilter,
                      uint32_t newMaxAnisotropy,
                      bool newVerticalSync,
                      bool newDepth,
                      bool newDebugRenderer) override;

            void setSize(const Size2& newSize) override;
            void present() override;

            void createFrameBuffer();

            EAGLContextPtr context = nil;
            CAEAGLLayerPtr eaglLayer = nil;

            GLuint msaaFrameBufferId = 0;
            GLuint msaaColorRenderBufferId = 0;

            GLuint resolveFrameBufferId = 0;
            GLuint resolveColorRenderBufferId = 0;

            GLuint depthRenderBufferId = 0;

            DisplayLink displayLink;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICEIOS_HPP
