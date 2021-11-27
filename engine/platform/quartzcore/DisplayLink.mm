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
    DisplayLink::DisplayLink(RenderCallback callback, void* userInfo):
        displayLink{[CADisplayLink displayLinkWithTarget:[[[DisplayLinkHandler alloc] initWithCallback:callback
                                                                                           andUserInfo:userInfo] autorelease]
                                                selector:@selector(draw:)]}
    {
        if (!displayLink)
            throw DisplayLinkError{"Failed to create display link"};

        [displayLink setFrameInterval:1.0F];
    }

    DisplayLink::~DisplayLink()
    {
        if (displayLink)
        {
            [displayLink invalidate];
            [displayLink release];
        }
    }

    void DisplayLink::addToRunLoop(const foundation::RunLoop& runLoop) const noexcept
    {
        [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
    }

    void DisplayLink::removeFromRunLoop(const foundation::RunLoop& runLoop) const noexcept
    {
        [displayLink removeFromRunLoop:runLoop forMode:NSDefaultRunLoopMode];
    }
}
