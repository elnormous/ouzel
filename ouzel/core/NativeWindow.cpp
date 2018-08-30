// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeWindow.hpp"

namespace ouzel
{
    NativeWindow::NativeWindow(const Size2& newSize,
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
        title(newTitle)
    {

    }

    void NativeWindow::close()
    {
    }

    void NativeWindow::setSize(const Size2& newSize)
    {
        size = newSize;
    }

    void NativeWindow::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;
    }

    void NativeWindow::setTitle(const std::string& newTitle)
    {
        title = newTitle;
    }

    std::vector<NativeWindow::Event> NativeWindow::getEvents() const
    {
        std::vector<Event> result;
        std::unique_lock<std::mutex> lock(eventQueueMutex);
        while (!eventQueue.empty())
        {
            result.push_back(eventQueue.front());
            eventQueue.pop();
        }
        return result;
    }

    void NativeWindow::addEvent(const Event& event)
    {
        std::unique_lock<std::mutex> lock(eventQueueMutex);
        eventQueue.push(event);
    }
}
