// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "MetalView.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "graphics/metal/RendererMetal.h"
#include "utils/Utils.h"

using namespace ouzel;

@interface ViewDelegate: NSObject<MTKViewDelegate>

@end

@implementation ViewDelegate

-(void)mtkView:(nonnull __unused MTKView *)view drawableSizeWillChange:(CGSize)size
{
    sharedEngine->getRenderer()->setSize(ouzel::Size2(static_cast<float>(size.width),
                                                      static_cast<float>(size.height)));
}

-(void)drawInMTKView:(nonnull __unused MTKView*)view
{
    if (sharedEngine->isRunning() && !sharedEngine->draw())
    {
        // tvOS app should not be exited
    }
}

@end

@implementation MetalView
{
    id<MTKViewDelegate> viewDelegate;
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
    [viewDelegate release];
    viewDelegate = Nil;
    
    [super dealloc];
}

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

@end
