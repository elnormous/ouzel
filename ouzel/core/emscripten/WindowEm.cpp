// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <emscripten/html5.h>
#include "WindowEm.h"
#include "core/Engine.h"

static EM_BOOL emUICallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_RESIZE)
    {
        reinterpret_cast<ouzel::WindowEm*>(userData)->handleResize();
        return true;
    }

    return false;
}

namespace ouzel
{
    WindowEm::WindowEm(const Size2& aSize,
                       bool aResizable,
                       bool aFullscreen,
                       const std::string& aTitle,
                       bool aHighDpi):
        Window(aSize, aResizable, aFullscreen, aTitle, aHighDpi)
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

        Event event;
        event.type = Event::Type::WINDOW_SIZE_CHANGE;

        event.windowEvent.window = this;
        event.windowEvent.size = Size2(static_cast<float>(width),
                                       static_cast<float>(height));

        sharedEngine->getEventDispatcher()->postEvent(event);
    }
}
