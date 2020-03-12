// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
        ~EngineLinux() override;

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        inline auto getDisplay() const noexcept { return display; }

    private:
        void runOnMainThread(const std::function<void()>& func) final;
        void executeAll();

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

#if OUZEL_SUPPORTS_X11
        Display* display = nullptr;
        Atom executeAtom = None;
#elif OUZEL_SUPPORTS_DISPMANX
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}

#endif // OUZEL_CORE_ENGINELINUX_HPP
