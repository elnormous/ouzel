// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    _application = new ouzel::Application();
    void* nativeView = _application->getEngine()->getRenderer()->getView()->getNativeView();
    
    NSView* openGLView = (__bridge NSView*)nativeView;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    _application->release();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

@end
