// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if !defined(__OBJC__)
#include <objc/objc.h>
#endif

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineMacOS: public Engine
    {
    public:
        EngineMacOS(int argc, char* argv[]);
        virtual ~EngineMacOS();

        virtual void run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual void openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        void executeAll();

    protected:
        virtual void main() override;

        std::queue<std::function<void(void)>> executeQueue;
        Mutex executeMutex;

        uint32_t noSleepAssertionID = 0;
        id executeHanlder = nil;
    };
}
