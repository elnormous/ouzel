// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if !defined(__OBJC__)
#include <objc/objc.h>
#endif

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineIOS final: public Engine
    {
    public:
        EngineIOS(int initArgc, char* initArgv[]);
        virtual ~EngineIOS();

        void run();

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual void openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        void executeAll();

    private:
        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;

        int argc = 0;
        char** argv = nullptr;
        id executeHanlder = nil;
    };
}
