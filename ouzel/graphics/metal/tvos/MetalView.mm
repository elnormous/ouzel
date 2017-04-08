// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MetalView.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "graphics/metal/RendererMetal.h"
#include "utils/Utils.h"

@interface ViewDelegate: NSObject<MTKViewDelegate>

@end

@implementation ViewDelegate

-(void)mtkView:(nonnull __unused MTKView *)view drawableSizeWillChange:(__unused CGSize)size
{
    // this is handled by window size change handler
}

-(void)drawInMTKView:(nonnull __unused MTKView*)view
{
    if (ouzel::sharedEngine->isRunning() && !ouzel::sharedEngine->draw())
    {
        // tvOS app should not be exited
    }
}

@end

@implementation MetalView
{
    id<MTKViewDelegate> viewDelegate;
}

-(void)insertText:(__unused NSString*)text
{
}

-(void)deleteBackward
{
}

-(BOOL)hasText
{
    return NO;
}

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        viewDelegate = [[ViewDelegate alloc] init];
        self.delegate = viewDelegate;
    }

    return self;
}

-(void)dealloc
{
    self.delegate = Nil;
    [viewDelegate release];

    [super dealloc];
}

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

@end
