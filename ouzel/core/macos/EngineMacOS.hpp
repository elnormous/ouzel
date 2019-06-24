// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINEMACOS_HPP
#define OUZEL_CORE_ENGINEMACOS_HPP

#if defined(__OBJC__)
typedef NSApplication* NSApplicationPtr;
typedef NSAutoreleasePool* NSAutoreleasePoolPtr;
#else
#  include <objc/NSObjCRuntime.h>
typedef id NSApplicationPtr;
typedef id NSAutoreleasePoolPtr;
#endif

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineMacOS final: public Engine
    {
    public:
        EngineMacOS(int argc, char* argv[]);
        ~EngineMacOS();

        void run();

        void openURL(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        void executeAll();

    private:
        void engineMain() final;
        void runOnMainThread(const std::function<void()>& func) final;

        NSApplicationPtr application = nullptr;
        NSAutoreleasePoolPtr pool = nullptr;

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        uint32_t noSleepAssertionID = 0;
        id executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINEMACOS_HPP
