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
        eaglLayer = (CAEAGLLayer*)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if (!context)
        {
            ouzel::log("Failed to initialize OpenGLES 2.0 context");
            return Nil;
        }

        if (![EAGLContext setCurrentContext:context])
        {
            ouzel::log("Failed to set current OpenGL context");
            return Nil;
        }

        // render buffer
        glGenRenderbuffers(1, &colorRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight);

        // frame buffer
        glGenFramebuffers(1, &frameBuffer);
        graphics::RendererOGL::bindFrameBuffer(frameBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, colorRenderBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            ouzel::log("Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return Nil;
        }

        if (![EAGLContext setCurrentContext:context])
        {
            ouzel::log("Failed to set current OpenGL context");
        }

        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];
        [displayLink setFrameInterval:1.0f];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        std::shared_ptr<graphics::RendererOGL> renderer = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());
        renderer->setFrameBuffer(frameBuffer);
    }

    return self;
}

-(void)dealloc
{
    [displayLink invalidate];
    [displayLink release];

    if ([EAGLContext currentContext] == context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [context release];

    if (frameBuffer) glDeleteFramebuffers(1, &frameBuffer);
    frameBuffer = 0;
    if (colorRenderBuffer) glDeleteRenderbuffers(1, &colorRenderBuffer);
    colorRenderBuffer = 0;

    [super dealloc];
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)idle:(id)sender
{
    if (![EAGLContext setCurrentContext:context])
    {
        ouzel::log("Failed to set current OpenGL context");
    }

    sharedEngine->run();

    [context presentRenderbuffer:GL_RENDERBUFFER];
}

@end
