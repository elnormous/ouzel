// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationWin: public Application
    {
    public:
        ApplicationWin(const std::vector<std::string>& pArgs);
        virtual ~ApplicationWin();

        virtual int run() override;
    };
}
