// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationLinux: public Application
    {
    public:
        ApplicationLinux(int aArgc, char* aArgv[]);

        virtual int run() override;
    };
}
