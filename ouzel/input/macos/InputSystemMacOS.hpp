// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
#define OUZEL_INPUT_INPUTSYSTEMMACOS_HPP

#include <memory>
#include <system_error>
#include <unordered_map>
#include <IOKit/hid/IOHIDManager.h>

#if defined(__OBJC__)
#  import <AppKit/NSCursor.h>
typedef NSCursor* NSCursorPtr;
#  include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#  include <objc/NSObjCRuntime.h>
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
        class IOKitErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept final
            {
                return "IOKit";
            }

            std::string message(int condition) const final
            {
                switch (condition)
                {
                    case kIOReturnError: return "kIOReturnError";
                    case kIOReturnNoMemory: return "kIOReturnNoMemory";
                    case kIOReturnNoResources: return "kIOReturnNoResources";
                    case kIOReturnIPCError: return "kIOReturnIPCError";
                    case kIOReturnNoDevice: return "kIOReturnNoDevice";
                    case kIOReturnNotPrivileged: return "kIOReturnNotPrivileged";
                    case kIOReturnBadArgument: return "kIOReturnBadArgument";
                    case kIOReturnLockedRead: return "kIOReturnLockedRead";
                    case kIOReturnLockedWrite: return "kIOReturnLockedWrite";
                    case kIOReturnExclusiveAccess: return "kIOReturnExclusiveAccess";
                    case kIOReturnBadMessageID: return "kIOReturnBadMessageID";
                    case kIOReturnUnsupported: return "kIOReturnUnsupported";
                    case kIOReturnVMError: return "kIOReturnVMError";
                    case kIOReturnInternalError: return "kIOReturnInternalError";
                    case kIOReturnIOError: return "kIOReturnIOError";
                    case kIOReturnCannotLock: return "kIOReturnCannotLock";
                    case kIOReturnNotOpen: return "kIOReturnNotOpen";
                    case kIOReturnNotReadable: return "kIOReturnNotReadable";
                    case kIOReturnNotWritable: return "kIOReturnNotWritable";
                    case kIOReturnNotAligned: return "kIOReturnNotAligned";
                    case kIOReturnBadMedia: return "kIOReturnBadMedia";
                    case kIOReturnStillOpen: return "kIOReturnStillOpen";
                    case kIOReturnRLDError: return "kIOReturnRLDError";
                    case kIOReturnDMAError: return "kIOReturnDMAError";
                    case kIOReturnBusy: return "kIOReturnBusy";
                    case kIOReturnTimeout: return "kIOReturnTimeout";
                    case kIOReturnOffline: return "kIOReturnOffline";
                    case kIOReturnNotReady: return "kIOReturnNotReady";
                    case kIOReturnNotAttached: return "kIOReturnNotAttached";
                    case kIOReturnNoChannels: return "kIOReturnNoChannels";
                    case kIOReturnNoSpace: return "kIOReturnNoSpace";
                    case kIOReturnPortExists: return "kIOReturnPortExists";
                    case kIOReturnCannotWire: return "kIOReturnCannotWire";
                    case kIOReturnNoInterrupt: return "kIOReturnNoInterrupt";
                    case kIOReturnNoFrames: return "kIOReturnNoFrames";
                    case kIOReturnMessageTooLarge: return "kIOReturnMessageTooLarge";
                    case kIOReturnNotPermitted: return "kIOReturnNotPermitted";
                    case kIOReturnNoPower: return "kIOReturnNoPower";
                    case kIOReturnNoMedia: return "kIOReturnNoMedia";
                    case kIOReturnUnformattedMedia: return "kIOReturnUnformattedMedia";
                    case kIOReturnUnsupportedMode: return "kIOReturnUnsupportedMode";
                    case kIOReturnUnderrun: return "kIOReturnUnderrun";
                    case kIOReturnOverrun: return "kIOReturnOverrun";
                    case kIOReturnDeviceError: return "kIOReturnDeviceError";
                    case kIOReturnNoCompletion: return "kIOReturnNoCompletion";
                    case kIOReturnAborted: return "kIOReturnAborted";
                    case kIOReturnNoBandwidth: return "kIOReturnNoBandwidth";
                    case kIOReturnNotResponding: return "kIOReturnNotResponding";
                    case kIOReturnIsoTooOld: return "kIOReturnIsoTooOld";
                    case kIOReturnIsoTooNew: return "kIOReturnIsoTooNew";
                    case kIOReturnNotFound: return "kIOReturnNotFound";
                    case kIOReturnInvalid: return "kIOReturnInvalid";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        extern const IOKitErrorCategory ioKitErrorCategory;

        class CursorMacOS;

        class InputSystemMacOS final: public InputSystem
        {
        public:
            explicit InputSystemMacOS(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemMacOS();

            void executeCommand(const Command& command) final;

            inline KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }
            inline MouseDeviceMacOS* getMouseDevice() const { return mouseDevice.get(); }
            inline TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

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

            std::vector<std::unique_ptr<CursorMacOS>> cursors;

            unsigned char emptyCursorData[4] = {0, 0, 0, 0};
            NSCursorPtr emptyCursor = nil;
            NSCursorPtr defaultCursor = nil;
        };
    }
}

#endif // OUZEL_INPUT_INPUTSYSTEMMACOS_HPP
