// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseMacOS.hpp"
#include "InputSystemMacOS.hpp"
#include "core/Engine.hpp"
#include "core/macos/NativeWindowMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseMacOS::setPosition(const Vector2& position)
        {
            ouzel::Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

            CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.x,
                                                  screenOrigin.y + windowOrigin.y + windowLocation.y));
        }

        void MouseMacOS::handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_PRESS;
            event.deviceId = deviceId;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_RELEASE;
            event.deviceId = deviceId;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleMove(const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_MOVE;
            event.deviceId = deviceId;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_SCROLL;
            event.deviceId = deviceId;
            event.position = position;
            event.scroll = scroll;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
