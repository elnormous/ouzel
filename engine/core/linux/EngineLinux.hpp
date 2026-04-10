#ifndef FLUXION_CORE_ENGINELINUX_HPP
#define FLUXION_CORE_ENGINELINUX_HPP

#include "../Setup.h"
#if FLUXION_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include "../../platform/x11/ThreadSupport.hpp"
#else
#  include <bcm_host.h>
#endif
#include "../Engine.hpp"

namespace fluxion::core::linux
{
#if FLUXION_SUPPORTS_X11
    int getLastError() noexcept;
#endif

    class Engine final: public core::Engine
    {
    public:
        Engine(const std::vector<std::string>& args);
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

#if FLUXION_SUPPORTS_X11
        platform::x11::ThreadSupport threadSupport;
        Display* display = nullptr;
        Atom executeAtom = None;
#elif FLUXION_SUPPORTS_DISPMANX
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}

#endif // FLUXION_CORE_ENGINELINUX_HPP
