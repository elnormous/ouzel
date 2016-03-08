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

void ouzelMain(const std::vector<std::string>& args);

-(void)applicationWillFinishLaunching:(NSNotification *)notification
{
    ouzelMain(ouzel::getArgs());
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
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
