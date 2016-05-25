// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "AppDelegate.h"
#import "OpenGLView.h"
#include "Engine.h"
#include "Renderer.h"
#include "WindowOSX.h"
#include "Size2.h"
#include "Utils.h"

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(NSNotification*)notification
{
    OUZEL_UNUSED(notification);
    ouzelMain(ouzel::getArgs());

    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->begin();
    }
}

-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
    OUZEL_UNUSED(notification);
}

-(void)applicationWillTerminate:(NSNotification*)notification
{
    OUZEL_UNUSED(notification);

    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->end();
    }
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    OUZEL_UNUSED(sender);
    return YES;
}

-(BOOL)application:(NSApplication*)sender openFile:(NSString*)filename
{
    OUZEL_UNUSED(sender);

    if (ouzel::sharedEngine)
    {
        ouzel::Event event;
        event.sender = ouzel::sharedEngine->getInput();
        event.type = ouzel::Event::Type::OPEN_FILE;

        event.systemEvent.filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];

        ouzel::sharedEngine->getEventDispatcher()->dispatchEvent(event);
    }

    return YES;
}

@end
