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
        
        // render buffer
        glGenRenderbuffers(1, &_colorRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        // frame buffer
        glGenFramebuffers(1, &_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, _colorRenderBuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            log("Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }
        
        // display link
        _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];
        [_displayLink setFrameInterval: 1.0f];
        [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        std::shared_ptr<RendererOGL> renderer = std::static_pointer_cast<RendererOGL>(Engine::getInstance()->getRenderer());
        renderer->initOpenGL(backingWidth, backingHeight, _frameBuffer);
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
        
        TouchEvent event;
        event.type = Event::Type::TOUCH_BEGIN;
        event.touchId = reinterpret_cast<uint64_t>(touch);
        event.position = Engine::getInstance()->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y));

        Engine::getInstance()->getEventDispatcher()->dispatchTouchBeginEvent(event, Engine::getInstance()->getInput());
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        TouchEvent event;
        event.type = Event::Type::TOUCH_MOVE;
        event.touchId = reinterpret_cast<uint64_t>(touch);
        event.position = Engine::getInstance()->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y));
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchMoveEvent(event, Engine::getInstance()->getInput());
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        TouchEvent event;
        event.type = Event::Type::TOUCH_END;
        event.touchId = reinterpret_cast<uint64_t>(touch);
        event.position = Engine::getInstance()->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y));
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchEndEvent(event, Engine::getInstance()->getInput());
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];
        
        TouchEvent event;
        event.type = Event::Type::TOUCH_CANCEL;
        event.touchId = reinterpret_cast<uint64_t>(touch);
        event.position = Engine::getInstance()->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y));
        
        Engine::getInstance()->getEventDispatcher()->dispatchTouchCancelEvent(event, Engine::getInstance()->getInput());
    }
}

@end
