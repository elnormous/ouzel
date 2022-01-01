// Ouzel by Elviss Strazdins

#include <iterator>
#include "Keyboard.hpp"
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"

namespace ouzel::input
{
    Keyboard::Keyboard(InputManager& initInputManager, DeviceId initDeviceId):
        Controller{initInputManager, Controller::Type::keyboard, initDeviceId}
    {
    }

    bool Keyboard::handleKeyPress(Keyboard::Key key)
    {
        auto event = std::make_unique<KeyboardEvent>();
        event->keyboard = this;
        event->key = key;

        if (!keyStates[static_cast<std::size_t>(key)])
        {
            keyStates[static_cast<std::size_t>(key)] = true;

            event->type = Event::Type::keyboardKeyPress;
            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        return false;
    }

    bool Keyboard::handleKeyRelease(Keyboard::Key key)
    {
        keyStates[static_cast<std::size_t>(key)] = false;

        auto event = std::make_unique<KeyboardEvent>();
        event->type = Event::Type::keyboardKeyRelease;
        event->keyboard = this;
        event->key = key;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }
}
