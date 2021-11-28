// Ouzel by Elviss Strazdins

#include "DisplayLink.hpp"

@interface DisplayLinkHandler: NSObject
@end

@implementation DisplayLinkHandler
{
    std::function<void()> callback;
}

- (id)initWithCallback:(std::function<void()>)initCallback
{
    if (self = [super init])
    {
        callback = std::move(initCallback);
    }

    return self;
}

- (void)draw:(__unused CADisplayLink*)sender
{
    callback();
}
@end

namespace ouzel::platform::quartzcore
{
    DisplayLink::DisplayLink(std::function<void()> callback):
        displayLink{[CADisplayLink displayLinkWithTarget:[[[DisplayLinkHandler alloc] initWithCallback:std::move(callback)] autorelease]
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
