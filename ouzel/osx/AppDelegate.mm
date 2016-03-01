// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Renderer.h"
#include "Size2.h"

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    _fullscreen = NO;
    
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
    
    _window.backgroundColor = [NSColor blackColor];
    _window.acceptsMouseMovedEvents = YES;
    _window.delegate = self;
    [_window setBackgroundColor:[NSColor blueColor]];
    [_window setCollectionBehavior: NSWindowCollectionBehaviorFullScreenPrimary];
    
    if (ouzel::Engine::getInstance()->getRenderer()->isFullscreen())
    {
        [_window toggleFullScreen:nil];
    }
    
    [_window setTitle:[NSString stringWithUTF8String:ouzel::Engine::getInstance()->getRenderer()->getTitle().c_str()]];
    
    ouzel::EventHandlerPtr eventHandler = std::make_shared<ouzel::EventHandler>();
    eventHandler->windowHandler = [self](const ouzel::WindowEventPtr& event, const ouzel::VoidPtr&) -> bool {
        
        switch (event->type)
        {
            case ouzel::Event::Type::WINDOW_TITLE_CHANGE:
            {
                NSString* title = [NSString stringWithCString:event->title.c_str() encoding:NSASCIIStringEncoding];
                
                if ([NSThread isMainThread])
                {
                    _window.title = title;
                }
                else
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        _window.title = title;
                    });
                }
                break;
            }
            case ouzel::Event::Type::WINDOW_SIZE_CHANGE:
            {
                NSRect frame = [NSWindow contentRectForFrameRect:[_window frame]
                                                       styleMask:[[self window] styleMask]];
                
                NSRect newFrame = [NSWindow frameRectForContentRect:
                                   NSMakeRect(NSMinX(frame), NSMaxY(frame) - event->size.height, event->size.width, event->size.height)
                                                          styleMask:[_window styleMask]];
                
                if ([NSThread isMainThread])
                {
                    [_window setFrame:newFrame display:YES animate:NO];
                }
                else
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [_window setFrame:newFrame display:YES animate:NO];
                    });
                }
                break;
            }
            case ouzel::Event::Type::WINDOW_FULLSCREEN_CHANGE:
            {
                if (event->fullscreen != (bool)_fullscreen)
                {
                    if ([NSThread isMainThread])
                    {
                        [_window toggleFullScreen:nil];
                    }
                    else
                    {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [_window toggleFullScreen:nil];
                        });
                    }
                }
                break;
            }
            default:
                break;
        }
        
        return true;
    };
    
    ouzel::Engine::getInstance()->getEventDispatcher()->addEventHandler(eventHandler);
    
    NSRect windowFrame = [NSWindow contentRectForFrameRect:[[self window] frame]
                                                 styleMask:[[self window] styleMask]];
    
    _openGLView = [[OpenGLView alloc] initWithFrame:windowFrame];
    
    [_window setContentView:_openGLView];
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

-(void)windowDidChangeScreen:(NSNotification *)notification
{
    [_openGLView changeDisplay];
}

-(void)windowWillClose:(NSNotification *)notification
{
    [_openGLView close];
}

-(void)windowWillEnterFullScreen:(NSNotification *)notification
{
    _fullscreen = YES;
}

-(void)windowDidEnterFullScreen:(NSNotification *)notification
{
    ouzel::Engine::getInstance()->getRenderer()->setFullscreen(true);
}

-(void)windowWillExitFullScreen:(NSNotification *)notification
{
    _fullscreen = NO;
}

-(void)windowDidExitFullScreen:(NSNotification *)notification
{
    ouzel::Engine::getInstance()->getRenderer()->setFullscreen(false);
}

@end
