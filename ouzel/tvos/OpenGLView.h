// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface OpenGLView: UIView
{
@private
    CAEAGLLayer* eaglLayer;
    EAGLContext* context;
    GLuint colorRenderBuffer;
    GLuint frameBuffer;

    CADisplayLink* displayLink;
}

-(id)initWithFrame:(CGRect)frameRect;

@property (readonly) GLint backingWidth;
@property (readonly) GLint backingHeight;

@end
