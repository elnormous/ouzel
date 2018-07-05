// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "NativeWindow.hpp"
#include "thread/Lock.hpp"

namespace ouzel
{
    NativeWindow::Listener::~Listener()
    {
    }

    void NativeWindow::init(const Size2& newSize,
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

    void NativeWindow::setListener(Listener* newListener)
    {
        Lock lock(listenerMutex);
        listener = newListener;
    }
}
