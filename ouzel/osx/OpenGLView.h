// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Cocoa/Cocoa.h>
#include <CoreVideo/CoreVideo.h>

@interface OpenGLView : NSView
{
@private
    NSOpenGLContext* _openGLContext;
    NSOpenGLPixelFormat* _pixelFormat;
    
    CVDisplayLinkRef _displayLink;
    BOOL _running;
    BOOL _resized;
}

-(id)initWithFrame:(NSRect)frameRect;

-(void)prepareOpenGL;
-(void)close;

-(void)draw;

-(void)setOpenGLContext:(NSOpenGLContext*)context;
-(NSOpenGLContext*)openGLContext;
-(void)clearGLContext;

-(void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat;
-(NSOpenGLPixelFormat*)pixelFormat;

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
