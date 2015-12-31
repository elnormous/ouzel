// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Renderer.h"
#include "Size2.h"

@interface AppDelegate ()
{
    ouzel::Engine* _engine;
}
@end

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    _engine = new ouzel::Engine();
    ouzel::Size2 size = ouzel::Renderer::getInstance()->getSize();
    
    NSScreen* screen = [NSScreen mainScreen];
    NSRect screenFrame = screen.frame;
    
    NSRect frame = NSMakeRect(screenFrame.size.width / 2 - size.width / 2,
                              screenFrame.size.height / 2 - size.height / 2,
                              size.width, size.height);
    
    NSUInteger windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
    
    if (ouzel::Renderer::getInstance()->getResizable())
    {
        windowStyleMask |= NSResizableWindowMask;
    }
    
    _window  = [[NSWindow alloc] initWithContentRect:frame
                                           styleMask:windowStyleMask
                                             backing:NSBackingStoreBuffered
                                               defer:NO
                                              screen:screen];
    
    _window.acceptsMouseMovedEvents = YES;
    [_window setBackgroundColor:[NSColor blueColor]];
    
    [_window setTitle:[NSString stringWithUTF8String:ouzel::Renderer::getInstance()->getTitle().c_str()]];
    
    OpenGLView* view = [[OpenGLView alloc] initWithFrame:NSMakeRect(0, 0, 600, 600)];
    
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
