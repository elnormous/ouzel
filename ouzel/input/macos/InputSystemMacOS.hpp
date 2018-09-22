// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <unordered_map>
#include <IOKit/hid/IOHIDManager.h>

#if defined(__OBJC__)
#import <AppKit/NSCursor.h>
typedef NSCursor* NSCursorPtr;
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/NSObjCRuntime.h>
typedef id NSCursorPtr;
typedef id GCControllerPtr;
#endif

#include "input/InputSystem.hpp"
#include "input/macos/GamepadMacOS.hpp"
#include "input/macos/GamepadGC.hpp"
#include "input/macos/GamepadIOKit.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS: public InputSystem
        {
        public:
            InputSystemMacOS();
            virtual ~InputSystemMacOS();

            void handleGamepadDiscoveryCompleted();

            void handleGamepadConnected(GCControllerPtr device);
            void handleGamepadDisconnected(GCControllerPtr device);
            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);

            void setCursorLocked(bool locked);
            bool isCursorLocked() const;

            void setCursorPosition(const Vector2& position);

            void startDeviceDiscovery();
            void stopDeviceDiscovery();

            void handleButtonValueChange(const GamepadMacOS& gamepad, Gamepad::Button button, bool pressed, float value);

        private:
            id connectDelegate = nil;
            IOHIDManagerRef hidManager = nullptr;

            uint32_t lastDeviceId = 0;
            std::unordered_map<uint32_t, std::unique_ptr<GamepadMacOS>> gamepads;
            std::unordered_map<GCControllerPtr, GamepadGC*> gamepadsGC;
            std::unordered_map<IOHIDDeviceRef, GamepadIOKit*> gamepadsIOKit;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    }
}
