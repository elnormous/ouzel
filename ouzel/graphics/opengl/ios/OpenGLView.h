// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#import <QuartzCore/QuartzCore.h>
#import "core/ios/ViewIOS.h"

@interface OpenGLView: ViewIOS
{
@private
    CADisplayLink* displayLink;
}

-(id)initWithFrame:(CGRect)frameRect;

@end

#endif
