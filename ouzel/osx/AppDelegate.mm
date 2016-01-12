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
    
    ouzel::EventHandlerPtr eventHandler = std::make_shared<ouzel::EventHandler>();
    eventHandler->windowTitleChangeHandler = [self](const ouzel::WindowEvent& event, ouzel::VoidPtr const&) -> bool {
        
        _window.title = [NSString stringWithCString:event.title.c_str() encoding:NSASCIIStringEncoding];
        return true;
    };
    
    eventHandler->windowSizeChangeHandler = [self](const ouzel::WindowEvent& event, ouzel::VoidPtr const&) -> bool {
        
        NSRect frame = [NSWindow contentRectForFrameRect:[_window frame]
                                                       styleMask:[[self window] styleMask]];
        
        NSRect newFrame = [NSWindow frameRectForContentRect:
                           NSMakeRect(NSMinX(frame), NSMaxY(frame) - event.size.height, event.size.width, event.size.height)
                           styleMask:[_window styleMask]];
        
        [_window setFrame:newFrame display:YES animate:[_window isVisible]];
        
        return true;
    };
    ouzel::Engine::getInstance()->getEventDispatcher()->addEventHandler(eventHandler);
    
    NSRect windowFrame = [NSWindow contentRectForFrameRect:[[self window] frame]
                                                 styleMask:[[self window] styleMask]];
    
    OpenGLView* view = [[OpenGLView alloc] initWithFrame:windowFrame];
    
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
