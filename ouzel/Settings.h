// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    struct Settings
    {
        Renderer::Driver driver = Renderer::Driver::NONE;
        Size2 size;
        bool resizable = false;
        bool fullscreen = false;
        float targetFPS = 60.0f;
    };
}
