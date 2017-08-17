// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_IOS && OUZEL_SUPPORTS_METAL

#import <QuartzCore/QuartzCore.h>
#include "RendererMetalIOS.hpp"
#include "MetalView.h"
#include "core/ios/WindowIOS.hpp"
#include "utils/Log.hpp"

@interface DisplayLinkHandler: NSObject
{
    ouzel::graphics::Renderer* renderer;
    NSConditionLock* runLock;
    NSRunLoop* runLoop;
    CADisplayLink* displayLink;
    NSThread* renderThread;
    bool verticalSync;
    bool running;
}

@end

@implementation DisplayLinkHandler

-(id)initWithRenderer:(ouzel::graphics::Renderer*)newRenderer andVerticalSync:(bool)newVerticalSync
{
    if (self = [super init])
    {
        renderer = newRenderer;
        verticalSync = newVerticalSync;
        running = true;

        if (verticalSync)
        {
            displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
            if (!displayLink)
            {
                ouzel::Log(ouzel::Log::Level::ERR) << "Failed to create display link";
                return nil;
            }
            [displayLink setFrameInterval:1.0f];
        }

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

    if (verticalSync)
    {
        runLoop = [NSRunLoop currentRunLoop];
        [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];
        [runLoop run];
    }
    else
    {
        while (running) [self draw:nil];
    }

    [runLock unlockWithCondition:1];
}

-(void)stop
{
    [self performSelector:@selector(exit) onThread:renderThread withObject:nil waitUntilDone:NO];
}

-(void)exit
{
    running = false;
    if (runLoop) CFRunLoopStop([runLoop getCFRunLoop]);
}

@end

namespace ouzel
{
    namespace graphics
    {
        RendererMetalIOS::~RendererMetalIOS()
        {
            if (displayLinkHandler) [displayLinkHandler stop];
            flushCommands();
            if (displayLinkHandler) [displayLinkHandler dealloc];
        }

        bool RendererMetalIOS::init(Window* newWindow,
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

            MetalView* view = (MetalView*)static_cast<WindowIOS*>(newWindow)->getNativeView();

            metalLayer = (CAMetalLayer*)view.layer;
            metalLayer.device = device;
            metalLayer.contentsScale = newWindow->getContentScale();
            CGSize drawableSize = CGSizeMake(newSize.width, newSize.height);
            metalLayer.drawableSize = drawableSize;

            colorFormat = metalLayer.pixelFormat;

            displayLinkHandler = [[DisplayLinkHandler alloc] initWithRenderer:this andVerticalSync:newVerticalSync];

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
