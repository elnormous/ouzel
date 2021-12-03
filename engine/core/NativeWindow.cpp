// Ouzel by Elviss Strazdins

#include "NativeWindow.hpp"
#include "Engine.hpp"

namespace ouzel::core
{
    NativeWindow::NativeWindow(const math::Size<std::uint32_t, 2>& newSize,
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
        title{newTitle}
    {
    }

    void NativeWindow::addCommand(const Command& command)
    {
        engine->executeOnMainThread(std::bind(&NativeWindow::executeCommand, this, command));
    }

    void NativeWindow::sendEvent(const Event& event)
    {
        std::unique_lock lock{eventQueueMutex};
        eventQueue.push(event);
        lock.unlock();
        eventQueueCondition.notify_all();
    }

    std::queue<NativeWindow::Event> NativeWindow::getEvents(bool waitForEvents)
    {
        std::unique_lock lock{eventQueueMutex};

        if (waitForEvents)
            eventQueueCondition.wait(lock, [this]() noexcept { return !eventQueue.empty(); });

        auto result = std::move(eventQueue);
        eventQueue = {};
        return result;
    }
}
