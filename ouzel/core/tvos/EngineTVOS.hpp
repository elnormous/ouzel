// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINETVOS_HPP
#define OUZEL_CORE_ENGINETVOS_HPP

#if !defined(__OBJC__)
#  include <objc/objc.h>
#endif

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineTVOS final: public Engine
    {
    public:
        EngineTVOS(int initArgc, char* initArgv[]);
        ~EngineTVOS();

        void run();

        void executeOnMainThread(const std::function<void()>& func) override;

        void openURL(const std::string& url) override;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        void executeAll();

    private:
        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

        int argc = 0;
        char** argv = nullptr;
        id executeHanlder = nil;
    };
}

#endif // OUZEL_CORE_ENGINETVOS_HPP
