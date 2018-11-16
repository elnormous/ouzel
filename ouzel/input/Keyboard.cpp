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

        bool Keyboard::handleKeyPress(Keyboard::Key key)
        {
            std::unique_ptr<KeyboardEvent> event(new KeyboardEvent());
            event->keyboard = this;
            event->key = key;

            if (!keyStates[static_cast<uint32_t>(key)])
            {
                keyStates[static_cast<uint32_t>(key)] = true;

                event->type = Event::Type::KEY_PRESS;
                return engine->getEventDispatcher().dispatchEvent(std::move(event));
            }

            return false;
        }

        bool Keyboard::handleKeyRelease(Keyboard::Key key)
        {
            keyStates[static_cast<uint32_t>(key)] = false;

            std::unique_ptr<KeyboardEvent> event(new KeyboardEvent());
            event->type = Event::Type::KEY_RELEASE;
            event->keyboard = this;
            event->key = key;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    } // namespace input
} // namespace ouzel
