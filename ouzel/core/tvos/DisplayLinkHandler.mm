// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "DisplayLinkHandler.h"
#include "utils/Log.hpp"

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
