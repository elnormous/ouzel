// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Cocoa/Cocoa.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#include "EngineMacOS.hpp"
#include "thread/Lock.hpp"
#include "utils/Log.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>

@end

@implementation AppDelegate

-(void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
    ouzel::engine->init();
}

-(void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
    ouzel::engine->start();
}

-(void)applicationWillTerminate:(__unused NSNotification*)notification
{
    ouzel::engine->exit();
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

-(BOOL)application:(__unused NSApplication*)sender openFile:(NSString*)filename
{
    if (ouzel::engine)
    {
        ouzel::Event event;
        event.type = ouzel::Event::Type::OPEN_FILE;

        event.systemEvent.filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];

        ouzel::engine->getEventDispatcher()->postEvent(event);
    }

    return YES;
}

-(void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
    ouzel::engine->resume();
}

-(void)applicationDidResignActive:(__unused NSNotification*)notification
{
    ouzel::engine->pause();
}

-(void)executeAll
{
    ouzel::EngineMacOS* engineMacOS = static_cast<ouzel::EngineMacOS*>(ouzel::engine);

    engineMacOS->executeAll();
}

-(void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
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
    }

    int EngineMacOS::run()
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        NSApplication* application = [NSApplication sharedApplication];
        [application activateIgnoringOtherApps:YES];
        [application setDelegate:[[[AppDelegate alloc] init] autorelease]];

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] init] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] init] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [quitItem setTarget: [application delegate]];
        [subMenu addItem:quitItem];

        application.mainMenu = mainMenu;

        [application run];

        [pool release];

        return EXIT_SUCCESS;
    }

    void EngineMacOS::executeOnMainThread(const std::function<void(void)>& func)
    {
        Lock lock(executeMutex);

        executeQueue.push(func);

        NSApplication* application = [NSApplication sharedApplication];
        NSObject* delegate = application.delegate;
        [delegate performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
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

        executeOnMainThread([this, newScreenSaverEnabled]() {
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

        executeOnMainThread([]() {
            NSApplication* application = [NSApplication sharedApplication];
            if ([application isRunning]) [application terminate:nil];
        });
    }

    void EngineMacOS::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                Lock lock(executeMutex);

                if (executeQueue.empty())
                {
                    break;
                }

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }
}
