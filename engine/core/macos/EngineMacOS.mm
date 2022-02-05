// Ouzel by Elviss Strazdins

#include <stdexcept>
#include <objc/NSObject.h>
#include "EngineMacOS.hpp"
#include "../../platform/iokit/ErrorCategory.hpp"

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
    Engine::Engine(const std::vector<std::string>& args):
        core::Engine{args}
    {
        executeHanlder = [[ExecuteHandler alloc] initWithEngine:this];
    }

    Engine::~Engine()
    {
        if (executeHanlder) [executeHanlder release];
        if (noSleepAssertionId) IOPMAssertionRelease(noSleepAssertionId);
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

            [[NSWorkspace sharedWorkspace] openURL:nsUrl];
        });
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        core::Engine::setScreenSaverEnabled(newScreenSaverEnabled);

        executeOnMainThread([this, newScreenSaverEnabled]() {
            if (newScreenSaverEnabled)
            {
                if (noSleepAssertionId)
                {
                    if (const auto error = IOPMAssertionRelease(noSleepAssertionId); error != kIOReturnSuccess)
                        throw std::system_error{error, platform::iokit::errorCategory, "Failed to enable screen saver"};

                    noSleepAssertionId = 0;
                }
            }
            else
            {
                if (!noSleepAssertionId)
                {
                    CFStringRef reasonForActivity = CFSTR("Ouzel disabling screen saver");

                    if (const auto error = IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleDisplaySleep,
                                                                       kIOPMAssertionLevelOn, reasonForActivity,
                                                                       &noSleepAssertionId); error != kIOReturnSuccess)
                        throw std::system_error{error, platform::iokit::errorCategory, "Failed to disable screen saver"};
                }
            }
        });
    }

    void Engine::engineMain()
    {
        core::Engine::engineMain();
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
