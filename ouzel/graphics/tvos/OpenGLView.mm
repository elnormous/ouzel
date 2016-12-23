// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "utils/Utils.h"

@implementation OpenGLView

-(void)insertText:(__unused NSString*)text
{
}

-(void)deleteBackward
{
}

-(BOOL)hasText
{
    return NO;
}

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

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)draw:(__unused id)sender
{
    if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
    {
        // tvOS app should not be exited
    }
}

@end
