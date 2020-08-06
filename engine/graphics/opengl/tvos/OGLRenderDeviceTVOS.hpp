// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP

#include "../../../core/Setup.h"

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include "../OGLRenderDevice.hpp"
#include "../../../core/tvos/DisplayLink.hpp"

#if defined(__OBJC__)
#  import <UIKit/UIKit.h>
typedef EAGLContext* EAGLContextPtr;
typedef CAEAGLLayer* CAEAGLLayerPtr;
#else
#  include <objc/objc.h>
typedef id EAGLContextPtr;
typedef id CAEAGLLayerPtr;
#endif

namespace ouzel::graphics::opengl::tvos
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        explicit RenderDevice(const Settings& settings,
                              core::Window& initWindow,
                              const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void renderCallback();

    private:
        void init(const Settings& settings) final;

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

        core::tvos::DisplayLink displayLink;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
