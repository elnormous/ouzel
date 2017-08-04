// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_METAL

#import <QuartzCore/QuartzCore.h>
#import "core/tvos/ViewTVOS.h"

@interface MetalView: ViewTVOS
{
@private
    CADisplayLink* displayLink;
}

@end

#endif
