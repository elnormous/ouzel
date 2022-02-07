// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
#define OUZEL_INPUT_INPUTSYSTEMMACOS_HPP

#include <memory>
#include <unordered_map>

#ifdef __OBJC__
#  import <AppKit/NSCursor.h>
using NSCursorPtr = NSCursor*;
#  include <GameController/GameController.h>
using GCControllerPtr = GCController*;
#else
#  include <objc/NSObjCRuntime.h>
using NSCursorPtr = id;
using GCControllerPtr = id;
#endif

#include "../InputSystem.hpp"
#include "../Keyboard.hpp"
#include "GamepadDeviceMacOS.hpp"
#include "GamepadDeviceGC.hpp"
#include "GamepadDeviceIOKit.hpp"
#include "MouseDeviceMacOS.hpp"
#include "../../platform/iokit/HidManager.hpp"

namespace ouzel::input::macos
{
    class Cursor;

    class InputSystem final: public input::InputSystem
    {
    public:
        InputSystem();
        ~InputSystem() override;

        auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
        auto getMouseDevice() const noexcept { return mouseDevice.get(); }
        auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

        void handleGamepadDiscoveryCompleted();

        void handleGamepadConnected(GCControllerPtr device);
        void handleGamepadDisconnected(GCControllerPtr device);
        void handleGamepadConnected(IOHIDDeviceRef device);
        void handleGamepadDisconnected(IOHIDDeviceRef device);

        NSCursorPtr getCursor() const;

    private:
        void executeCommand(const Command& command) final;

        auto getNextDeviceId() noexcept
        {
            ++lastDeviceId.value;
            return lastDeviceId;
        }

        void startGamepadDiscovery();
        void stopGamepadDiscovery();

        DeviceId lastDeviceId;
        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<MouseDevice> mouseDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;
        std::unordered_map<GCControllerPtr, std::unique_ptr<GamepadDeviceGC>> gamepadDevicesGC;
        std::unordered_map<IOHIDDeviceRef, std::unique_ptr<GamepadDeviceIOKit>> gamepadDevicesIOKit;

        id connectDelegate = nil;
        platform::iokit::HidManager hidManager;

        std::vector<std::unique_ptr<Cursor>> cursors;

        unsigned char emptyCursorData[4] = {0, 0, 0, 0};
        NSCursorPtr emptyCursor = nil;
        NSCursorPtr defaultCursor = nil;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
