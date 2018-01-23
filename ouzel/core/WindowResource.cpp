// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowResource.hpp"

namespace ouzel
{
    WindowResource::Listener::~Listener()
    {
    }

    bool WindowResource::init(const Size2& newSize,
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

        return true;
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
        std::unique_lock<std::mutex> lock(listenerMutex);
        listener = newListener;
    }
}
