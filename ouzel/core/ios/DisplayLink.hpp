// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

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
#include "thread/Thread.hpp"

namespace ouzel
{
    class DisplayLink
    {
    public:
        DisplayLink(ouzel::graphics::RenderDevice& initRenderDevice):
            renderDevice(initRenderDevice), running(false)
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

        ouzel::graphics::RenderDevice& renderDevice;
        Thread renderThread;
        std::atomic<bool> running;
        bool verticalSync = false;
        CADisplayLinkPtr displayLink = nil;
        NSRunLoopPtr runLoop = nil;
    };
}
