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
    DisplayLink::DisplayLink(std::function<void()> callback) noexcept:
        displayLink{[CADisplayLink displayLinkWithTarget:[[[DisplayLinkHandler alloc] initWithCallback:std::move(callback)] autorelease]
                                                selector:@selector(draw:)]}
    {
    }

    DisplayLink::~DisplayLink()
    {
        if (displayLink)
        {
            [displayLink invalidate];
            [displayLink release];
        }
    }

    DisplayLink::DisplayLink(const DisplayLink& other) noexcept:
        displayLink{[other.displayLink retain]}
    {
    }

    DisplayLink::DisplayLink(DisplayLink&& other) noexcept:
        displayLink{other.displayLink}
    {
        other.displayLink = nil;
    }

    DisplayLink& DisplayLink::operator=(const DisplayLink& other) noexcept
    {
        if (&other == this) return *this;
        [other.displayLink retain];
        [displayLink release];
        displayLink = other.displayLink;
        return *this;
    }

    DisplayLink& DisplayLink::operator=(DisplayLink&& other) noexcept
    {
        if (&other == this) return *this;
        [displayLink release];
        displayLink = other.displayLink;
        other.displayLink = nil;
        return *this;
    }

    void DisplayLink::setFrameInterval(const std::uint32_t frameInterval)
    {
        [displayLink setFrameInterval:frameInterval];
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
