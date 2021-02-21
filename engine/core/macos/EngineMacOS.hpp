// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEMACOS_HPP
#define OUZEL_CORE_ENGINEMACOS_HPP

#if defined(__OBJC__)
typedef NSApplication* NSApplicationPtr;
#else
#  include <objc/NSObjCRuntime.h>
typedef id NSApplicationPtr;
#endif

#include "../Engine.hpp"

namespace ouzel::core::macos
{
    class Engine final: public core::Engine
    {
    public:
        Engine(int argc, char* argv[]);
        ~Engine() override;

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        void executeAll();

    private:
        void engineMain() final;
        void runOnMainThread(const std::function<void()>& func) final;

        NSApplicationPtr application = nil;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        std::uint32_t noSleepAssertionID = 0;
        id executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINEMACOS_HPP
