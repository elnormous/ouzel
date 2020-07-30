// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#import <Cocoa/Cocoa.h>
#import <IOKit/pwr_mgt/IOPMLib.h>
#include "EngineMacOS.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>
@end

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
    ouzel::engine->init();
}

- (void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
    ouzel::engine->start();
}

- (void)applicationWillTerminate:(__unused NSNotification*)notification
{
    ouzel::engine->exit();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

- (BOOL)application:(__unused NSApplication*)sender openFile:(NSString*)filename
{
    if (ouzel::engine)
    {
        auto event = std::make_unique<ouzel::SystemEvent>();
        event->type = ouzel::Event::Type::openFile;
        event->filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];
        ouzel::engine->getEventDispatcher().postEvent(std::move(event));
    }

    return YES;
}

- (void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
    //ouzel::engine->resume();
}

- (void)applicationDidResignActive:(__unused NSNotification*)notification
{
    //ouzel::engine->pause();
}

- (void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
}
@end

@interface ExecuteHandler: NSObject
@end

@implementation ExecuteHandler
{
    ouzel::core::macos::Engine* engine;
}

- (id)initWithEngine:(ouzel::core::macos::Engine*)initEngine
{
    if (self = [super init])
        engine = initEngine;

    return self;
}

- (void)executeAll
{
    engine->executeAll();
}
@end

namespace ouzel::core::macos
{
    Engine::Engine(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);

        pool = [[NSAutoreleasePool alloc] init];

        application = [NSApplication sharedApplication];
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

        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    Engine::~Engine()
    {
        if (executeHanlder) [executeHanlder release];
        if (pool) [pool release];
    }

    void Engine::run()
    {
        [application run];
    }

    void Engine::runOnMainThread(const std::function<void()>& func)
    {
        std::unique_lock lock(executeMutex);
        executeQueue.push(func);
        lock.unlock();

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    void Engine::openUrl(const std::string& url)
    {
        executeOnMainThread([url]() {
            NSString* urlString = [NSString stringWithUTF8String:url.c_str()];
            NSURL* nsUrl = [NSURL URLWithString:urlString];

            [[NSWorkspace sharedWorkspace] openURL:nsUrl];
        });
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        core::Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([this, newScreenSaverEnabled]() {
            if (newScreenSaverEnabled)
            {
                if (noSleepAssertionID)
                {
                    if (IOPMAssertionRelease(noSleepAssertionID) != kIOReturnSuccess)
                        throw std::runtime_error("Failed to enable screen saver");

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
                        throw std::runtime_error("Failed to disable screen saver");
                }
            }
        });
    }

    void Engine::engineMain()
    {
        core::Engine::engineMain();

        executeOnMainThread([]() {
            NSApplication* app = [NSApplication sharedApplication];
            if ([app isRunning]) [app terminate:nil];
        });
    }

    void Engine::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock(executeMutex);

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
