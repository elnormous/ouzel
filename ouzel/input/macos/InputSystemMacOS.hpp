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
#include "input/macos/GamepadDeviceMacOS.hpp"
#include "input/macos/GamepadDeviceGC.hpp"
#include "input/macos/GamepadDeviceIOKit.hpp"
#include "input/macos/KeyboardDeviceMacOS.hpp"
#include "input/macos/MouseDeviceMacOS.hpp"
#include "input/macos/TouchpadDeviceMacOS.hpp"

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

            KeyboardDeviceMacOS* getKeyboardDevice() const { return keyboardDevice; }
            MouseDeviceMacOS* getMouseDevice() const { return mouseDevice; }
            TouchpadDeviceMacOS* getTouchpadDevice() const { return touchpadDevice; }

            void handleGamepadDiscoveryCompleted();

            void handleGamepadConnected(GCControllerPtr device);
            void handleGamepadDisconnected(GCControllerPtr device);
            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);

            void setCursorLocked(bool locked);
            bool isCursorLocked() const;

            void startGamepadDiscovery();
            void stopGamepadDiscovery();

        private:
            id connectDelegate = nil;
            IOHIDManagerRef hidManager = nullptr;

            uint32_t lastDeviceId = 0;
            KeyboardDeviceMacOS* keyboardDevice = nullptr;
            MouseDeviceMacOS* mouseDevice = nullptr;
            TouchpadDeviceMacOS* touchpadDevice = nullptr;
            std::unordered_map<GCControllerPtr, GamepadDeviceGC*> gamepadDevicesGC;
            std::unordered_map<IOHIDDeviceRef, GamepadDeviceIOKit*> gamepadDevicesIOKit;

            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    }
}
