// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <map>
#import <Carbon/Carbon.h>
#import <objc/message.h>
#include "InputSystemMacOS.hpp"
#include "events/Event.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

typedef struct CF_BRIDGED_TYPE(id) __IOHIDServiceClient * IOHIDServiceClientRef;
extern "C" CFTypeRef _Nullable IOHIDServiceClientCopyProperty(IOHIDServiceClientRef service, CFStringRef key);

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputSystemMacOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputSystemMacOS*)initInput
{
    if (self = [super init])
        input = initInput;

        return self;
}

-(void)handleControllerConnected:(NSNotification*)notification
{
    input->handleGamepadConnected(notification.object);
}

-(void)handleControllerDisconnected:(NSNotification*)notification
{
    input->handleGamepadDisconnected(notification.object);
}

@end

static void deviceAdded(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputSystemMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputSystemMacOS*>(ctx);
    inputMacOS->handleGamepadConnected(device);
}

static void deviceRemoved(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputSystemMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputSystemMacOS*>(ctx);
    inputMacOS->handleGamepadDisconnected(device);
}

#if !defined(__MAC_10_12) || __MAC_OS_X_VERSION_MAX_ALLOWED < __MAC_10_12
enum
{
    kVK_RightCommand = 0x36
};
#endif

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<uint16_t, Keyboard::Key> keyMap = {
            {kVK_UpArrow, Keyboard::Key::UP},
            {kVK_DownArrow, Keyboard::Key::DOWN},
            {kVK_LeftArrow, Keyboard::Key::LEFT},
            {kVK_RightArrow, Keyboard::Key::RIGHT},
            {kVK_F1, Keyboard::Key::F1},
            {kVK_F2, Keyboard::Key::F2},
            {kVK_F3, Keyboard::Key::F3},
            {kVK_F4, Keyboard::Key::F4},
            {kVK_F5, Keyboard::Key::F5},
            {kVK_F6, Keyboard::Key::F6},
            {kVK_F7, Keyboard::Key::F7},
            {kVK_F8, Keyboard::Key::F8},
            {kVK_F9, Keyboard::Key::F9},
            {kVK_F10, Keyboard::Key::F10},
            {kVK_F11, Keyboard::Key::F11},
            {kVK_F12, Keyboard::Key::F12},
            {kVK_F13, Keyboard::Key::F13},
            {kVK_F14, Keyboard::Key::F14},
            {kVK_F15, Keyboard::Key::F15},
            {kVK_F16, Keyboard::Key::F16},
            {kVK_F17, Keyboard::Key::F17},
            {kVK_F18, Keyboard::Key::F18},
            {kVK_F19, Keyboard::Key::F19},
            {kVK_F20, Keyboard::Key::F20},
            {kVK_Home, Keyboard::Key::HOME},
            {kVK_End, Keyboard::Key::END},
            {NSInsertFunctionKey, Keyboard::Key::INSERT},
            {kVK_ForwardDelete, Keyboard::Key::DEL},
            {kVK_Help, Keyboard::Key::HELP},
            {NSSelectFunctionKey, Keyboard::Key::SELECT},
            {NSPrintFunctionKey, Keyboard::Key::PRINT},
            {NSExecuteFunctionKey, Keyboard::Key::EXECUTE},
            {NSPrintScreenFunctionKey, Keyboard::Key::SNAPSHOT},
            {NSPauseFunctionKey, Keyboard::Key::PAUSE},
            {NSScrollLockFunctionKey, Keyboard::Key::SCROLL},
            {kVK_Delete, Keyboard::Key::BACKSPACE},
            {kVK_Tab, Keyboard::Key::TAB},
            {kVK_Return, Keyboard::Key::RETURN},
            {kVK_Escape, Keyboard::Key::ESCAPE},
            {kVK_Control, Keyboard::Key::LEFT_CONTROL},
            {kVK_RightControl, Keyboard::Key::RIGHT_CONTROL},
            {kVK_Command, Keyboard::Key::LEFT_SUPER},
            {kVK_RightCommand, Keyboard::Key::RIGHT_SUPER},
            {kVK_Shift, Keyboard::Key::LEFT_SHIFT},
            {kVK_RightShift, Keyboard::Key::RIGHT_SHIFT},
            {kVK_Option, Keyboard::Key::LEFT_ALT},
            {kVK_RightOption, Keyboard::Key::RIGHT_ALT},
            {kVK_Space, Keyboard::Key::SPACE},

            {kVK_ANSI_A, Keyboard::Key::A},
            {kVK_ANSI_B, Keyboard::Key::B},
            {kVK_ANSI_C, Keyboard::Key::C},
            {kVK_ANSI_D, Keyboard::Key::D},
            {kVK_ANSI_E, Keyboard::Key::E},
            {kVK_ANSI_F, Keyboard::Key::F},
            {kVK_ANSI_G, Keyboard::Key::G},
            {kVK_ANSI_H, Keyboard::Key::H},
            {kVK_ANSI_I, Keyboard::Key::I},
            {kVK_ANSI_J, Keyboard::Key::J},
            {kVK_ANSI_K, Keyboard::Key::K},
            {kVK_ANSI_L, Keyboard::Key::L},
            {kVK_ANSI_M, Keyboard::Key::M},
            {kVK_ANSI_N, Keyboard::Key::N},
            {kVK_ANSI_O, Keyboard::Key::O},
            {kVK_ANSI_P, Keyboard::Key::P},
            {kVK_ANSI_Q, Keyboard::Key::Q},
            {kVK_ANSI_R, Keyboard::Key::R},
            {kVK_ANSI_S, Keyboard::Key::S},
            {kVK_ANSI_T, Keyboard::Key::T},
            {kVK_ANSI_U, Keyboard::Key::U},
            {kVK_ANSI_V, Keyboard::Key::V},
            {kVK_ANSI_W, Keyboard::Key::W},
            {kVK_ANSI_X, Keyboard::Key::X},
            {kVK_ANSI_Y, Keyboard::Key::Y},
            {kVK_ANSI_Z, Keyboard::Key::Z},

            {kVK_ANSI_0, Keyboard::Key::NUM_0},
            {kVK_ANSI_1, Keyboard::Key::NUM_1},
            {kVK_ANSI_2, Keyboard::Key::NUM_2},
            {kVK_ANSI_3, Keyboard::Key::NUM_3},
            {kVK_ANSI_4, Keyboard::Key::NUM_4},
            {kVK_ANSI_5, Keyboard::Key::NUM_5},
            {kVK_ANSI_6, Keyboard::Key::NUM_6},
            {kVK_ANSI_7, Keyboard::Key::NUM_7},
            {kVK_ANSI_8, Keyboard::Key::NUM_8},
            {kVK_ANSI_9, Keyboard::Key::NUM_9},

            {kVK_ANSI_Comma, Keyboard::Key::COMMA},
            {kVK_ANSI_Period, Keyboard::Key::PERIOD},
            {kVK_PageUp, Keyboard::Key::PRIOR},
            {kVK_PageDown, Keyboard::Key::NEXT},

            {kVK_ANSI_Keypad0, Keyboard::Key::NUMPAD_0},
            {kVK_ANSI_Keypad1, Keyboard::Key::NUMPAD_1},
            {kVK_ANSI_Keypad2, Keyboard::Key::NUMPAD_2},
            {kVK_ANSI_Keypad3, Keyboard::Key::NUMPAD_3},
            {kVK_ANSI_Keypad4, Keyboard::Key::NUMPAD_4},
            {kVK_ANSI_Keypad5, Keyboard::Key::NUMPAD_5},
            {kVK_ANSI_Keypad6, Keyboard::Key::NUMPAD_6},
            {kVK_ANSI_Keypad7, Keyboard::Key::NUMPAD_7},
            {kVK_ANSI_Keypad8, Keyboard::Key::NUMPAD_8},
            {kVK_ANSI_Keypad9, Keyboard::Key::NUMPAD_9},

            {kVK_ANSI_KeypadDecimal, Keyboard::Key::DECIMAL},
            {kVK_ANSI_KeypadMultiply, Keyboard::Key::MULTIPLY},
            {kVK_ANSI_KeypadPlus, Keyboard::Key::PLUS},
            {kVK_ANSI_KeypadClear, Keyboard::Key::OEM_CLEAR},
            {kVK_ANSI_KeypadDivide, Keyboard::Key::DIVIDE},
            {kVK_ANSI_KeypadEnter, Keyboard::Key::RETURN},
            {kVK_ANSI_KeypadMinus, Keyboard::Key::SUBTRACT},

            {kVK_ANSI_Semicolon, Keyboard::Key::SEMICOLON},
            {kVK_ANSI_Slash, Keyboard::Key::SLASH},
            {kVK_ANSI_Grave, Keyboard::Key::GRAVE},
            {kVK_ANSI_LeftBracket, Keyboard::Key::LEFT_BRACKET},
            {kVK_ANSI_Backslash, Keyboard::Key::BACKSLASH},
            {kVK_ANSI_RightBracket, Keyboard::Key::RIGHT_BRACKET}
        };

        static const std::map<uint16_t, NSUInteger> maskMap = {
            {kVK_Control, NX_DEVICELCTLKEYMASK},
            {kVK_RightControl, NX_DEVICERCTLKEYMASK},
            {kVK_Command, NX_DEVICELCMDKEYMASK},
            {kVK_RightCommand, NX_DEVICERCMDKEYMASK},
            {kVK_Shift, NX_DEVICELSHIFTKEYMASK},
            {kVK_RightShift, NX_DEVICERSHIFTKEYMASK},
            {kVK_Option, NX_DEVICELALTKEYMASK},
            {kVK_RightOption, NX_DEVICERALTKEYMASK}
        };

        Keyboard::Key InputSystemMacOS::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        NSUInteger InputSystemMacOS::getKeyMask(uint16_t keyCode)
        {
            auto i = maskMap.find(keyCode);

            if (i != maskMap.end())
                return i->second;
            else
                return 0;
        }

        uint32_t InputSystemMacOS::getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons)
        {
            uint32_t modifiers = 0;

            if (modifierFlags & NSShiftKeyMask) modifiers |= SHIFT_DOWN;
            if (modifierFlags & NSAlternateKeyMask) modifiers |= ALT_DOWN;
            if (modifierFlags & NSControlKeyMask) modifiers |= CONTROL_DOWN;
            if (modifierFlags & NSCommandKeyMask) modifiers |= SUPER_DOWN;
            if (modifierFlags & NSFunctionKeyMask) modifiers |= FUNCTION_DOWN;

            if (pressedMouseButtons)
            {
                if (pressedMouseButtons & (1 << 0)) modifiers |= LEFT_MOUSE_DOWN;
                if (pressedMouseButtons & (1 << 1)) modifiers |= RIGHT_MOUSE_DOWN;
                if (pressedMouseButtons & (1 << 2)) modifiers |= MIDDLE_MOUSE_DOWN;
            }

            return modifiers;
        }

        InputSystemMacOS::InputSystemMacOS()
        {
            connectDelegate = [[ConnectDelegate alloc] initWithInput:this];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerConnected:)
                                                         name:GCControllerDidConnectNotification
                                                       object:nil];

            [[NSNotificationCenter defaultCenter] addObserver:connectDelegate
                                                     selector:@selector(handleControllerDisconnected:)
                                                         name:GCControllerDidDisconnectNotification
                                                       object:nil];

            for (GCController* controller in [GCController controllers])
                handleGamepadConnected(controller);

            NSArray* criteria = @[
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_Joystick)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_GamePad)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey : @(kHIDUsage_GD_MultiAxisController)}
                                  ];

            hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

            IOHIDManagerSetDeviceMatchingMultiple(hidManager, (CFArrayRef)criteria);
            IOReturn ret = IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
            if (ret != kIOReturnSuccess)
                throw SystemError("Failed to initialize HID manager, error: " + std::to_string(ret));

            IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAdded, this);
            IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemoved, this);
            IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];

            Event keyboardConnectEvent;
            keyboardConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<KeyboardDevice> keyboard(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardConnectEvent.deviceId = keyboard->getId();
            keyboardConnectEvent.deviceType = Controller::Type::KEYBOARD;
            keyboardDevice = keyboard.get();
            inputDevices.insert(std::make_pair(keyboard->getId(), std::move(keyboard)));
            addEvent(keyboardConnectEvent);

            Event mouseConnectEvent;
            mouseConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<MouseDeviceMacOS> mouse(new MouseDeviceMacOS(*this, ++lastDeviceId));
            mouseConnectEvent.deviceId = mouse->getId();
            mouseConnectEvent.deviceType = Controller::Type::MOUSE;
            mouseDevice = mouse.get();
            inputDevices.insert(std::make_pair(mouse->getId(), std::move(mouse)));
            addEvent(mouseConnectEvent);

            Event touchpadConnectEvent;
            touchpadConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<TouchpadDevice> touchpad(new TouchpadDevice(*this, ++lastDeviceId));
            touchpadConnectEvent.deviceId = touchpad->getId();
            touchpadConnectEvent.deviceType = Controller::Type::TOUCHPAD;
            touchpadDevice = touchpad.get();
            inputDevices.insert(std::make_pair(touchpad->getId(), std::move(touchpad)));
            addEvent(touchpadConnectEvent);
        }

        InputSystemMacOS::~InputSystemMacOS()
        {
            if (connectDelegate)
                [connectDelegate release];

            if (hidManager)
            {
                IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
                CFRelease(hidManager);
            }
        }

        void InputSystemMacOS::executeCommand(Command command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    startGamepadDiscovery();
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    stopGamepadDiscovery();
                    break;
                case Command::Type::SET_ABSOLUTE_DPAD_VALUES:
                {
                    break;
                }
                case Command::Type::SET_PLAYER_INDEX:
                {
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITON:
                {
                    auto i = inputDevices.find(command.deviceId);

                    if (i != inputDevices.end())
                    {
                        InputDevice* device = i->second.get();

                        if (device == mouseDevice)
                            mouseDevice->setPosition(command.position);
                    }
                    break;
                }
                case Command::Type::SET_CURSOR:
                {
                    break;
                }
                case Command::Type::SET_CURSOR_VISIBLE:
                {
                    break;
                }
                case Command::Type::SET_CURSOR_LOCKED:
                {
                    break;
                }
                default:
                    break;
            }
        }

        void InputSystemMacOS::setCursorLocked(bool locked)
        {
            /*engine->executeOnMainThread([locked] {
                CGAssociateMouseAndMouseCursorPosition(!locked);
            });
            cursorLocked = locked;*/
        }

        bool InputSystemMacOS::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputSystemMacOS::startGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputSystemMacOS::stopGamepadDiscovery()
        {
            Log(Log::Level::INFO) << "Stopped gamepad discovery";

            [GCController stopWirelessControllerDiscovery];
        }

        void InputSystemMacOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";

            Event event;
            event.type = Event::Type::DEVICE_DISCOVERY_COMPLETE;
            addEvent(event);
        }

        void InputSystemMacOS::handleGamepadConnected(GCControllerPtr controller)
        {
            int32_t vendorId = 0;
            int32_t productId = 0;

            if (class_respondsToSelector(object_getClass(controller), sel_getUid("hidServices")))
            {
                NSArray* hidServices = reinterpret_cast<NSArray* (*)(id, SEL)>(&objc_msgSend)(controller, sel_getUid("hidServices"));

                if (hidServices && [hidServices count] > 0)
                {
                    IOHIDServiceClientRef service = reinterpret_cast<IOHIDServiceClientRef (*)(id, SEL)>(&objc_msgSend)([hidServices firstObject], sel_getUid("service"));

                    CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service, CFSTR(kIOHIDVendorIDKey)));
                    if (vendor)
                    {
                        CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);
                        CFRelease(vendor);
                    }

                    CFNumberRef product = static_cast<CFNumberRef>(IOHIDServiceClientCopyProperty(service, CFSTR(kIOHIDProductIDKey)));
                    if (product)
                    {
                        CFNumberGetValue(product, kCFNumberSInt32Type, &productId);
                        CFRelease(product);
                    }
                }
            }

            bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

            // Use IOKit only if the controller does not support GameController framework
            if (supportsGameController)
            {
                std::unique_ptr<GamepadDeviceGC> gamepadDevice(new GamepadDeviceGC(*this, ++lastDeviceId, controller));

                Event event;
                event.type = Event::Type::DEVICE_CONNECT;
                event.deviceId = gamepadDevice->getId();
                event.deviceType = Controller::Type::GAMEPAD;

                gamepadDevicesGC.insert(std::make_pair(controller, gamepadDevice.get()));
                inputDevices.insert(std::make_pair(gamepadDevice->getId(), std::move(gamepadDevice)));

                addEvent(event);
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = gamepadDevicesGC.find(controller);

            if (i != gamepadDevicesGC.end())
            {
                GamepadDeviceGC* gamepadDeviceGC = i->second;

                Event event;
                event.type = Event::Type::DEVICE_DISCONNECT;
                event.deviceId = gamepadDeviceGC->getId();
                event.deviceType = Controller::Type::GAMEPAD;
                addEvent(event);

                gamepadDevicesGC.erase(i);

                auto gamepadDeviceIterator = std::find_if(inputDevices.begin(), inputDevices.end(), [gamepadDeviceGC](const std::pair<const uint32_t, std::unique_ptr<InputDevice>>& inputDevice) {
                    return inputDevice.second.get() == gamepadDeviceGC;
                });

                if (gamepadDeviceIterator != inputDevices.end())
                    inputDevices.erase(gamepadDeviceIterator);
            }
        }

        void InputSystemMacOS::handleGamepadConnected(IOHIDDeviceRef device)
        {
            int32_t vendorId = 0;
            int32_t productId = 0;

            CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
            if (vendor)
                CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

            CFNumberRef product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
            if (product)
                CFNumberGetValue(product, kCFNumberSInt32Type, &productId);

            bool supportsGameController = (vendorId == 0x1038 && productId == 0x1420) || // SteelSeries Nimbus
            (vendorId == 0x0F0D && productId == 0x0090); // Horipad Ultimate

            // Use IOKit only if the controller does not support GameController framework
            if (!supportsGameController)
            {
                std::unique_ptr<GamepadDeviceIOKit> gamepadDevice(new GamepadDeviceIOKit(*this, ++lastDeviceId, device));

                Event event;
                event.type = Event::Type::DEVICE_CONNECT;
                event.deviceId = gamepadDevice->getId();
                event.deviceType = Controller::Type::GAMEPAD;

                gamepadDevicesIOKit.insert(std::make_pair(device, gamepadDevice.get()));
                inputDevices.insert(std::make_pair(gamepadDevice->getId(), std::move(gamepadDevice)));

                addEvent(event);
            }
        }

        void InputSystemMacOS::handleGamepadDisconnected(IOHIDDeviceRef device)
        {
            auto i = gamepadDevicesIOKit.find(device);

            if (i != gamepadDevicesIOKit.end())
            {
                GamepadDeviceIOKit* gamepadDeviceIOKit = i->second;

                Event event;
                event.type = Event::Type::DEVICE_DISCONNECT;
                event.deviceId = gamepadDeviceIOKit->getId();
                event.deviceType = Controller::Type::GAMEPAD;

                addEvent(event);

                gamepadDevicesIOKit.erase(i);

                auto gamepadDeviceIterator = std::find_if(inputDevices.begin(), inputDevices.end(), [gamepadDeviceIOKit](const std::pair<const uint32_t, std::unique_ptr<InputDevice>>& inputDevice) {
                    return inputDevice.second.get() == gamepadDeviceIOKit;
                });

                if (gamepadDeviceIterator != inputDevices.end())
                    inputDevices.erase(gamepadDeviceIterator);
            }
        }
    } // namespace input
} // namespace ouzel
