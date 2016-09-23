// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include <html5.h>
#include "InputEm.h"

EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    return 1;
}

EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    return 1;
}

EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    return 1;
}

namespace ouzel
{
    namespace input
    {
        InputEm::InputEm()
        {
            emscripten_set_keypress_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keydown_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keyup_callback(nullptr, this, 1, emKeyCallback);

            emscripten_set_mousedown_callback(nullptr, this, 1, emMouseCallback);
            emscripten_set_mouseup_callback(nullptr, this, 1, emMouseCallback);
            emscripten_set_mousemove_callback(nullptr, this, 1, emMouseCallback);

            emscripten_set_wheel_callback(nullptr, this, 1, emWheelCallback);
        }

        InputEm::~InputEm()
        {
        }
    } // namespace input
} // namespace ouzel
