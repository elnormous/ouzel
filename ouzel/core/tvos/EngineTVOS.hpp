// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if !defined(__OBJC__)
#include <objc/objc.h>
#endif

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineTVOS: public Engine
    {
    public:
        EngineTVOS(int initArgc, char* initArgv[]);
        virtual ~EngineTVOS();

        virtual int run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        void executeAll();

    protected:
        std::queue<std::function<void(void)>> executeQueue;
        Mutex executeMutex;

        int argc = 0;
        char** argv = nullptr;
        id executeHanlder = nil;
    };
}
