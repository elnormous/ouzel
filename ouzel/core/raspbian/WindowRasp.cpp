// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <bcm_host.h>
#include "WindowRasp.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    WindowRasp::WindowRasp()
    {
        bcm_host_init();
    }

    WindowRasp::~WindowRasp()
    {
        bcm_host_deinit();
    }

    bool WindowRasp::init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth)
    {
        if (!Window::init(newSize,
                          newResizable,
                          newFullscreen,
                          newExclusiveFullscreen,
                          newTitle,
                          newHighDpi,
                          depth))
        {
            return false;
        }

        uint32_t screenWidth;
        uint32_t screenHeight;
        int32_t success = graphics_get_display_size(0, &screenWidth, &screenHeight);

        if (success == -1)
        {
            Log(Log::Level::ERR) << "Failed to get display size";
            return false;
        }

        size.width = static_cast<float>(screenWidth);
        size.height = static_cast<float>(screenHeight);

        return true;
    }
}
