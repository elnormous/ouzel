// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface OpenGLView: UIView
{
@private
    CADisplayLink* displayLink;
}

-(id)initWithFrame:(CGRect)frameRect;

@end

#endif
