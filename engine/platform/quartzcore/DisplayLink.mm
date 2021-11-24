// Ouzel by Elviss Strazdins

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
    DisplayLink::DisplayLink(RenderCallback callback, void* userInfo)
    {
        DisplayLinkHandler* displayLinkHandler = [[[DisplayLinkHandler alloc] initWithCallback:callback andUserInfo:userInfo] autorelease];

        displayLink = [CADisplayLink displayLinkWithTarget:displayLinkHandler selector:@selector(draw:)];
        if (!displayLink)
            throw DisplayLinkError{"Failed to create display link"};

        [displayLink setFrameInterval:1.0F];
    }

    DisplayLink::~DisplayLink()
    {
        if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
        if (displayLink)
        {
            [displayLink invalidate];
            [displayLink release];
        }
    }

    void DisplayLink::start()
    {
        runLoop = [NSRunLoop currentRunLoop];
        [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
        [runLoop run];
    }

    void DisplayLink::stop()
    {
        if (runLoop)
        {
            CFRunLoopStop([runLoop getCFRunLoop]);
            runLoop = nil;
        }
    }
}
