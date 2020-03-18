// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP

#include "core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include "graphics/opengl/OGLRenderDevice.hpp"
#include "core/tvos/DisplayLink.hpp"

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
        namespace opengl
        {
            class RenderDeviceTVOS final: public RenderDevice
            {
                friend Renderer;
            public:
                explicit RenderDeviceTVOS(const std::function<void(const Event&)>& initCallback);
                ~RenderDeviceTVOS() override;

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
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
