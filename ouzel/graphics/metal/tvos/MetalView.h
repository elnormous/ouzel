// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface MetalView: UIView
{
@private
    CADisplayLink* displayLink;
}

@end
