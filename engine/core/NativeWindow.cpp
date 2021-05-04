// Ouzel by Elviss Strazdins

#include "NativeWindow.hpp"
#include "Engine.hpp"

namespace ouzel::core
{
    NativeWindow::NativeWindow(const std::function<void(const Event&)>& initCallback,
                               const Size<std::uint32_t, 2>& newSize,
                               bool newResizable,
                               bool newFullscreen,
                               bool newExclusiveFullscreen,
                               const std::string& newTitle,
                               bool newHighDpi):
        size{newSize},
        resizable{newResizable},
        fullscreen{newFullscreen},
        exclusiveFullscreen{newExclusiveFullscreen},
        highDpi{newHighDpi},
        title{newTitle},
        callback{initCallback}
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
