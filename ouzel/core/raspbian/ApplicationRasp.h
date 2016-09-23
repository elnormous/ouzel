// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationRasp: public Application
    {
    public:
        ApplicationRasp(int pArgc, char* pArgv[]);
        virtual ~ApplicationRasp() {}

        virtual int run() override;
    };
}
