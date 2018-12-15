// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_DISPLAYLINK_HPP
#define OUZEL_CORE_DISPLAYLINK_HPP

#include <functional>
#include <thread>
#if defined(__OBJC__)
#import <QuartzCore/QuartzCore.h>
typedef CADisplayLink* CADisplayLinkPtr;
typedef NSRunLoop* NSRunLoopPtr;
#else
#include <objc/objc.h>
typedef id CADisplayLinkPtr;
typedef id NSRunLoopPtr;
#endif

#include "graphics/RenderDevice.hpp"

typedef void (*RenderCallback)(void*);

namespace ouzel
{
    class DisplayLink final
    {
    public:
        DisplayLink(RenderCallback initCallback, void* initUserInfo):
            callback(initCallback), userInfo(initUserInfo), running(false)
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
        void main();

        RenderCallback callback;
        void* userInfo;
        std::thread renderThread;
        std::atomic_bool running{false};
        bool verticalSync = false;
        CADisplayLinkPtr displayLink = nil;
        NSRunLoopPtr runLoop = nil;
    };
}

#endif // OUZEL_CORE_DISPLAYLINK_HPP
