// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "DisplayLink.hpp"
#include "utils/Errors.hpp"
#include "utils/Utils.hpp"

@interface DisplayLinkHandler: NSObject
{
    RenderCallback callback;
    void* userInfo;
}
@end

@implementation DisplayLinkHandler

-(id)initWithCallback:(RenderCallback)initCallback andUserInfo:(void*)initUserInfo
{
    if (self = [super init])
    {
        callback = initCallback;
        userInfo = initUserInfo;
    }

    return self;
}

-(void)draw:(__unused CADisplayLink*)sender
{
    @autoreleasepool
    {
        callback(userInfo);
    }
}

@end

namespace ouzel
{
    DisplayLink::~DisplayLink()
    {
        running = false;
        if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
        if (renderThread.joinable()) renderThread.join();
        if (displayLink)
        {
            [displayLink invalidate];
            [displayLink release];
        }
    }

    void DisplayLink::start(bool initVerticalSync)
    {
        verticalSync = initVerticalSync;

        if (verticalSync)
        {
            DisplayLinkHandler* displayLinkHandler = [[DisplayLinkHandler alloc] initWithCallback:callback andUserInfo:userInfo];

            displayLink = [CADisplayLink displayLinkWithTarget:displayLinkHandler selector:@selector(draw:)];
            if (!displayLink)
                throw SystemError("Failed to create display link");

            [displayLink setFrameInterval:1.0F];
        }

        running = true;

        renderThread = std::thread(&DisplayLink::main, this);
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
        setCurrentThreadName("Render");

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
                callback(userInfo);
            }
        }
    }
}
