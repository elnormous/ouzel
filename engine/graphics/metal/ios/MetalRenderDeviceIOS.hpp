// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
#define OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP

#include "../../../core/Setup.h"

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS && OUZEL_COMPILE_METAL

#include "../MetalRenderDevice.hpp"
#include "../../../thread/Thread.hpp"
#include "../../../platform/foundation/RunLoop.hpp"
#include "../../../platform/quartzcore/DisplayLink.hpp"

namespace ouzel::graphics::metal::ios
{
    class RenderDevice final: public metal::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        void start() final;
        void renderCallback();

    private:
        void renderMain();

        thread::Thread renderThread;
        std::atomic_bool running{false};
        std::mutex runLoopMutex;
        std::condition_variable runLoopCondition;
        bool started = false;
        platform::foundation::RunLoop runLoop;
        platform::quartzcore::DisplayLink displayLink;
    };
}

#endif

#endif // OUZEL_GRAPHICS_METALRENDERDEVICEIOS_HPP
