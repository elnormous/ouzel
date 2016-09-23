// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <html5.h>
#include "WindowEm.h"

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
    
    WindowEm::WindowEm(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {
        emscripten_set_resize_callback(nullptr, this, 1, emUICallback);
        printf("callback registered\n");
    }

    WindowEm::~WindowEm()
    {
    }

    bool WindowEm::init()
    {
        emscripten_set_canvas_size(static_cast<int>(size.width),
                                   static_cast<int>(size.height));

        return Window::init();
    }

    void WindowEm::setSize(const Size2& newSize)
    {
        emscripten_set_canvas_size(static_cast<int>(newSize.width),
                                   static_cast<int>(newSize.height));

        Window::setSize(newSize);
    }

    void WindowEm::handleResize()
    {
        int width, height, fullscreen;
        emscripten_get_canvas_size(&width, &height, &fullscreen);
        Window::setSize(Size2(static_cast<float>(width), static_cast<float>(height)));
    }
}
