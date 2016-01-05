// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "Engine.h"
#include "RendererOGL.h"
#include "EventDispatcher.h"
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
            NSLog(@"Failed to initialize OpenGLES 2.0 context");
            exit(1);
        }
        
        [self makeContextCurrent];
        
        [self setupRenderBuffer];
        [self setupFrameBuffer];
        
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];
        [_displayLink setFrameInterval: 1.0f];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        RendererOGL* renderer = static_cast<RendererOGL*>(Renderer::getInstance());
        renderer->initOpenGL(backingWidth, backingHeight);
    }
    
    return self;
}

-(void)dealloc
{
    if ([EAGLContext currentContext] == _context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    
    if (_frameBuffer) glDeleteFramebuffers(1, &_frameBuffer);
    _frameBuffer = 0;
    if (_colorRenderBuffer) glDeleteRenderbuffers(1, &_colorRenderBuffer);
    _colorRenderBuffer = 0;
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)makeContextCurrent
{
    if (![EAGLContext setCurrentContext:_context])
    {
        NSLog(@"Failed to set current OpenGL context");
        exit(1);
    }
}

-(void)setupRenderBuffer
{
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
}

-(void)setupFrameBuffer
{
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, _colorRenderBuffer);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        log("Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

-(void)idle:(id)sender
{
    [self makeContextCurrent];
    
    Engine::getInstance()->run();
    
    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        Event event;
        event.type = Event::Type::TOUCH_BEGIN;
        event.touchEvent.touchId = reinterpret_cast<uint64_t>(touch);
        event.touchEvent.position = Renderer::getInstance()->viewToScreenLocation(Vector2(location.x, location.y));

        EventDispatcher::getInstance()->dispatchEvent(event);
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        Event event;
        event.type = Event::Type::TOUCH_MOVE;
        event.touchEvent.touchId = reinterpret_cast<uint64_t>(touch);
        event.touchEvent.position = Renderer::getInstance()->viewToScreenLocation(Vector2(location.x, location.y));
        
        EventDispatcher::getInstance()->dispatchEvent(event);
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        Event event;
        event.type = Event::Type::TOUCH_END;
        event.touchEvent.touchId = reinterpret_cast<uint64_t>(touch);
        event.touchEvent.position = Renderer::getInstance()->viewToScreenLocation(Vector2(location.x, location.y));
        
        EventDispatcher::getInstance()->dispatchEvent(event);
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        Event event;
        event.type = Event::Type::TOUCH_CANCEL;
        event.touchEvent.touchId = reinterpret_cast<uint64_t>(touch);
        event.touchEvent.position = Renderer::getInstance()->viewToScreenLocation(Vector2(location.x, location.y));
        
        EventDispatcher::getInstance()->dispatchEvent(event);
    }
}

@end
