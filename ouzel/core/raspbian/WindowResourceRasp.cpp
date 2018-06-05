// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
        if (display != DISPMANX_NO_HANDLE)
            vc_dispmanx_display_close(display);

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
            return false;

        display = vc_dispmanx_display_open(0);
        if (display == DISPMANX_NO_HANDLE)
        {
            Log(Log::Level::ERR) << "Failed to open display";
            return false;
        }

        DISPMANX_MODEINFO_T modeInfo;
        int32_t success = vc_dispmanx_display_get_info(display, &modeInfo);

        if (success < 0)
        {
            Log(Log::Level::ERR) << "Failed to get display size";
            return false;
        }

        VC_RECT_T dstRect;
        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.width = modeInfo.width;
        dstRect.height = modeInfo.height;

        VC_RECT_T srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.width = modeInfo.width;
        srcRect.height = modeInfo.height;

        DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);

        if (dispmanUpdate == DISPMANX_NO_HANDLE)
        {
            Log(Log::Level::ERR) << "Failed to start display update";
            return false;
        }

        DISPMANX_ELEMENT_HANDLE_T dispmanElement = vc_dispmanx_element_add(dispmanUpdate, display,
                                                                           0, &dstRect, 0,
                                                                           &srcRect, DISPMANX_PROTECTION_NONE,
                                                                           0, 0, DISPMANX_NO_ROTATE);

        nativewindow.element = dispmanElement;
        nativewindow.width = modeInfo.width;
        nativewindow.height = modeInfo.height;
        vc_dispmanx_update_submit_sync(dispmanUpdate);

        size.width = static_cast<float>(modeInfo.width);
        size.height = static_cast<float>(modeInfo.height);
        resolution = size;

        return true;
    }
}
