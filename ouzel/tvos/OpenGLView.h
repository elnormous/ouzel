// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface OpenGLView : UIView
{
@private
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
}

-(id)initWithFrame:(CGRect)frameRect;

@end
