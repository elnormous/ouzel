// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWEM_HPP
#define OUZEL_CORE_NATIVEWINDOWEM_HPP

#include "../NativeWindow.hpp"

namespace ouzel
{
    class NativeWindowEm final: public NativeWindow
    {
    public:
        NativeWindowEm(const std::function<void(const Event&)>& initCallback,
                       const Size2U& newSize,
                       bool newFullscreen,
                       const std::string& newTitle,
                       bool newHighDpi);

        void executeCommand(const Command& command) final;

        void setSize(const Size2U& newSize);
        void setFullscreen(bool newFullscreen);

        void handleResize();
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWEM_HPP
