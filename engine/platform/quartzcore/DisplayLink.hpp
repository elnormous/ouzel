// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP
#define OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP

#include <functional>
#include <stdexcept>
#ifdef __OBJC__
#  import <QuartzCore/CADisplayLink.h>
using CADisplayLinkPtr = CADisplayLink*;
#else
#  include <objc/objc.h>
using CADisplayLinkPtr = id;
#endif

#include "../foundation/RunLoop.hpp"

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
        DisplayLink(std::function<void()> callback);
        ~DisplayLink();

        DisplayLink(const DisplayLink& other) noexcept;
        DisplayLink& operator=(const DisplayLink& other) noexcept;
        DisplayLink(DisplayLink&& other) noexcept;
        DisplayLink& operator=(DisplayLink&& other) noexcept;

        void setFrameInterval(std::uint32_t frameInterval);
        void addToRunLoop(const foundation::RunLoop& runLoop) const noexcept;
        void removeFromRunLoop(const foundation::RunLoop& runLoop) const noexcept;

    private:
        void renderMain();

        CADisplayLinkPtr displayLink = nil;
    };
}

#endif // OUZEL_PLATFORM_QUARTZCORE_DISPLAYLINK_HPP
