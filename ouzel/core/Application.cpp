// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"

namespace ouzel
{
    Application::Application()
    {
        sharedApplication = this;
    }

    Application::~Application()
    {
    }
}
