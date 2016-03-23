// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Cocoa/Cocoa.h>
#include <CoreVideo/CoreVideo.h>

@interface OpenGLView : NSOpenGLView
{
@private
    CVDisplayLinkRef _displayLink;
    CGDirectDisplayID _displayId;
    BOOL _running;
    BOOL _resized;
}

-(id)initWithFrame:(NSRect)frameRect;

-(void)prepareOpenGL;
-(void)close;
-(void)changeDisplay;

-(void)draw;

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
