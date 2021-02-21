// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if OUZEL_COMPILE_METAL

#include "MetalView.h"

@implementation MetalView

- (id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        self.opaque = YES;
        self.backgroundColor = nil;

        CAMetalLayer* metalLayer = (CAMetalLayer*)self.layer;

        metalLayer.edgeAntialiasingMask = 0;
        metalLayer.masksToBounds = YES;
        metalLayer.presentsWithTransaction = NO;
        metalLayer.anchorPoint = CGPointMake(0.5, 0.5);
        metalLayer.frame = frameRect;
        metalLayer.magnificationFilter = kCAFilterNearest;
        metalLayer.minificationFilter = kCAFilterNearest;
        metalLayer.framebufferOnly = NO;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    }

    return self;
}

+ (Class)layerClass
{
    return [CAMetalLayer class];
}
@end

#endif
