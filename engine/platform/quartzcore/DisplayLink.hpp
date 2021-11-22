// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP
#define OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP

#include <stdexcept>
#ifdef __OBJC__
#  import <QuartzCore/QuartzCore.h>
typedef CADisplayLink* CADisplayLinkPtr;
typedef NSRunLoop* NSRunLoopPtr;
#else
#  include <objc/objc.h>
typedef id CADisplayLinkPtr;
typedef id NSRunLoopPtr;
#endif

#include "../../graphics/RenderDevice.hpp"
#include "../../thread/Thread.hpp"

using RenderCallback = void (*)(void*);

namespace ouzel::platform::quartzcore
{
    class DisplayLinkError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class DisplayLink final
    {
    public:
        DisplayLink(RenderCallback initCallback, void* initUserInfo):
            callback{initCallback}, userInfo{initUserInfo}, running{false}
        {
        }

        ~DisplayLink();

        DisplayLink(const DisplayLink&) = delete;
        DisplayLink& operator=(const DisplayLink&) = delete;
        DisplayLink(DisplayLink&&) = delete;
        DisplayLink& operator=(DisplayLink&&) = delete;

        void start(bool initVerticalSync);
        void stop();

    private:
        void renderMain();

        RenderCallback callback;
        void* userInfo;
        thread::Thread renderThread;
        std::atomic_bool running{false};
        bool verticalSync = false;
        CADisplayLinkPtr displayLink = nil;
        NSRunLoopPtr runLoop = nil;
    };
}

#endif // OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP
