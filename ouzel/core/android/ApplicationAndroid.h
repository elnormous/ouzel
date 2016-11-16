// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Application.h"

namespace ouzel
{
    class ApplicationAndroid: public Application
    {
    public:
        ApplicationAndroid();

        virtual int run() override;
    };
}
