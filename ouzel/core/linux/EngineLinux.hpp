// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#else
#  include <bcm_host.h>
#endif
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineLinux: public Engine
    {
    public:
        EngineLinux(int initArgc, char* initArgv[]);
        virtual ~EngineLinux();

        void run();

        virtual void executeOnMainThread(const std::function<void(void)>& func) override;
        virtual void openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

#if OUZEL_SUPPORTS_X11
        inline Display* getDisplay() const { return display; }
#else
        inline DISPMANX_DISPLAY_HANDLE_T getDisplay() const { return display; }
#endif

    protected:
        void executeAll();

        std::queue<std::function<void(void)>> executeQueue;
        std::mutex executeMutex;

#if OUZEL_SUPPORTS_X11
        Display* display = nullptr;
#else
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}
