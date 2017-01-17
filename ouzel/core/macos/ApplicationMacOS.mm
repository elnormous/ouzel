// Copyright (C) 2017 Elviss Strazdins
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
            [application activateIgnoringOtherApps : YES];

            AppDelegate* appDelegate = [[[AppDelegate alloc] init] autorelease];
            [application setDelegate:appDelegate];
            [application run];
        }

        return EXIT_SUCCESS;
    }

    void ApplicationMacOS::exit()
    {
        Application::exit();

        dispatch_async(mainQueue, ^{
            NSApplication* application = [NSApplication sharedApplication];
            [application terminate:Nil];
        });
    }

    void ApplicationMacOS::execute(const std::function<void(void)>& func)
    {
        std::function<void(void)> localFunction = func;

        dispatch_async(mainQueue, ^{
            localFunction();
        });
    }

    bool ApplicationMacOS::openURL(const std::string& url)
    {
        NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];

        return [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:nsStringURL]] == YES;
    }
}
