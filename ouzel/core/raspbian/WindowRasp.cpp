// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <bcm_host.h>
#include "WindowRasp.h"
#include "utils/Log.h"

namespace ouzel
{
    WindowRasp::WindowRasp(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
    {
        bcm_host_init();
    }

    WindowRasp::~WindowRasp()
    {
        bcm_host_deinit();
    }

    bool WindowRasp::init()
    {
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

        return Window::init();
    }
}
