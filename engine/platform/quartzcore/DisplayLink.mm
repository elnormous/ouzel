// Ouzel by Elviss Strazdins

#include <stdexcept>
#include "DisplayLink.hpp"

@interface DisplayLinkHandler: NSObject
@end

@implementation DisplayLinkHandler
{
    RenderCallback callback;
    void* userInfo;
}

- (id)initWithCallback:(RenderCallback)initCallback andUserInfo:(void*)initUserInfo
{
    if (self = [super init])
    {
        callback = initCallback;
        userInfo = initUserInfo;
    }

    return self;
}

- (void)draw:(__unused CADisplayLink*)sender
{
    callback(userInfo);
}
@end

namespace ouzel::platform::quartzcore
{
    DisplayLink::~DisplayLink()
    {
        running = false;
        if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
        if (renderThread.isJoinable()) renderThread.join();
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
                throw std::runtime_error("Failed to create display link");

            [displayLink setFrameInterval:1.0F];
        }

        running = true;

        renderThread = thread::Thread(&DisplayLink::renderMain, this);
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

    void DisplayLink::renderMain()
    {
        thread::setCurrentThreadName("Render");

        if (verticalSync)
        {
            runLoop = [NSRunLoop currentRunLoop];
            [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
            [runLoop run];
        }
        else
        {
            while (running)
                callback(userInfo);
        }
    }
}
