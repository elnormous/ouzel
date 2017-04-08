// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/QuartzCore.h>

@interface MetalView: NSView
{
@private
    CVDisplayLinkRef displayLink;
    CGDirectDisplayID displayId;
}

@property (nonatomic, strong) CAMetalLayer* metalLayer;

@end
