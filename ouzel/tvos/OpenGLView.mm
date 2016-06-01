// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "Engine.h"
#include "RendererOGL.h"
#include "Utils.h"

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];

        if (!displayLink)
        {
            return Nil;
        }

        float frameInterval = 1.0f;

        if (sharedEngine->getTargetFPS() > 60.0f)
        {
            log("FPS bigger that 60.0f is not supported");
        }
        else if (sharedEngine->getTargetFPS() > 0.0f)
        {
            frameInterval = 60.0f / sharedEngine->getTargetFPS();
        }

        [displayLink setFrameInterval:frameInterval];
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

-(void)idle:(id)sender
{
    sharedEngine->run();
}

@end
