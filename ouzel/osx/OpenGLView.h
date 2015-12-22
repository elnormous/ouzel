// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Cocoa/Cocoa.h>

namespace ouzel
{
    class Engine;
    class Renderer;
}

@interface OpenGLView : NSView
{
@private
    ouzel::Engine* _engine;
    ouzel::Renderer* _renderer;
    NSOpenGLContext* _openGLContext;
    NSOpenGLPixelFormat* _pixelFormat;
}

-(id)initWithFrame:(NSRect)frameRect engine:(ouzel::Engine*)engine;

-(void)update;

-(void)prepareOpenGL;

-(void)drawRect:(NSRect)bounds;

-(void)setOpenGLContext:(NSOpenGLContext*)context;
-(NSOpenGLContext*)openGLContext;
-(void)clearGLContext;

-(void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat;
-(NSOpenGLPixelFormat*)pixelFormat;

-(void)makeOpenGLContextCurrent;

// input
-(BOOL)acceptsFirstResponder;

-(void)keyDown:(NSEvent*)event;

-(void)mouseDown:(NSEvent*)event;
-(void)mouseUp:(NSEvent*)event;
-(void)mouseMoved:(NSEvent *)event;
-(void)mouseDragged:(NSEvent*)event; // Mouse click-and-drag
-(void)scrollWheel:(NSEvent*)event; // Mouse scroll wheel movement

-(void)swipeWithEvent:(NSEvent*)event; // Trackpad swipe gesture
-(void)rotateWithEvent:(NSEvent*)event; // Trackpad twist gesture
-(void)magnifyWithEvent:(NSEvent*)event; // Trackpad pinch gesture

@end
