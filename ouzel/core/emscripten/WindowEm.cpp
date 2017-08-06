// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <emscripten/html5.h>
#include "WindowEm.hpp"
#include "core/Engine.hpp"

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
    WindowEm::WindowEm()
    {
        emscripten_set_resize_callback(nullptr, this, 1, emUICallback);
    }

    bool WindowEm::init(const Size2& newSize,
                        bool newResizable,
                        bool newFullscreen,
                        const std::string& newTitle,
                        bool newHighDpi,
                        bool depth)
    {
        if (!Window::init(newSize, newResizable, newFullscreen, newTitle, newHighDpi, depth))
        {
            return false;
        }

        if (size.v[0] <= 0.0f || size.v[1] <= 0.0f)
        {
            int width, height, fullscreen;
            emscripten_get_canvas_size(&width, &height, &fullscreen);

            if (size.v[0] <= 0.0f) size.v[0] = static_cast<float>(width);
            if (size.v[1] <= 0.0f) size.v[1] = static_cast<float>(height);
        }

        emscripten_set_canvas_size(static_cast<int>(size.v[0]),
                                   static_cast<int>(size.v[1]));

        return true;
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
