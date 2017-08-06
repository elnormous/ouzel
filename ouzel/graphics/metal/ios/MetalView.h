// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.hpp"

#if OUZEL_SUPPORTS_METAL

#import <QuartzCore/QuartzCore.h>
#import "core/ios/ViewIOS.h"

@interface MetalView: ViewIOS
{
@private
    NSThread* renderThread;
    NSRunLoop* runLoop;
    CADisplayLink* displayLink;
}

@end

#endif
