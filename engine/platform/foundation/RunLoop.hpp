// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_FOUNDATION_RUNLOOP_HPP
#define OUZEL_PLATFORM_FOUNDATION_RUNLOOP_HPP

#include <functional>
#ifdef __OBJC__
#  import <Foundation/NSRunLoop.h>
typedef NSRunLoop* NSRunLoopPtr;
#else
#  include <objc/NSObjCRuntime.h>
using NSRunLoopPtr = id;
#endif

namespace ouzel::platform::foundation
{
    class RunLoop final
    {
    public:
        RunLoop() noexcept;
        ~RunLoop();
        RunLoop(const RunLoop& other) noexcept;
        RunLoop(RunLoop&& other) noexcept;
        RunLoop& operator=(const RunLoop& other) noexcept;
        RunLoop& operator=(RunLoop&& other) noexcept;

        bool operator==(const RunLoop& other) const noexcept
        {
            return runLoop == other.runLoop;
        }

        bool operator!=(const RunLoop& other) const noexcept
        {
            return runLoop != other.runLoop;
        }

        operator NSRunLoopPtr() const noexcept { return runLoop; }

        void run() const noexcept;
        void stop() const noexcept;
        bool isRunning() const noexcept;
        void performFunction(std::function<void()> function) const noexcept;

    private:
        NSRunLoopPtr runLoop = nil;
    };
}

#endif // OUZEL_PLATFORM_FOUNDATION_RUNLOOP_HPP
