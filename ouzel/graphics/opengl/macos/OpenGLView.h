// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

@interface OpenGLView: NSView
{
@private
    CVDisplayLinkRef displayLink;
}

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
