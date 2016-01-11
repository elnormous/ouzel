// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Renderer.h"
#include "Size2.h"

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    ouzel::Engine::getInstance()->init();
    ouzel::Size2 size = ouzel::Engine::getInstance()->getRenderer()->getSize();
    
    NSScreen* screen = [NSScreen mainScreen];
    NSRect screenFrame = screen.frame;
    
    NSRect frame = NSMakeRect(screenFrame.size.width / 2 - size.width / 2,
                              screenFrame.size.height / 2 - size.height / 2,
                              size.width, size.height);
    
    NSUInteger windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask;
    
    if (ouzel::Engine::getInstance()->getRenderer()->getResizable())
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
    
    [_window setTitle:[NSString stringWithUTF8String:ouzel::Engine::getInstance()->getRenderer()->getTitle().c_str()]];
    
    std::shared_ptr<ouzel::EventHandler> eventHandler = std::make_shared<ouzel::EventHandler>();
    eventHandler->windowTitleChangeHandler = [self](const ouzel::WindowEvent& event, std::shared_ptr<void> const&) -> bool {
        _window.title = [NSString stringWithCString:event.title.c_str() encoding:NSASCIIStringEncoding];
        return true;
    };
    ouzel::Engine::getInstance()->getEventDispatcher()->addEventHandler(eventHandler);
    
    OpenGLView* view = [[OpenGLView alloc] initWithFrame:NSMakeRect(0, 0, 600, 600)];
    
    [_window setContentView:view];
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [_window makeKeyAndOrderFront:self];
}

-(void)applicationWillTerminate:(NSNotification *)aNotification
{
    ouzel::Engine::getInstance()->end();
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

@end
