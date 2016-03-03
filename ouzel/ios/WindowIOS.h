// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Window.h"

namespace ouzel
{
    class WindowIOS: public Window
    {
    public:
        WindowIOS(const Size2& size, bool resizable, bool fullscreen, const std::string& title);
    };
}
