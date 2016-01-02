// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface OpenGLView : UIView
{
@private
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _frameBuffer;
    
    CADisplayLink* _displayLink;
}

-(id)initWithFrame:(CGRect)frameRect;

@end
