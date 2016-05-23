// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Renderer.h"

namespace ouzel
{
    struct Settings
    {
        graphics::Renderer::Driver driver = graphics::Renderer::Driver::DEFAULT;

        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        graphics::Renderer::TextureFiltering textureFiltering = graphics::Renderer::TextureFiltering::NONE;
        bool resizable = false;
        bool fullscreen = false;
        float targetFPS = 60.0f;
        std::string title = "ouzel";
    };
}
