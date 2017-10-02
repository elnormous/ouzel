// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <bcm_host.h>
#include "WindowResourceRasp.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    WindowResourceRasp::WindowResourceRasp()
    {
        bcm_host_init();
    }

    WindowResourceRasp::~WindowResourceRasp()
    {
        bcm_host_deinit();
    }

    bool WindowResourceRasp::init(const Size2& newSize,
                                  bool newResizable,
                                  bool newFullscreen,
                                  bool newExclusiveFullscreen,
                                  const std::string& newTitle,
                                  bool newHighDpi,
                                  bool depth)
    {
        if (!WindowResource::init(newSize,
                                  newResizable,
                                  newFullscreen,
                                  newExclusiveFullscreen,
                                  newTitle,
                                  newHighDpi,
                                  depth))
        {
            return false;
        }

        DISPMANX_DISPLAY_HANDLE_T display = vc_dispmanx_display_open(0);
        if (!display)
        {
            Log(Log::Level::ERR) << "Failed to open display";
            return false;
        }

        DISPMANX_MODEINFO_T modeInfo;
        int32_t success = vc_dispmanx_display_get_info(display, &modeInfo);

        if (success < 0)
        {
            Log(Log::Level::ERR) << "Failed to get display size";
            vc_dispmanx_display_close(display);
            return false;
        }

        size.width = static_cast<float>(modeInfo.width);
        size.height = static_cast<float>(modeInfo.height);
        resolution = size;

        vc_dispmanx_display_close(display);

        return true;
    }
}
