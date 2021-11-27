// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_TV && OUZEL_COMPILE_OPENGL

#include "../OGLRenderDevice.hpp"
#include "../../../thread/Thread.hpp"
#include "../../../platform/foundation/RunLoop.hpp"
#include "../../../platform/quartzcore/DisplayLink.hpp"

#ifdef __OBJC__
#  import <UIKit/UIKit.h>
typedef EAGLSharegroup* EAGLSharegroupPtr;
typedef EAGLContext* EAGLContextPtr;
typedef CAEAGLLayer* CAEAGLLayerPtr;
#else
#  include <objc/objc.h>
typedef id EAGLSharegroupPtr;
typedef id EAGLContextPtr;
typedef id CAEAGLLayerPtr;
#endif

namespace ouzel::graphics::opengl::tvos
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        void renderCallback();

    private:
        void resizeFrameBuffer() final;
        void present() final;

        void createFrameBuffer();

        void renderMain();

        EAGLSharegroupPtr shareGroup = nil;
        EAGLContextPtr context = nil;
        CAEAGLLayerPtr eaglLayer = nil;

        GLuint msaaFrameBufferId = 0;
        GLuint msaaColorRenderBufferId = 0;

        GLuint resolveFrameBufferId = 0;
        GLuint resolveColorRenderBufferId = 0;

        GLuint depthRenderBufferId = 0;

        thread::Thread renderThread;
        std::atomic_bool running{false};
        std::mutex runLoopMutex;
        std::condition_variable runLoopCondition;
        platform::foundation::RunLoop runLoop;
        platform::quartzcore::DisplayLink displayLink;
    };
}
#endif

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICETVOS_HPP
