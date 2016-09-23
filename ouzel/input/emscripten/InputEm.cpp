// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include "InputEm.h"
#include "core/Engine.h"
#include "events/Event.h"

EM_BOOL emKeyCallback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
        case EMSCRIPTEN_EVENT_KEYDOWN:
            ouzel::sharedEngine->getInput()->keyDown(ouzel::input::InputEm::convertKeyCode(keyEvent->key),
                                                     ouzel::input::InputEm::getKeyboardModifiers(keyEvent));
            break;
        case EMSCRIPTEN_EVENT_KEYUP:
            ouzel::sharedEngine->getInput()->keyUp(ouzel::input::InputEm::convertKeyCode(keyEvent->key),
                                                   ouzel::input::InputEm::getKeyboardModifiers(keyEvent));
            break;
    }

    return 1;
}

EM_BOOL emMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    ouzel::input::MouseButton button;

    switch (mouseEvent->button)
    {
        case 0:
            button = ouzel::input::MouseButton::LEFT;
            break;
        case 1:
            button = ouzel::input::MouseButton::RIGHT;
            break;
        case 2:
            button = ouzel::input::MouseButton::MIDDLE;
            break;
        default:
            button = ouzel::input::MouseButton::NONE;
            break;
    }

    ouzel::Vector2 position(static_cast<float>(mouseEvent->clientX),
                            static_cast<float>(mouseEvent->clientY));

    

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
            ouzel::sharedEngine->getInput()->mouseDown(button,
                                                       ouzel::sharedEngine->getRenderer()->viewToScreenLocation(position),
                                                       ouzel::input::InputEm::getMouseModifiers(mouseEvent));
            break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
            ouzel::sharedEngine->getInput()->mouseUp(button,
                                                     ouzel::sharedEngine->getRenderer()->viewToScreenLocation(position),
                                                     ouzel::input::InputEm::getMouseModifiers(mouseEvent));
            break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
            ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getRenderer()->viewToScreenLocation(position),
                                                       ouzel::input::InputEm::getMouseModifiers(mouseEvent));
        break;
    }   

    return 1;
}

EM_BOOL emWheelCallback(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
{
    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        ouzel::Vector2 position(static_cast<float>(wheelEvent->mouse.clientX),
                                static_cast<float>(wheelEvent->mouse.clientY));

        ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY)),
                                                     ouzel::sharedEngine->getRenderer()->viewToScreenLocation(position),
                                                     ouzel::input::InputEm::getMouseModifiers(&wheelEvent->mouse));
    }

    return 1;
}

namespace ouzel
{
    namespace input
    {
        static std::map<std::string, KeyboardKey> keyMap;

        InputEm::InputEm()
        {
            emscripten_set_keypress_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keydown_callback(nullptr, this, 1, emKeyCallback);
            emscripten_set_keyup_callback(nullptr, this, 1, emKeyCallback);

            emscripten_set_mousedown_callback("#canvas", this, 1, emMouseCallback);
            emscripten_set_mouseup_callback("#canvas", this, 1, emMouseCallback);
            emscripten_set_mousemove_callback("#canvas", this, 1, emMouseCallback);

            emscripten_set_wheel_callback("#canvas", this, 1, emWheelCallback);
        }

        InputEm::~InputEm()
        {
        }
        
        KeyboardKey InputEm::convertKeyCode(const EM_UTF8 key[32])
        {
            auto i = keyMap.find(key);

            if (i != keyMap.end())
            {
                return i->second;
            }
            else
            {
                return KeyboardKey::NONE;
            }
        }

        uint32_t InputEm::getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent)
        {
            uint32_t modifiers = 0;

            if (keyboardEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (keyboardEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (keyboardEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (keyboardEvent->metaKey) modifiers |= ouzel::COMMAND_DOWN;

            return modifiers;
        }

        uint32_t InputEm::getMouseModifiers(const EmscriptenMouseEvent* mouseEvent)
        {
            uint32_t modifiers = 0;

            if (mouseEvent->ctrlKey) modifiers |= ouzel::CONTROL_DOWN;
            if (mouseEvent->shiftKey) modifiers |= ouzel::SHIFT_DOWN;
            if (mouseEvent->altKey) modifiers |= ouzel::ALT_DOWN;
            if (mouseEvent->metaKey) modifiers |= ouzel::COMMAND_DOWN;

            return modifiers;
        }
    } // namespace input
} // namespace ouzel
