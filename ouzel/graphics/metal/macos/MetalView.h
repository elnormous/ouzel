// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_METALVIEW_H
#define OUZEL_GRAPHICS_METALVIEW_H

#include "core/Setup.h"

#if OUZEL_COMPILE_METAL

#import "core/macos/ViewMacOS.h"

@interface MetalView: ViewMacOS

@end

#endif

#endif // OUZEL_GRAPHICS_METALVIEW_H
