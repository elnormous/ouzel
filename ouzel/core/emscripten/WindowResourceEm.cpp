// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <emscripten/html5.h>
#include "WindowResourceEm.hpp"
#include "core/Engine.hpp"

static EM_BOOL emUICallback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_RESIZE)
    {
        reinterpret_cast<ouzel::WindowResourceEm*>(userData)->handleResize();
        return true;
    }

    return false;
}

namespace ouzel
{
    WindowResourceEm::WindowResourceEm()
    {
        emscripten_set_resize_callback(nullptr, this, 1, emUICallback);
    }

    void WindowResourceEm::init(const Size2& newSize,
                                bool newResizable,
                                bool newFullscreen,
                                bool newExclusiveFullscreen,
                                const std::string& newTitle,
                                bool newHighDpi,
                                bool depth)
    {
        WindowResource::init(newSize,
                             newResizable,
                             newFullscreen,
                             newExclusiveFullscreen,
                             newTitle,
                             newHighDpi,
                             depth);

        if (size.width <= 0.0F || size.height <= 0.0F)
        {
            int width, height, fullscreen;
            emscripten_get_canvas_size(&width, &height, &fullscreen);

            if (size.width <= 0.0F) size.width = static_cast<float>(width);
            if (size.height <= 0.0F) size.height = static_cast<float>(height);
        }

        resolution = size;

        emscripten_set_canvas_size(static_cast<int>(size.width),
                                   static_cast<int>(size.height));
    }

    void WindowResourceEm::setSize(const Size2& newSize)
    {
        WindowResource::setSize(newSize);

        emscripten_set_canvas_size(static_cast<int>(newSize.width),
                                   static_cast<int>(newSize.height));
    }

    void WindowResourceEm::handleResize()
    {
        int width, height, fullscreen;
        emscripten_get_canvas_size(&width, &height, &fullscreen);

        Size2 newSize(static_cast<float>(width), static_cast<float>(height));

        size = newSize;
        resolution = size;

        if (listener)
        {
            listener->onSizeChange(size);
            listener->onResolutionChange(resolution);
        }
    }
}
