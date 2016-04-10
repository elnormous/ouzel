// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"

namespace ouzel
{
    struct Settings
    {
        video::Renderer::Driver driver =
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS) || defined(OUZEL_PLATFORM_ANDROID) || defined(OUZEL_PLATFORM_LINUX)
        video::Renderer::Driver::OPENGL;
#elif defined(SUPPORTS_DIRECT3D11)
        video::Renderer::Driver::DIRECT3D11;
#endif

        Size2 size;
        uint32_t sampleCount = 1; // MSAA sample count
        bool resizable = false;
        bool fullscreen = false;
        float targetFPS = 60.0f;
        std::string title = "ouzel";
    };
}
