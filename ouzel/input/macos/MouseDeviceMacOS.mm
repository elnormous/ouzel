// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"
#include "core/Engine.hpp"
#include "core/macos/NativeWindowMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceMacOS::setPosition(const Vector2& position)
        {
            ouzel::Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

            CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.x,
                                                  screenOrigin.y + windowOrigin.y + windowLocation.y));
        }

        void MouseDeviceMacOS::handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_PRESS;
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseDeviceMacOS::handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_RELEASE;
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseDeviceMacOS::handleMove(const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_MOVE;
            event.deviceId = id;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseDeviceMacOS::handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_SCROLL;
            event.deviceId = id;
            event.position = position;
            event.scroll = scroll;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
