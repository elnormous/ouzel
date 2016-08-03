// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationRPI: public Application
    {
    public:
        ApplicationRPI(int pArgc, char* pArgv[]);
        virtual ~ApplicationRPI() {}

        virtual bool run() override;
    };
}
