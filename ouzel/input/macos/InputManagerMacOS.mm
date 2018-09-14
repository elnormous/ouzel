// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#import <Carbon/Carbon.h>
#import <GameController/GameController.h>
#import <objc/message.h>
#include "InputManagerMacOS.hpp"
#include "NativeCursorMacOS.hpp"
#include "GamepadGC.hpp"
#include "GamepadIOKit.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

typedef struct CF_BRIDGED_TYPE(id) __IOHIDServiceClient * IOHIDServiceClientRef;
extern "C" CFTypeRef _Nullable IOHIDServiceClientCopyProperty(IOHIDServiceClientRef service, CFStringRef key);

@interface ConnectDelegate: NSObject
{
    ouzel::input::InputManagerMacOS* input;
}

-(void)handleControllerConnected:(NSNotification*)notification;
-(void)handleControllerDisconnected:(NSNotification*)notification;

@end

@implementation ConnectDelegate

-(id)initWithInput:(ouzel::input::InputManagerMacOS*)initInput
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

#if !defined(__MAC_10_12) || __MAC_OS_X_VERSION_MAX_ALLOWED < __MAC_10_12
enum
{
    kVK_RightCommand = 0x36
};
#endif

static void deviceAdded(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputManagerMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputManagerMacOS*>(ctx);
    inputMacOS->handleGamepadConnected(device);
}

static void deviceRemoved(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputManagerMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputManagerMacOS*>(ctx);
    inputMacOS->handleGamepadDisconnected(device);
}

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<uint16_t, KeyboardKey> keyMap = {
            {kVK_UpArrow, KeyboardKey::UP},
            {kVK_DownArrow, KeyboardKey::DOWN},
            {kVK_LeftArrow, KeyboardKey::LEFT},
            {kVK_RightArrow, KeyboardKey::RIGHT},
            {kVK_F1, KeyboardKey::F1},
            {kVK_F2, KeyboardKey::F2},
            {kVK_F3, KeyboardKey::F3},
            {kVK_F4, KeyboardKey::F4},
            {kVK_F5, KeyboardKey::F5},
            {kVK_F6, KeyboardKey::F6},
            {kVK_F7, KeyboardKey::F7},
            {kVK_F8, KeyboardKey::F8},
            {kVK_F9, KeyboardKey::F9},
            {kVK_F10, KeyboardKey::F10},
            {kVK_F11, KeyboardKey::F11},
            {kVK_F12, KeyboardKey::F12},
            {kVK_F13, KeyboardKey::F13},
            {kVK_F14, KeyboardKey::F14},
            {kVK_F15, KeyboardKey::F15},
            {kVK_F16, KeyboardKey::F16},
            {kVK_F17, KeyboardKey::F17},
            {kVK_F18, KeyboardKey::F18},
            {kVK_F19, KeyboardKey::F19},
            {kVK_F20, KeyboardKey::F20},
            {kVK_Home, KeyboardKey::HOME},
            {kVK_End, KeyboardKey::END},
            {NSInsertFunctionKey, KeyboardKey::INSERT},
            {kVK_ForwardDelete, KeyboardKey::DEL},
            {kVK_Help, KeyboardKey::HELP},
            {NSSelectFunctionKey, KeyboardKey::SELECT},
            {NSPrintFunctionKey, KeyboardKey::PRINT},
            {NSExecuteFunctionKey, KeyboardKey::EXECUTE},
            {NSPrintScreenFunctionKey, KeyboardKey::SNAPSHOT},
            {NSPauseFunctionKey, KeyboardKey::PAUSE},
            {NSScrollLockFunctionKey, KeyboardKey::SCROLL},
            {kVK_Delete, KeyboardKey::BACKSPACE},
            {kVK_Tab, KeyboardKey::TAB},
            {kVK_Return, KeyboardKey::RETURN},
            {kVK_Escape, KeyboardKey::ESCAPE},
            {kVK_Control, KeyboardKey::LEFT_CONTROL},
            {kVK_RightControl, KeyboardKey::RIGHT_CONTROL},
            {kVK_Command, KeyboardKey::LEFT_SUPER},
            {kVK_RightCommand, KeyboardKey::RIGHT_SUPER},
            {kVK_Shift, KeyboardKey::LEFT_SHIFT},
            {kVK_RightShift, KeyboardKey::RIGHT_SHIFT},
            {kVK_Option, KeyboardKey::LEFT_ALT},
            {kVK_RightOption, KeyboardKey::RIGHT_ALT},
            {kVK_Space, KeyboardKey::SPACE},

            {kVK_ANSI_A, KeyboardKey::A},
            {kVK_ANSI_B, KeyboardKey::B},
            {kVK_ANSI_C, KeyboardKey::C},
            {kVK_ANSI_D, KeyboardKey::D},
            {kVK_ANSI_E, KeyboardKey::E},
            {kVK_ANSI_F, KeyboardKey::F},
            {kVK_ANSI_G, KeyboardKey::G},
            {kVK_ANSI_H, KeyboardKey::H},
            {kVK_ANSI_I, KeyboardKey::I},
            {kVK_ANSI_J, KeyboardKey::J},
            {kVK_ANSI_K, KeyboardKey::K},
            {kVK_ANSI_L, KeyboardKey::L},
            {kVK_ANSI_M, KeyboardKey::M},
            {kVK_ANSI_N, KeyboardKey::N},
            {kVK_ANSI_O, KeyboardKey::O},
            {kVK_ANSI_P, KeyboardKey::P},
            {kVK_ANSI_Q, KeyboardKey::Q},
            {kVK_ANSI_R, KeyboardKey::R},
            {kVK_ANSI_S, KeyboardKey::S},
            {kVK_ANSI_T, KeyboardKey::T},
            {kVK_ANSI_U, KeyboardKey::U},
            {kVK_ANSI_V, KeyboardKey::V},
            {kVK_ANSI_W, KeyboardKey::W},
            {kVK_ANSI_X, KeyboardKey::X},
            {kVK_ANSI_Y, KeyboardKey::Y},
            {kVK_ANSI_Z, KeyboardKey::Z},

            {kVK_ANSI_0, KeyboardKey::NUM_0},
            {kVK_ANSI_1, KeyboardKey::NUM_1},
            {kVK_ANSI_2, KeyboardKey::NUM_2},
            {kVK_ANSI_3, KeyboardKey::NUM_3},
            {kVK_ANSI_4, KeyboardKey::NUM_4},
            {kVK_ANSI_5, KeyboardKey::NUM_5},
            {kVK_ANSI_6, KeyboardKey::NUM_6},
            {kVK_ANSI_7, KeyboardKey::NUM_7},
            {kVK_ANSI_8, KeyboardKey::NUM_8},
            {kVK_ANSI_9, KeyboardKey::NUM_9},

            {kVK_ANSI_Comma, KeyboardKey::COMMA},
            {kVK_ANSI_Period, KeyboardKey::PERIOD},
            {kVK_PageUp, KeyboardKey::PRIOR},
            {kVK_PageDown, KeyboardKey::NEXT},

            {kVK_ANSI_Keypad0, KeyboardKey::NUMPAD_0},
            {kVK_ANSI_Keypad1, KeyboardKey::NUMPAD_1},
            {kVK_ANSI_Keypad2, KeyboardKey::NUMPAD_2},
            {kVK_ANSI_Keypad3, KeyboardKey::NUMPAD_3},
            {kVK_ANSI_Keypad4, KeyboardKey::NUMPAD_4},
            {kVK_ANSI_Keypad5, KeyboardKey::NUMPAD_5},
            {kVK_ANSI_Keypad6, KeyboardKey::NUMPAD_6},
            {kVK_ANSI_Keypad7, KeyboardKey::NUMPAD_7},
            {kVK_ANSI_Keypad8, KeyboardKey::NUMPAD_8},
            {kVK_ANSI_Keypad9, KeyboardKey::NUMPAD_9},

            {kVK_ANSI_KeypadDecimal, KeyboardKey::DECIMAL},
            {kVK_ANSI_KeypadMultiply, KeyboardKey::MULTIPLY},
            {kVK_ANSI_KeypadPlus, KeyboardKey::PLUS},
            {kVK_ANSI_KeypadClear, KeyboardKey::OEM_CLEAR},
            {kVK_ANSI_KeypadDivide, KeyboardKey::DIVIDE},
            {kVK_ANSI_KeypadEnter, KeyboardKey::RETURN},
            {kVK_ANSI_KeypadMinus, KeyboardKey::SUBTRACT},

            {kVK_ANSI_Semicolon, KeyboardKey::SEMICOLON},
            {kVK_ANSI_Slash, KeyboardKey::SLASH},
            {kVK_ANSI_Grave, KeyboardKey::GRAVE},
            {kVK_ANSI_LeftBracket, KeyboardKey::LEFT_BRACKET},
            {kVK_ANSI_Backslash, KeyboardKey::BACKSLASH},
            {kVK_ANSI_RightBracket, KeyboardKey::RIGHT_BRACKET}
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

        KeyboardKey InputManagerMacOS::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        NSUInteger InputManagerMacOS::getKeyMask(uint16_t keyCode)
        {
            auto i = maskMap.find(keyCode);

            if (i != maskMap.end())
                return i->second;
            else
                return 0;
        }

        uint32_t InputManagerMacOS::getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons)
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

        InputManagerMacOS::InputManagerMacOS()
        {
            currentCursor = defaultCursor = [NSCursor arrowCursor];

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

            unsigned char* data = emptyCursorData;

            NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(1, 1)];
            NSBitmapImageRep* rep = [[NSBitmapImageRep alloc]
                                     initWithBitmapDataPlanes:&data
                                     pixelsWide:1
                                     pixelsHigh:1
                                     bitsPerSample:8
                                     samplesPerPixel:4
                                     hasAlpha:YES
                                     isPlanar:NO
                                     colorSpaceName:NSDeviceRGBColorSpace
                                     bitmapFormat:NSAlphaNonpremultipliedBitmapFormat
                                     bytesPerRow:4
                                     bitsPerPixel:32];

            [image addRepresentation:rep];
            emptyCursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(0, 0)];

            [image release];
            [rep release];

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        InputManagerMacOS::~InputManagerMacOS()
        {
            resourceDeleteSet.clear();
            resources.clear();

            if (connectDelegate)
                [connectDelegate release];

            if (hidManager)
            {
                IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
                CFRelease(hidManager);
            }
        }

        void InputManagerMacOS::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

            NativeCursorMacOS* cursorMacOS = static_cast<NativeCursorMacOS*>(resource);

            if (cursorMacOS)
            {
                currentCursor = cursorMacOS->getNativeCursor();
                if (!currentCursor) currentCursor = emptyCursor;
            }
            else
                currentCursor = defaultCursor;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            [windowMacOS->getNativeWindow() invalidateCursorRectsForView:windowMacOS->getNativeView()];
        }

        NativeCursor* InputManagerMacOS::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursorMacOS> cursorResource(new NativeCursorMacOS(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerMacOS::setCursorVisible(bool visible)
        {
            if (visible != cursorVisible)
            {
                cursorVisible = visible;

                engine->executeOnMainThread([this, visible] {
                    if (visible)
                        [currentCursor set];
                    else
                        [emptyCursor set];
                });
            }
        }

        bool InputManagerMacOS::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerMacOS::setCursorPosition(const Vector2& position)
        {
            InputManager::setCursorPosition(position);

            ouzel::Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            engine->executeOnMainThread([windowLocation] {
                CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

                NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
                CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

                CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.x,
                                                      screenOrigin.y + windowOrigin.y + windowLocation.y));
            });
        }

        void InputManagerMacOS::setCursorLocked(bool locked)
        {
            engine->executeOnMainThread([locked] {
                CGAssociateMouseAndMouseCursorPosition(!locked);
            });
            cursorLocked = locked;
        }

        bool InputManagerMacOS::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputManagerMacOS::startDeviceDiscovery()
        {
            Log(Log::Level::INFO) << "Started gamepad discovery";

            discovering = true;

            [GCController startWirelessControllerDiscoveryWithCompletionHandler:
             ^(void){ handleGamepadDiscoveryCompleted(); }];
        }

        void InputManagerMacOS::stopDeviceDiscovery()
        {
            if (discovering)
            {
                Log(Log::Level::INFO) << "Stopped gamepad discovery";

                [GCController stopWirelessControllerDiscovery];

                discovering = false;
            }
        }

        void InputManagerMacOS::handleGamepadDiscoveryCompleted()
        {
            Log(Log::Level::INFO) << "Gamepad discovery completed";
            discovering = false;
        }

        void InputManagerMacOS::handleGamepadConnected(GCControllerPtr controller)
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
                Event event;
                event.type = Event::Type::GAMEPAD_CONNECT;

                std::unique_ptr<GamepadGC> gamepad(new GamepadGC(controller));
                gamepadsGC.push_back(gamepad.get());

                event.gamepadEvent.gamepad = gamepad.get();

                gamepads.push_back(std::move(gamepad));

                engine->getEventDispatcher().postEvent(event);
            }
        }

        void InputManagerMacOS::handleGamepadDisconnected(GCControllerPtr controller)
        {
            auto i = std::find_if(gamepadsGC.begin(), gamepadsGC.end(), [controller](GamepadGC* gamepad) {
                return gamepad->getController() == controller;
            });

            if (i != gamepadsGC.end())
            {
                GamepadGC* gamepadGC = *i;

                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;

                event.gamepadEvent.gamepad = gamepadGC;

                engine->getEventDispatcher().postEvent(event);

                gamepadsGC.erase(i);

                auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadGC](const std::unique_ptr<Gamepad>& gamepad) {
                    return gamepad.get() == gamepadGC;
                });

                if (gamepadIterator != gamepads.end())
                    gamepads.erase(gamepadIterator);
            }
        }

        void InputManagerMacOS::handleGamepadConnected(IOHIDDeviceRef device)
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
                Event event;
                event.type = Event::Type::GAMEPAD_CONNECT;

                std::unique_ptr<GamepadIOKit> gamepad(new GamepadIOKit(device));
                gamepadsIOKit.push_back(gamepad.get());

                event.gamepadEvent.gamepad = gamepad.get();

                gamepads.push_back(std::move(gamepad));

                engine->getEventDispatcher().postEvent(event);
            }
        }

        void InputManagerMacOS::handleGamepadDisconnected(IOHIDDeviceRef device)
        {
            auto i = std::find_if(gamepadsIOKit.begin(), gamepadsIOKit.end(), [device](GamepadIOKit* gamepad) {
                return gamepad->getDevice() == device;
            });

            if (i != gamepadsIOKit.end())
            {
                GamepadIOKit* gamepadIOKit = *i;

                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;

                event.gamepadEvent.gamepad = gamepadIOKit;

                engine->getEventDispatcher().postEvent(event);

                gamepadsIOKit.erase(i);

                auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadIOKit](const std::unique_ptr<Gamepad>& gamepad) {
                    return gamepad.get() == gamepadIOKit;
                });

                if (gamepadIterator != gamepads.end())
                    gamepads.erase(gamepadIterator);
            }
        }
    } // namespace input
} // namespace ouzel
