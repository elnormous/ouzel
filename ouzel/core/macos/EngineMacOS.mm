// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#include "EngineMacOS.hpp"
#include "utils/Log.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>

@end

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->init();
}

-(void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
    if (ouzel::sharedEngine)
    {
        ouzel::sharedEngine->start();
    }
}

-(void)applicationWillTerminate:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->exit();
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

-(BOOL)application:(__unused NSApplication*)sender openFile:(NSString*)filename
{
    if (ouzel::sharedEngine)
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::OPEN_FILE;

        event.systemEvent.filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];

        ouzel::sharedEngine->getEventDispatcher()->postEvent(event);
    }

    return YES;
}

-(void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->resume();
}

-(void)applicationDidResignActive:(__unused NSNotification*)notification
{
    ouzel::sharedEngine->pause();
}

@end

namespace ouzel
{
    EngineMacOS::EngineMacOS(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }

        mainQueue = dispatch_get_main_queue();
    }

    int EngineMacOS::run()
    {
        @autoreleasepool
        {
            NSApplication* application = [NSApplication sharedApplication];
            [application activateIgnoringOtherApps:YES];

            AppDelegate* appDelegate = [[[AppDelegate alloc] init] autorelease];
            [application setDelegate:appDelegate];
            [application run];
        }

        return EXIT_SUCCESS;
    }

    void EngineMacOS::executeOnMainThread(const std::function<void(void)>& func)
    {
        if (func)
        {
            std::function<void(void)> localFunction = func;

            dispatch_async(mainQueue, ^{
                localFunction();
            });
        }
    }

    bool EngineMacOS::openURL(const std::string& url)
    {
        NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];
        NSURL* nsURL = [NSURL URLWithString:nsStringURL];

        return [[NSWorkspace sharedWorkspace] openURL:nsURL] == YES;
    }

    void EngineMacOS::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        dispatch_async(mainQueue, ^{
            if (newScreenSaverEnabled)
            {
                if (noSleepAssertionID)
                {
                    if (IOPMAssertionRelease(noSleepAssertionID) != kIOReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to enable screen saver";
                    }

                    noSleepAssertionID = 0;
                }
            }
            else
            {
                if (!noSleepAssertionID)
                {
                    CFStringRef reasonForActivity = CFSTR("Ouzel disabling screen saver");
                    
                    if (IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleDisplaySleep,
                                                    kIOPMAssertionLevelOn, reasonForActivity, &noSleepAssertionID) != kIOReturnSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to disable screen saver";
                    }
                }
            }
        });
    }

    void EngineMacOS::main()
    {
        Engine::main();

        dispatch_async(mainQueue, ^{
            NSApplication* application = [NSApplication sharedApplication];
            if ([application isRunning]) [application terminate:Nil];
        });
    }
}
