// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#import <QuartzCore/QuartzCore.h>
#include "OpenGLView.h"

@implementation OpenGLView

-(BOOL)wantsLayer
{
    return YES;
}

-(id)makeBackingLayer
{
    return [CALayer layer];
}

@end

#endif
