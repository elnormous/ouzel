// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINELINUX_HPP
#define OUZEL_CORE_ENGINELINUX_HPP

#include "../Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include "../../platform/x11/ThreadSupport.hpp"
#else
#  include <bcm_host.h>
#endif
#include "../Engine.hpp"

namespace ouzel::core::linux
{
#if OUZEL_SUPPORTS_X11
    const std::error_category& getErrorCategory() noexcept;
    int getLastError();
#endif

    class Engine final: public core::Engine
    {
    public:
        Engine(int argc, char* argv[]);
        ~Engine() override;

        void run();

        void openUrl(const std::string& url) final;

        void setScreenSaverEnabled(bool newScreenSaverEnabled) final;

        auto getDisplay() const noexcept { return display; }

    private:
        void runOnMainThread(const std::function<void()>& func) final;
        void executeAll();

        std::queue<std::function<void()>> executeQueue;
        std::mutex executeMutex;

#if OUZEL_SUPPORTS_X11
        platform::x11::ThreadSupport threadSupport;
        Display* display = nullptr;
        Atom executeAtom = None;
#elif OUZEL_SUPPORTS_DISPMANX
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}

#endif // OUZEL_CORE_ENGINELINUX_HPP
