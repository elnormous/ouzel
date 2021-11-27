// Ouzel by Elviss Strazdins

#include "RunLoop.hpp"

namespace ouzel::platform::foundation
{
    RunLoop::RunLoop() noexcept:
        runLoop{[[NSRunLoop currentRunLoop] retain]}
    {
    }

    RunLoop::~RunLoop()
    {
        [runLoop release];
    }

    RunLoop::RunLoop(const RunLoop& other) noexcept:
        runLoop{[other.runLoop retain]}
    {
    }

    RunLoop::RunLoop(RunLoop&& other) noexcept:
        runLoop{other.runLoop}
    {
        other.runLoop = nil;
    }

    RunLoop& RunLoop::operator=(const RunLoop& other) noexcept
    {
        if (&other == this) return *this;
        [runLoop release];
        runLoop = [other.runLoop retain];
        return *this;
    }

    RunLoop& RunLoop::operator=(RunLoop&& other) noexcept
    {
        if (&other == this) return *this;
        [runLoop release];
        runLoop = other.runLoop;
        other.runLoop = nil;
        return *this;
    }

    void RunLoop::run() const noexcept
    {
        [runLoop run];
    }

    void RunLoop::stop() const noexcept
    {
        if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
    }

    bool RunLoop::isRunning() const noexcept
    {
        return [runLoop currentMode] != nil;
    }
}
