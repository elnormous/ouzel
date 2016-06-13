// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#include <vector>
#include "Engine.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
    ouzel::setArgs(argc, argv);

    @autoreleasepool
    {
        NSApplication* application = [NSApplication sharedApplication];

        AppDelegate* appDelegate = [[AppDelegate alloc] init];
        [application setDelegate:appDelegate];
        [application run];
        [appDelegate release];
    }

    return 0;
}
