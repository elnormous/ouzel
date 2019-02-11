// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINELINUX_HPP
#define OUZEL_CORE_ENGINELINUX_HPP

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#else
#  include <bcm_host.h>
#endif
#include "core/Engine.hpp"

namespace ouzel
{
    class EngineLinux final: public Engine
    {
    public:
        EngineLinux(int initArgc, char* initArgv[]);
        ~EngineLinux();

        void run();

        void openURL(const std::string& url) override;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

#if OUZEL_SUPPORTS_X11
        inline Display* getDisplay() const { return display; }
#elif OUZEL_SUPPORTS_DISPMANX
        inline DISPMANX_DISPLAY_HANDLE_T getDisplay() const { return display; }
#endif

    private:
        void runOnMainThread(const std::function<void()>& func) override;
        void executeAll();

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;
        Atom executeAtom = None;

#if OUZEL_SUPPORTS_X11
        Display* display = nullptr;
#elif OUZEL_SUPPORTS_DISPMANX
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}

#endif // OUZEL_CORE_ENGINELINUX_HPP
