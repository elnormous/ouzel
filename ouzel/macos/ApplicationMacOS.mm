// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationMacOS.h"
#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

namespace ouzel
{
    ApplicationMacOS::ApplicationMacOS(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    bool ApplicationMacOS::run()
    {
        @autoreleasepool
        {
            NSApplication* application = [NSApplication sharedApplication];

            AppDelegate* appDelegate = [[AppDelegate alloc] init];
            [application setDelegate:appDelegate];
            [application run];
            [appDelegate release];
        }

        return true;
    }
}