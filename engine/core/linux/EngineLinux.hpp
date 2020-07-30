// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_ENGINELINUX_HPP
#define OUZEL_CORE_ENGINELINUX_HPP

#include "../Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#else
#  include <bcm_host.h>
#endif
#include "../Engine.hpp"

namespace ouzel::core::linux
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "X11";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case BadAccess: return "BadAccess";
                case BadAlloc: return "BadAlloc";
                case BadAtom: return "BadAtom";
                case BadColor: return "BadColor";
                case BadCursor: return "BadCursor";
                case BadDrawable: return "BadDrawable";
                case BadFont: return "BadFont";
                case BadGC: return "BadGC";
                case BadIDChoice: return "BadIDChoice";
                case BadImplementation: return "BadImplementation";
                case BadLength: return "BadLength";
                case BadMatch: return "BadMatch";
                case BadName: return "BadName";
                case BadPixmap: return "BadPixmap";
                case BadRequest: return "BadRequest";
                case BadValue: return "BadValue";
                case BadWindow: return "BadWindow";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    extern ErrorCategory errorCategory;
    int getLastError();

    class Engine final: public core::Engine
    {
    public:
        Engine(int initArgc, char* initArgv[]);
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
        Display* display = nullptr;
        Atom executeAtom = None;
#elif OUZEL_SUPPORTS_DISPMANX
        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
#endif
    };
}

#endif // OUZEL_CORE_ENGINELINUX_HPP
