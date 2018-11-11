// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
#define OUZEL_INPUT_INPUTSYSTEMMACOS_HPP

#include <memory>
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
#include "input/macos/MouseDeviceMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorMacOS;

        class InputSystemMacOS final: public InputSystem
        {
        public:
            explicit InputSystemMacOS(const std::function<bool(const Event&)>& initCallback);
            ~InputSystemMacOS();

            void executeCommand(const Command& command) override;

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceMacOS* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            void handleGamepadDiscoveryCompleted();

            void handleGamepadConnected(GCControllerPtr device);
            void handleGamepadDisconnected(GCControllerPtr device);
            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);

            NSCursorPtr getCursor() const;

        private:
            void startGamepadDiscovery();
            void stopGamepadDiscovery();

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<MouseDeviceMacOS> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;
            std::unordered_map<GCControllerPtr, std::unique_ptr<GamepadDeviceGC>> gamepadDevicesGC;
            std::unordered_map<IOHIDDeviceRef, std::unique_ptr<GamepadDeviceIOKit>> gamepadDevicesIOKit;

            id connectDelegate = nil;
            IOHIDManagerRef hidManager = nullptr;

            std::vector<std::unique_ptr<NativeCursorMacOS>> cursors;

            unsigned char emptyCursorData[4] = {0, 0, 0, 0};
            NSCursorPtr emptyCursor = nil;
            NSCursorPtr defaultCursor = nil;
        };
    }
}

#endif // OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
