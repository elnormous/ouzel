// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Engine.hpp"

namespace ouzel
{
    class EngineMacOS: public Engine
    {
    public:
        EngineMacOS(int argc, char* argv[]);

        virtual int run() override;

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;

        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

        void executeAll();

    protected:
        virtual void main() override;

        std::queue<std::function<void(void)>> executeQueue;
        Mutex executeMutex;

        uint32_t noSleepAssertionID = 0;
    };
}
