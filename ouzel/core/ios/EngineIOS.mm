// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#import <UIKit/UIKit.h>
#include "EngineIOS.hpp"

@interface AppDelegate: UIResponder<UIApplicationDelegate>
@end

@implementation AppDelegate

-(BOOL)application:(__unused UIApplication*)application willFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(deviceOrientationDidChange:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:[UIDevice currentDevice]];

    ouzel::engine->init();

    return YES;
}

-(BOOL)application:(__unused UIApplication*)application didFinishLaunchingWithOptions:(__unused NSDictionary*)launchOptions
{
    if (ouzel::engine)
        ouzel::engine->start();

    return YES;
}

-(void)applicationDidBecomeActive:(__unused UIApplication*)application
{
    ouzel::engine->resume();
}

-(void)applicationWillResignActive:(__unused UIApplication*)application
{
    ouzel::engine->pause();
}

-(void)applicationDidEnterBackground:(__unused UIApplication*)application
{
}

-(void)applicationWillEnterForeground:(__unused UIApplication*)application
{
}

-(void)applicationWillTerminate:(__unused UIApplication*)application
{
    ouzel::engine->exit();
}

-(void)applicationDidReceiveMemoryWarning:(__unused UIApplication*)application
{
    if (ouzel::engine)
    {
        std::unique_ptr<ouzel::SystemEvent> event(new ouzel::SystemEvent());
        event->type = ouzel::Event::Type::LOW_MEMORY;

        ouzel::engine->getEventDispatcher().postEvent(std::move(event));
    }
}

-(void)deviceOrientationDidChange:(NSNotification*)note
{
    UIDevice* device = note.object;
    UIDeviceOrientation orientation = device.orientation;

    std::unique_ptr<ouzel::SystemEvent> event(new ouzel::SystemEvent());
    event->type = ouzel::Event::Type::ORIENTATION_CHANGE;

    switch (orientation)
    {
        case UIDeviceOrientationPortrait:
            event->orientation = ouzel::SystemEvent::Orientation::PORTRAIT;
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            event->orientation = ouzel::SystemEvent::Orientation::PORTRAIT_REVERSE;
            break;
        case UIDeviceOrientationLandscapeLeft:
            event->orientation = ouzel::SystemEvent::Orientation::LANDSCAPE;
            break;
        case UIDeviceOrientationLandscapeRight:
            event->orientation = ouzel::SystemEvent::Orientation::LANDSCAPE_REVERSE;
            break;
        case UIDeviceOrientationFaceUp:
            event->orientation = ouzel::SystemEvent::Orientation::FACE_UP;
            break;
        case UIDeviceOrientationFaceDown:
            event->orientation = ouzel::SystemEvent::Orientation::FACE_DOWN;
            break;
        default:
            event->orientation = ouzel::SystemEvent::Orientation::UNKNOWN;
            break;
    }

    ouzel::engine->getEventDispatcher().postEvent(std::move(event));
}
@end

@interface ExecuteHandler: NSObject
@end

@implementation ExecuteHandler
{
    ouzel::EngineIOS* engine;
}

-(id)initWithEngine:(ouzel::EngineIOS*)initEngine
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
    EngineIOS::EngineIOS(int initArgc, char* initArgv[]):
        argc(initArgc), argv(initArgv)
    {
        for (int i = 0; i < initArgc; ++i)
            args.push_back(initArgv[i]);

        pool = [[NSAutoreleasePool alloc] init];
        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    EngineIOS::~EngineIOS()
    {
        if (executeHanlder) [executeHanlder release];
        if (pool) [pool release];
    }

    void EngineIOS::run()
    {
        UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }

    void EngineIOS::runOnMainThread(const std::function<void()>& func)
    {
        std::unique_lock<std::mutex> lock(executeMutex);
        executeQueue.push(func);
        lock.unlock();

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    void EngineIOS::openURL(const std::string& url)
    {
        executeOnMainThread([url](){
            NSString* nsStringURL = [NSString stringWithUTF8String:url.c_str()];
            NSURL* nsURL = [NSURL URLWithString:nsStringURL];

            [[UIApplication sharedApplication] openURL:nsURL];
        });
    }

    void EngineIOS::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled]() {
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }

    void EngineIOS::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(executeMutex);

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
