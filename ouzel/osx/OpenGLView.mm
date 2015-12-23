// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include "Engine.h"
#include "RendererOGL.h"

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(NSRect)frameRect engine:(ouzel::Engine*)engine
{
    self = [super initWithFrame:frameRect];
    if (self != nil)
    {
        _engine = engine;
        _renderer = _engine->getRenderer();
        
        NSTimer *updateTimer = [NSTimer timerWithTimeInterval:1.0f/60.0f target:self selector:@selector(idle:) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:updateTimer forMode:NSDefaultRunLoopMode];
        
        // Create pixel format
        NSOpenGLPixelFormatAttribute attributes[] =
        {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core, // ensure we're using 3.2
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 32, // set depth buffer size
            0
        };
        
        _pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        // Create OpenGL context
        _openGLContext = [[NSOpenGLContext alloc] initWithFormat:_pixelFormat shareContext:NULL];
        [_openGLContext setView:self];
        [_openGLContext makeCurrentContext];
    }
    
    return self;
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    
    [self update];
}

-(void)update
{
    _renderer->resize(Size2(_frame.size.width, _frame.size.height));
    
    [_openGLContext update];
}

-(void)lockFocus
{
    [super lockFocus];
    if ([_openGLContext view] != self)
    {
        [_openGLContext setView:self];
    }
    
    [self makeOpenGLContextCurrent];
}

-(void)prepareOpenGL
{
    RendererOGL* renderer = static_cast<RendererOGL*>(_renderer);
    renderer->initOpenGL(_frame.size.width, _frame.size.height);
}

-(void)idle:(NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

-(void)drawRect:(NSRect)bounds
{
    [_openGLContext makeCurrentContext];
    
    _engine->run();
    
    [_openGLContext flushBuffer];
}

-(void)setOpenGLContext:(NSOpenGLContext*)context
{
    _openGLContext = context;
}
-(NSOpenGLContext*)openGLContext
{
    return _openGLContext;
}
-(void)clearGLContext
{
    [_openGLContext clearDrawable];
}

-(void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat
{
    _pixelFormat = pixelFormat;
}
-(NSOpenGLPixelFormat*)pixelFormat
{
    return _pixelFormat;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

void updateModifiers(NSEvent* theEvent, Event& event)
{
    event.shiftDown = theEvent.modifierFlags & NSShiftKeyMask;
    event.controlDown = theEvent.modifierFlags & NSControlKeyMask;
    event.commandDown = theEvent.modifierFlags & NSCommandKeyMask;
    event.functionDown = theEvent.modifierFlags & NSFunctionKeyMask;
}

-(void)makeOpenGLContextCurrent
{
    [_openGLContext makeCurrentContext];
}

-(void)keyDown:(NSEvent*)theEvent
{
    Event event;
    event.type = Event::Type::KEY_DOWN;
    event.key = theEvent.keyCode;
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)keyUp:(NSEvent*)theEvent
{
    Event event;
    event.type = Event::Type::KEY_UP;
    event.key = theEvent.keyCode;
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseButton = 0;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseButton = 0;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseButton = 1;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseButton = 1;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseDown:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DOWN;
    event.mouseButton = 2;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseUp:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_UP;
    event.mouseButton = 2;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseMoved:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_MOVE;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)mouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseButton = 0;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)rightMouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseButton = 1;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)otherMouseDragged:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_DRAG;
    event.mouseButton = 2;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)scrollWheel:(NSEvent*)theEvent
{
    NSPoint location = theEvent.locationInWindow;
    
    Event event;
    event.type = Event::Type::MOUSE_SCROLL;
    event.mousePosition = _renderer->absoluteToWorldLocation(Vector2(location.x, location.y));
    event.mouseScroll = Vector2(theEvent.scrollingDeltaX, theEvent.scrollingDeltaY);
    updateModifiers(theEvent, event);
    
    _engine->handleEvent(event);
}

-(void)swipeWithEvent:(NSEvent*)theEvent
{
    
}

-(void)rotateWithEvent:(NSEvent*)theEvent
{
    
}

-(void)magnifyWithEvent:(NSEvent*)theEvent
{
    
}

@end
