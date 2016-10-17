// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dispatch/dispatch.h>
#include "core/Application.h"

namespace ouzel
{
    class ApplicationMacOS: public Application
    {
    public:
        ApplicationMacOS(int aArgc, char* aArgv[]);
        virtual ~ApplicationMacOS() {}

        virtual int run() override;

        virtual void execute(const std::function<void(void)>& func) override;

    protected:
        dispatch_queue_t mainQueue;
    };
}
