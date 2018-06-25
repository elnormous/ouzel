// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "DisplayLink.hpp"
#include "utils/Errors.hpp"

@interface DisplayLinkHandler: NSObject
{
    ouzel::graphics::RenderDevice* renderDevice;
}
@end

@implementation DisplayLinkHandler

-(id)initWithRenderDevice:(ouzel::graphics::RenderDevice*)initRenderDevice
{
    if (self = [super init])
        renderDevice = initRenderDevice;

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

    void DisplayLink::start(bool initVerticalSync)
    {
        verticalSync = initVerticalSync;

        if (verticalSync)
        {
            DisplayLinkHandler* displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderDevice:&renderDevice];

            displayLink = [CADisplayLink displayLinkWithTarget:displayLinkHandler selector:@selector(draw:)];
            if (!displayLink)
                throw SystemError("Failed to create display link");

            [displayLink setFrameInterval:1.0F];
        }

        running = true;

        renderThread = Thread(std::bind(&DisplayLink::main, this), "Render");
    }

    void DisplayLink::stop()
    {
        running = false;
        if (runLoop)
        {
            CFRunLoopStop([runLoop getCFRunLoop]);
            runLoop = nil;
        }
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
