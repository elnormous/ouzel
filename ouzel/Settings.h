// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

namespace ouzel
{
    struct Settings
    {
        video::Renderer::Driver driver = video::Renderer::Driver::DEFAULT;

        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        bool resizable = false;
        bool fullscreen = false;
        float targetFPS = 60.0f;
        std::string title = "ouzel";
    };
}
