// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWEM_HPP
#define OUZEL_CORE_NATIVEWINDOWEM_HPP

#include "core/NativeWindow.hpp"

namespace ouzel
{
    class NativeWindowEm final: public NativeWindow
    {
    public:
        NativeWindowEm(const std::function<void(const Event&)>& initCallback,
                       const Size2<uint32_t>& newSize,
                       bool newFullscreen,
                       const std::string& newTitle,
                       bool newHighDpi);

        void executeCommand(const Command& command) override;

        void setSize(const Size2<uint32_t>& newSize);
        void setFullscreen(bool newFullscreen);

        void handleResize();
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWEM_HPP
