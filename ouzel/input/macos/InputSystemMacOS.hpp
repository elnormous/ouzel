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
#include "input/Keyboard.hpp"
#include "input/macos/GamepadMacOS.hpp"
#include "input/macos/GamepadGC.hpp"
#include "input/macos/GamepadIOKit.hpp"
#include "input/macos/KeyboardMacOS.hpp"
#include "input/macos/MouseMacOS.hpp"
#include "input/macos/TouchpadMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(uint16_t keyCode);
            static NSUInteger getKeyMask(uint16_t keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);

            InputSystemMacOS();
            virtual ~InputSystemMacOS();

            virtual void executeCommand(Command command) override;

            KeyboardMacOS* getKeyboard() const { return keyboard; }
            MouseMacOS* getMouse() const { return mouse; }
            TouchpadMacOS* getTouchpad() const { return touchpad; }

            void handleGamepadDiscoveryCompleted();

            void handleGamepadConnected(GCControllerPtr device);
            void handleGamepadDisconnected(GCControllerPtr device);
            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);

            void setCursorLocked(bool locked);
            bool isCursorLocked() const;

            void setCursorPosition(const Vector2& position);

            void startGamepadDiscovery();
            void stopGamepadDiscovery();

            void handleButtonValueChange(const GamepadMacOS& gamepad, Gamepad::Button button, bool pressed, float value);

        private:
            id connectDelegate = nil;
            IOHIDManagerRef hidManager = nullptr;

            uint32_t lastDeviceId = 0;
            KeyboardMacOS* keyboard = nullptr;
            MouseMacOS* mouse = nullptr;
            TouchpadMacOS* touchpad = nullptr;
            std::unordered_map<GCControllerPtr, GamepadGC*> gamepadsGC;
            std::unordered_map<IOHIDDeviceRef, GamepadIOKit*> gamepadsIOKit;

            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    }
}
