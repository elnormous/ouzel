// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.hpp"

#if OUZEL_SUPPORTS_OPENGL

#include "OpenGLView.h"
#include "core/Engine.hpp"
#include "core/Window.hpp"

@implementation OpenGLView

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];

        if (!displayLink)
        {
            return Nil;
        }

        [displayLink setFrameInterval:1.0f];

        renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(runThread) object:nil];
        [renderThread start];
    }

    return self;
}

-(void)dealloc
{
    [self performSelector:@selector(stop) onThread:renderThread withObject:nil waitUntilDone:YES];

    [displayLink invalidate];
    [displayLink release];

    [super dealloc];
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)runThread
{
    runLoop = [NSRunLoop currentRunLoop];

    [displayLink addToRunLoop:runLoop forMode:NSDefaultRunLoopMode];

    [runLoop run];
}

-(void)stop
{
    CFRunLoopStop([runLoop getCFRunLoop]);
}

-(void)draw:(__unused id)sender
{
    @autoreleasepool
    {
        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->getRenderer()->process();
        }
    }
}

@end

#endif
