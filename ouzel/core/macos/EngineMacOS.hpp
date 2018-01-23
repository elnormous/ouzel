// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dispatch/dispatch.h>
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

        virtual bool setCurrentThreadName(const std::string& name) override;

    protected:
        virtual void main() override;

        dispatch_queue_t mainQueue;
        uint32_t noSleepAssertionID = 0;
    };
}
