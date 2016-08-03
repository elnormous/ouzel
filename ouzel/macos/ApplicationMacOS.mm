// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#include "ApplicationMacOS.h"
#import "AppDelegate.h"

namespace ouzel
{
    ApplicationMacOS::ApplicationMacOS(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
        mainQueue = dispatch_get_main_queue();
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

    void ApplicationMacOS::execute(const std::function<void(void)>& func)
    {
        dispatch_async(mainQueue, ^{
            func();
        });
    }
}
