// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <html5.h>
#include "WindowEm.h"
#include "core/Engine.h"

namespace ouzel
{
    EM_BOOL emUICallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
    {
        if (eventType == EMSCRIPTEN_EVENT_RESIZE)
        {
            reinterpret_cast<WindowEm*>(userData)->handleResize();
            return true;
        }

        return false;
    }

    WindowEm::WindowEm(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
    {
        emscripten_set_resize_callback(nullptr, this, 1, emUICallback);
    }

    bool WindowEm::init()
    {
        if (size.v[0] <= 0.0f || size.v[1] <= 0.0f)
        {
            int width, height, fullscreen;
            emscripten_get_canvas_size(&width, &height, &fullscreen);

            if (size.v[0] <= 0.0f) size.v[0] = static_cast<float>(width);
            if (size.v[1] <= 0.0f) size.v[1] = static_cast<float>(height);
        }

        emscripten_set_canvas_size(static_cast<int>(size.v[0]),
                                   static_cast<int>(size.v[1]));

        return Window::init();
    }

    void WindowEm::setSize(const Size2& newSize)
    {
        emscripten_set_canvas_size(static_cast<int>(newSize.v[0]),
                                   static_cast<int>(newSize.v[1]));

        Window::setSize(newSize);
    }

    void WindowEm::handleResize()
    {
        int width, height, fullscreen;
        emscripten_get_canvas_size(&width, &height, &fullscreen);
        Window::setSize(Size2(static_cast<float>(width), static_cast<float>(height)));

        sharedEngine->getRenderer()->setSize(size * getContentScale());
    }
}
