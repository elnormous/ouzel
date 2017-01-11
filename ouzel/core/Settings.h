// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "core/CompileConfig.h"
#include "graphics/Renderer.h"
#include "audio/Audio.h"

namespace ouzel
{
    struct Settings
    {
        graphics::Renderer::Driver renderDriver = graphics::Renderer::Driver::DEFAULT;
        audio::Audio::Driver audioDriver = audio::Audio::Driver::DEFAULT;

        std::string title = "ouzel";
        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        graphics::TextureFilter textureFilter = graphics::TextureFilter::NONE;
        bool resizable = false;
        bool fullscreen = false;
        bool verticalSync = true;
        graphics::PixelFormat backBufferFormat = graphics::PixelFormat::DEFAULT;
        uint32_t depthBits = 0;
    };
}
