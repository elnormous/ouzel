// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#include "ApplicationMacOS.h"
#import "AppDelegate.h"

namespace ouzel
{
    ApplicationMacOS::ApplicationMacOS(int aArgc, char* aArgv[]):
        Application(aArgc, aArgv)
    {
        mainQueue = dispatch_get_main_queue();
    }

    int ApplicationMacOS::run()
    {
        @autoreleasepool
        {
            NSApplication* application = [NSApplication sharedApplication];

            AppDelegate* appDelegate = [[[AppDelegate alloc] init] autorelease];
            [application setDelegate:appDelegate];
            [application run];
        }

        return EXIT_SUCCESS;
    }

    void ApplicationMacOS::execute(const std::function<void(void)>& func)
    {
        std::function<void(void)> localFunction = func;

        dispatch_async(mainQueue, ^{
            localFunction();
        });
    }
}
