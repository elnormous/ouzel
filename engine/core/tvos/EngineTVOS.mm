// Ouzel by Elviss Strazdins

#include <objc/NSObject.h>
#include "EngineTVOS.hpp"

@interface ExecuteHandler: NSObject
@end

@implementation ExecuteHandler
{
    ouzel::core::tvos::Engine* engine;
}

- (id)initWithEngine:(ouzel::core::tvos::Engine*)initEngine
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

namespace ouzel::core::tvos
{
    Engine::Engine(const std::vector<std::string>& args):
        core::Engine{args}
    {
        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    Engine::~Engine()
    {
        [executeHanlder release];
    }

    void Engine::runOnMainThread(const std::function<void()>& func)
    {
        std::unique_lock lock{executeMutex};
        executeQueue.push(func);
        lock.unlock();

        [executeHanlder performSelectorOnMainThread:@selector(executeAll) withObject:nil waitUntilDone:NO];
    }

    void Engine::openUrl(const std::string& url)
    {
        executeOnMainThread([url]() {
            NSString* urlString = [NSString stringWithUTF8String:url.c_str()];
            NSURL* nsUrl = [NSURL URLWithString:urlString];

            [[UIApplication sharedApplication] openURL:nsUrl];
        });
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        core::Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([newScreenSaverEnabled]() {
            [UIApplication sharedApplication].idleTimerDisabled = newScreenSaverEnabled ? YES : NO;
        });
    }

    void Engine::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock{executeMutex};

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
