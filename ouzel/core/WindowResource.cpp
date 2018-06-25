// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "WindowResource.hpp"
#include "thread/Lock.hpp"

namespace ouzel
{
    WindowResource::Listener::~Listener()
    {
    }

    void WindowResource::init(const Size2& newSize,
                              bool newResizable,
                              bool newFullscreen,
                              bool newExclusiveFullscreen,
                              const std::string& newTitle,
                              bool newHighDpi,
                              bool)
    {
        size = newSize;
        resizable = newResizable;
        fullscreen = newFullscreen;
        exclusiveFullscreen = newExclusiveFullscreen;
        highDpi = newHighDpi;
        title = newTitle;
    }

    void WindowResource::close()
    {
    }

    void WindowResource::setSize(const Size2& newSize)
    {
        size = newSize;
    }

    void WindowResource::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;
    }

    void WindowResource::setTitle(const std::string& newTitle)
    {
        title = newTitle;
    }

    void WindowResource::setListener(Listener* newListener)
    {
        Lock lock(listenerMutex);
        listener = newListener;
    }
}
