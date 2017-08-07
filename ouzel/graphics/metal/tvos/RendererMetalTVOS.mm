// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_TVOS && OUZEL_SUPPORTS_METAL

#import <QuartzCore/QuartzCore.h>
#include "RendererMetalTVOS.hpp"
#include "MetalView.h"
#include "core/tvos/WindowTVOS.hpp"
#include "utils/Log.hpp"

@interface DisplayLinkHandler: NSObject
{
    ouzel::graphics::Renderer* renderer;
    NSConditionLock* runLock;
    NSRunLoop* runLoop;
    CADisplayLink* displayLink;
    NSThread* renderThread;
}

@end

@implementation DisplayLinkHandler

-(id)initWithRenderer:(ouzel::graphics::Renderer*)newRenderer
{
    if (self = [super init])
    {
        renderer = newRenderer;

        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
        if (!displayLink)
        {
            ouzel::Log(ouzel::Log::Level::ERR) << "Failed to create display link";
            return Nil;
        }
        [displayLink setFrameInterval:1.0f];

        runLock = [[NSConditionLock alloc] initWithCondition:0];
        renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(runThread) object:nil];
        [renderThread start];
    }

    return self;
}

-(void)dealloc
{
    [runLock lockWhenCondition:1];
    [runLock unlock];
    [runLock release];
    [displayLink invalidate];
    [displayLink release];
    [renderThread release];
    [super dealloc];
}

-(void)draw:(__unused CADisplayLink*)sender
{
    @autoreleasepool
    {
        renderer->process();
    }
}

-(void)runThread
{
    [runLock lock];

    runLoop = [NSRunLoop currentRunLoop];
    [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
    [runLoop run];

    [runLock unlockWithCondition:1];
}

-(void)stop
{
    [self performSelector:@selector(exit) onThread:renderThread withObject:nil waitUntilDone:NO];
}

-(void)exit
{
    CFRunLoopStop([runLoop getCFRunLoop]);
}

@end

namespace ouzel
{
    namespace graphics
    {
        RendererMetalTVOS::~RendererMetalTVOS()
        {
            if (displayLinkHandler) [displayLinkHandler stop];
            flushCommands();
            if (displayLinkHandler) [displayLinkHandler dealloc];
        }

        bool RendererMetalTVOS::init(Window* newWindow,
                                     const Size2& newSize,
                                     uint32_t newSampleCount,
                                     Texture::Filter newTextureFilter,
                                     uint32_t newMaxAnisotropy,
                                     bool newVerticalSync,
                                     bool newDepth,
                                     bool newDebugRenderer)
        {
            if (!RendererMetal::init(newWindow,
                                     newSize,
                                     newSampleCount,
                                     newTextureFilter,
                                     newMaxAnisotropy,
                                     newVerticalSync,
                                     newDepth,
                                     newDebugRenderer))
            {
                return false;
            }

            MetalView* view = (MetalView*)static_cast<WindowTVOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            metalLayer.contentsScale = newWindow->getContentScale();
            CGSize drawableSize = CGSizeMake(newSize.v[0], newSize.v[1]);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderer:this];

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
