// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "core/Engine.h"
#include "opengl/RendererOGL.h"
#include "input/Input.h"
#include "utils/Utils.h"

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

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)idle:(id)sender
{
    OUZEL_UNUSED(sender);

    sharedEngine->run();
}

-(void)touchesBegan:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchBegin(reinterpret_cast<uint64_t>(touch),
                                             sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchMove(reinterpret_cast<uint64_t>(touch),
                                            sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchEnd(reinterpret_cast<uint64_t>(touch),
                                           sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchCancel(reinterpret_cast<uint64_t>(touch),
                                              sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

@end
