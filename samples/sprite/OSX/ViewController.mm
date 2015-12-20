// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "ViewController.h"

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    _application = new ouzel::Application();
    NSView* nativeView = _application->getEngine()->getRenderer()->getView()->getNativeView();
    
    [self.view addSubview:nativeView];
}

- (void)setRepresentedObject:(id)representedObject
{
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

@end
