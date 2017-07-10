// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#include "MetalView.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "input/Input.h"
#include "graphics/metal/RendererMetal.h"

@implementation MetalView

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        self.opaque          = YES;
        self.backgroundColor = nil;

        CAMetalLayer* metalLayer = (CAMetalLayer*)self.layer;

        metalLayer.edgeAntialiasingMask = 0;
        metalLayer.masksToBounds = YES;
        metalLayer.presentsWithTransaction = NO;
        metalLayer.anchorPoint = CGPointMake(0.5, 0.5);
        metalLayer.frame = frameRect;
        metalLayer.magnificationFilter = kCAFilterNearest;
        metalLayer.minificationFilter = kCAFilterNearest;
        metalLayer.framebufferOnly = NO;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];

        if (!displayLink)
        {
            return Nil;
        }

        [displayLink setFrameInterval:1.0f];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }

    return self;
}

-(void)dealloc
{
    [displayLink invalidate];
    [displayLink release];

    [super dealloc];
}

+(Class)layerClass
{
    return [CAMetalLayer class];
}

-(void)draw:(__unused id)sender
{
    if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
    {
        // iOS app should not be exited
    }
}

-(void)touchesBegan:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchBegin(reinterpret_cast<uint64_t>(touch),
                                                    ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                       static_cast<float>(location.y))));
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchMove(reinterpret_cast<uint64_t>(touch),
                                                   ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                      static_cast<float>(location.y))));
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchEnd(reinterpret_cast<uint64_t>(touch),
                                                  ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                     static_cast<float>(location.y))));
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(__unused ::UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        ouzel::sharedEngine->getInput()->touchCancel(reinterpret_cast<uint64_t>(touch),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(static_cast<float>(location.x),
                                                                                                                                        static_cast<float>(location.y))));
    }
}

@end

#endif
