// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "Engine.h"

@interface AppDelegate ()
{
    ouzel::Engine* _engine;
}
@end

@implementation AppDelegate

-(id)init
{
    if (self = [super init])
    {
        NSRect frame = NSMakeRect(100, 100, 800, 600);
        
        NSUInteger windowStyleMask = NSTitledWindowMask | NSResizableWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
        
        _window  = [[NSWindow alloc] initWithContentRect:frame
                                               styleMask:windowStyleMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        
        [_window setBackgroundColor:[NSColor blueColor]];
        [_window makeKeyAndOrderFront:self];
        
        _engine = new ouzel::Engine();
    }
    
    return self;
}

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    OpenGLView* view = [[OpenGLView alloc] initWithFrame:NSMakeRect(0, 0, 600, 600) engine:_engine];
    
    [_window setContentView:view];
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [_window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)aNotification
{
    delete _engine;
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

@end
