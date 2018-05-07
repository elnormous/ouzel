// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "DisplayLink.hpp"
#include "utils/Log.hpp"

@interface DisplayLinkHandler: NSObject
{
    ouzel::graphics::RenderDevice* renderDevice;
}
@end

@implementation DisplayLinkHandler

-(id)initWithRenderDevice:(ouzel::graphics::RenderDevice*)initRenderDevice
{
    if (self = [super init])
    {
        renderDevice = initRenderDevice;
    }

    return self;
}

-(void)draw:(__unused CADisplayLink*)sender
{
    @autoreleasepool
    {
        renderDevice->process();
    }
}

@end

namespace ouzel
{
    DisplayLink::~DisplayLink()
    {
        running = false;
        if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
        if (renderThread.isJoinable()) renderThread.join();
        [displayLink invalidate];
        [displayLink release];
    }

    bool DisplayLink::start(bool initVerticalSync)
    {
        verticalSync = initVerticalSync;

        if (verticalSync)
        {
            DisplayLinkHandler* displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderDevice:&renderDevice];

            displayLink = [CADisplayLink displayLinkWithTarget:displayLinkHandler selector:@selector(draw:)];
            if (!displayLink)
            {
                ouzel::Log(ouzel::Log::Level::ERR) << "Failed to create display link";
                return false;
            }
            [displayLink setFrameInterval:1.0F];
        }

        running = true;

        renderThread = Thread(std::bind(&DisplayLink::main, this), "Render");

        return true;
    }

    bool DisplayLink::stop()
    {
        running = false;
        if (runLoop)
        {
            CFRunLoopStop([runLoop getCFRunLoop]);
            runLoop = nil;
        }

        return true;
    }

    void DisplayLink::main()
    {
        if (verticalSync)
        {
            runLoop = [NSRunLoop currentRunLoop];
            [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
            [runLoop run];
        }
        else
        {
            while (running)
            {
                @autoreleasepool
                {
                    renderDevice.process();
                }
            }
        }
    }
}
