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
        _eaglLayer = (CAEAGLLayer*)self.layer;
        _eaglLayer.opaque = YES;
        _eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                         [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if (!_context)
        {
            ouzel::log("Failed to initialize OpenGLES 2.0 context");
            return Nil;
        }

        if (![EAGLContext setCurrentContext:_context])
        {
            ouzel::log("Failed to set current OpenGL context");
            return Nil;
        }

        // render buffer
        glGenRenderbuffers(1, &_colorRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];

        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight);

        // frame buffer
        glGenFramebuffers(1, &_frameBuffer);
        graphics::RendererOGL::bindFrameBuffer(_frameBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, _colorRenderBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            ouzel::log("Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return Nil;
        }

        if (![EAGLContext setCurrentContext:_context])
        {
            ouzel::log("Failed to set current OpenGL context");
        }

        // display link
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];
        [_displayLink setFrameInterval:1.0f];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        std::shared_ptr<graphics::RendererOGL> renderer = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());
        renderer->setFrameBuffer(_frameBuffer);
    }

    return self;
}

-(void)dealloc
{
    [_displayLink invalidate];
    [_displayLink release];

    if ([EAGLContext currentContext] == _context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [_context release];

    if (_frameBuffer) glDeleteFramebuffers(1, &_frameBuffer);
    _frameBuffer = 0;
    if (_colorRenderBuffer) glDeleteRenderbuffers(1, &_colorRenderBuffer);
    _colorRenderBuffer = 0;

    [super dealloc];
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)idle:(id)sender
{
    if (![EAGLContext setCurrentContext:_context])
    {
        ouzel::log("Failed to set current OpenGL context");
    }

    sharedEngine->run();

    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

@end
