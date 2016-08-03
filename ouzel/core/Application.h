// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Noncopyable.h"

namespace ouzel
{
    class Application: public Noncopyable
    {
    public:
        Application();
        virtual ~Application();
    };

    extern Application* sharedApplication;
}
