// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Keyboard.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Keyboard::Keyboard(InputManager& initInputManager, uint32_t initDeviceId):
            Controller(initInputManager, Controller::Type::KEYBOARD, initDeviceId)
        {
            std::fill(std::begin(keyStates), std::end(keyStates), false);
        }

        void Keyboard::handleKeyPress(Keyboard::Key key, uint32_t modifiers)
        {
            Event event;
            event.keyboardEvent.keyboard = this;
            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            if (!keyStates[static_cast<uint32_t>(key)])
            {
                keyStates[static_cast<uint32_t>(key)] = true;

                event.type = Event::Type::KEY_PRESS;
                engine->getEventDispatcher().postEvent(event, true);
            }
            else
            {
                event.type = Event::Type::KEY_REPEAT;
                engine->getEventDispatcher().postEvent(event, true);
            }
        }

        void Keyboard::handleKeyRelease(Keyboard::Key key, uint32_t modifiers)
        {
            keyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.type = Event::Type::KEY_RELEASE;
            event.keyboardEvent.keyboard = this;
            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            engine->getEventDispatcher().postEvent(event, true);
        }
    } // namespace input
} // namespace ouzel
