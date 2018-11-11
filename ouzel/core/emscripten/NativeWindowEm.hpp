// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWEM_HPP
#define OUZEL_CORE_NATIVEWINDOWEM_HPP

#include "core/NativeWindow.hpp"

namespace ouzel
{
    class NativeWindowEm final: public NativeWindow
    {
    public:
        NativeWindowEm(EventHandler& initEventHandler,
                       const Size2& newSize,
                       bool newFullscreen,
                       const std::string& newTitle,
                       bool newHighDpi);
        void setSize(const Size2& newSize) override;
        void setFullscreen(bool newFullscreen) override;

        void handleResize();
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWEM_HPP
