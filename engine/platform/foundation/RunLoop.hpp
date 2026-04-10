#ifndef FLUXION_PLATFORM_FOUNDATION_RUNLOOP_HPP
#define FLUXION_PLATFORM_FOUNDATION_RUNLOOP_HPP

#include <functional>
#ifdef __OBJC__
#  import <Foundation/NSRunLoop.h>
using NSRunLoopPtr = NSRunLoop*;
#else
#  include <objc/NSObjCRuntime.h>
using NSRunLoopPtr = id;
#endif

namespace fluxion::platform::foundation
{
    class RunLoop final
    {
    public:
        RunLoop() noexcept = default;
        RunLoop(NSRunLoopPtr initRunLoop) noexcept;
        ~RunLoop();
        RunLoop(const RunLoop& other) noexcept;
        RunLoop(RunLoop&& other) noexcept;
        RunLoop& operator=(const RunLoop& other) noexcept;
        RunLoop& operator=(RunLoop&& other) noexcept;

        [[nodiscard]] bool operator==(const RunLoop& other) const noexcept
        {
            return runLoop == other.runLoop;
        }

        [[nodiscard]] bool operator!=(const RunLoop& other) const noexcept
        {
            return runLoop != other.runLoop;
        }

        [[nodiscard]] operator NSRunLoopPtr() const noexcept { return runLoop; }

        void run() const noexcept;
        void stop() const noexcept;
        [[nodiscard]] bool isRunning() const noexcept;
        void performFunction(std::function<void()> function) const noexcept;

    private:
        NSRunLoopPtr runLoop = nil;
    };

    [[nodiscard]] RunLoop mainRunLoop() noexcept;
    [[nodiscard]] RunLoop currentRunLoop() noexcept;
}

#endif // FLUXION_PLATFORM_FOUNDATION_RUNLOOP_HPP
