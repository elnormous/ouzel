// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import <Cocoa/Cocoa.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#include "EngineMacOS.hpp"
#include "utils/Errors.hpp"

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
        std::unique_ptr<ouzel::SystemEvent> event(new ouzel::SystemEvent());
        event->type = ouzel::Event::Type::OPEN_FILE;
        event->filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];
        ouzel::engine->getEventDispatcher().postEvent(std::move(event));
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

-(void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
}

@end

@interface ExecuteHandler: NSObject
{
    ouzel::EngineMacOS* engine;
}
@end

@implementation ExecuteHandler

-(id)initWithEngine:(ouzel::EngineMacOS*)initEngine
{
    if (self = [super init])
        engine = initEngine;

    return self;
}

-(void)executeAll
{
    engine->executeAll();
}

@end

namespace ouzel
{
    EngineMacOS::EngineMacOS(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);

        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    EngineMacOS::~EngineMacOS()
    {
        if (executeHanlder) [executeHanlder release];
    }

    void EngineMacOS::run()
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
        [quitItem setTarget:[application delegate]];
        [subMenu addItem:quitItem];

        application.mainMenu = mainMenu;

        [application run];

        [pool release];
    }

    void EngineMacOS::executeOnMainThread(const std::function<void(void)>& func)
    {
        std::unique_lock<std::mutex> lock(executeMutex);

        executeQueue.push(func);

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    void EngineMacOS::openURL(const std::string& url)
    {
        executeOnMainThread([url](){
            NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];
            NSURL* nsURL = [NSURL URLWithString:nsStringURL];

            [[NSWorkspace sharedWorkspace] openURL:nsURL];
        });
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
                        throw SystemError("Failed to enable screen saver");

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
                        throw SystemError("Failed to disable screen saver");
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
                std::unique_lock<std::mutex> lock(executeMutex);

                if (executeQueue.empty()) break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func) func();
        }
    }
}
