// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "NativeWindow.hpp"
#include "Engine.hpp"

namespace ouzel
{
    NativeWindow::NativeWindow(const std::function<void(const Event&)>& initCallback,
                               const Size2<uint32_t>& newSize,
                               bool newResizable,
                               bool newFullscreen,
                               bool newExclusiveFullscreen,
                               const std::string& newTitle,
                               bool newHighDpi):
        size(newSize),
        resizable(newResizable),
        fullscreen(newFullscreen),
        exclusiveFullscreen(newExclusiveFullscreen),
        highDpi(newHighDpi),
        title(newTitle),
        callback(initCallback)
    {

    }

    void NativeWindow::addCommand(const Command& command)
    {
        engine->executeOnMainThread(std::bind(&NativeWindow::executeCommand, this, command));
    }

    void NativeWindow::sendEvent(const Event& event)
    {
        callback(event);
    }
}
