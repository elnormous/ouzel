// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationEm: public Application
    {
    public:
        ApplicationEm(int aArgc, char* aArgv[]);

        virtual int run() override;

        bool step();

        virtual bool openURL(const std::string& url) override;
    };
}
