// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#import <QuartzCore/QuartzCore.h>
#include "graphics/Renderer.hpp"

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

-(id)initWithRenderer:(ouzel::graphics::Renderer*)newRenderer andVerticalSync:(bool)newVerticalSync;
-(void)stop;

@end
