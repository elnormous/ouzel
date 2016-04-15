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
}

-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
    OUZEL_UNUSED(notification);
    ouzelMain(ouzel::getArgs());
    ouzel::sharedEngine->begin();
}

-(void)applicationWillTerminate:(NSNotification*)notification
{
    OUZEL_UNUSED(notification);
    ouzel::sharedEngine->end();
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    OUZEL_UNUSED(sender);
    return YES;
}

-(BOOL)application:(NSApplication *)sender openFile:(NSString *)filename
{
    OUZEL_UNUSED(sender);

    ouzel::SystemEventPtr event = std::make_shared<ouzel::SystemEvent>();
    event->type = ouzel::Event::Type::OPEN_FILE;
    event->filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];

    ouzel::sharedEngine->getEventDispatcher()->dispatchEvent(event, ouzel::sharedEngine->getInput());

    return YES;
}

@end
