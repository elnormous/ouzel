// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dispatch/dispatch.h>
#include "core/Engine.h"

namespace ouzel
{
    class EngineIOS: public Engine
    {
    public:
        EngineIOS(int aArgc, char* aArgv[]);

        virtual int run() override;

        virtual void execute(const std::function<void(void)>& func) override;

        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

    protected:
        dispatch_queue_t mainQueue;

        int argc = 0;
        char** argv = nullptr;
    };
}
