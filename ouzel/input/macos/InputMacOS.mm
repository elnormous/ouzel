// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#import <CoreGraphics/CoreGraphics.h>
#import <GameController/GameController.h>
#import <Carbon/Carbon.h>
#include "InputMacOS.h"
#include "CursorResourceMacOS.h"
#include "GamepadMacOS.h"
#include "core/macos/WindowMacOS.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"

#if !defined(MAC_OS_X_VERSION_10_12) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_12
enum
{
    kVK_RightCommand = 0x36
};
#endif

static void deviceAdded(void* ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputMacOS*>(ctx);
    inputMacOS->handleGamepadConnected(device);
}

static void deviceRemoved(void *ctx, IOReturn, void*, IOHIDDeviceRef device)
{
    ouzel::input::InputMacOS* inputMacOS = reinterpret_cast<ouzel::input::InputMacOS*>(ctx);
    inputMacOS->handleGamepadDisconnected(device);
}

namespace ouzel
{
    namespace input
    {
        static const std::map<uint16_t, KeyboardKey> keyMap = {
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
            {kVK_Control, KeyboardKey::LCONTROL},
            {kVK_RightControl, KeyboardKey::RCONTROL},
            {kVK_Command, KeyboardKey::LSUPER},
            {kVK_RightCommand, KeyboardKey::RSUPER},
            {kVK_Shift, KeyboardKey::LSHIFT},
            {kVK_RightShift, KeyboardKey::RSHIFT},
            {kVK_Option, KeyboardKey::LALT},
            {kVK_RightOption, KeyboardKey::RALT},
            {kVK_Space, KeyboardKey::SPACE},

            {kVK_ANSI_A, KeyboardKey::KEY_A},
            {kVK_ANSI_B, KeyboardKey::KEY_B},
            {kVK_ANSI_C, KeyboardKey::KEY_C},
            {kVK_ANSI_D, KeyboardKey::KEY_D},
            {kVK_ANSI_E, KeyboardKey::KEY_E},
            {kVK_ANSI_F, KeyboardKey::KEY_F},
            {kVK_ANSI_G, KeyboardKey::KEY_G},
            {kVK_ANSI_H, KeyboardKey::KEY_H},
            {kVK_ANSI_I, KeyboardKey::KEY_I},
            {kVK_ANSI_J, KeyboardKey::KEY_J},
            {kVK_ANSI_K, KeyboardKey::KEY_K},
            {kVK_ANSI_L, KeyboardKey::KEY_L},
            {kVK_ANSI_M, KeyboardKey::KEY_M},
            {kVK_ANSI_N, KeyboardKey::KEY_N},
            {kVK_ANSI_O, KeyboardKey::KEY_O},
            {kVK_ANSI_P, KeyboardKey::KEY_P},
            {kVK_ANSI_Q, KeyboardKey::KEY_Q},
            {kVK_ANSI_R, KeyboardKey::KEY_R},
            {kVK_ANSI_S, KeyboardKey::KEY_S},
            {kVK_ANSI_T, KeyboardKey::KEY_T},
            {kVK_ANSI_U, KeyboardKey::KEY_U},
            {kVK_ANSI_V, KeyboardKey::KEY_V},
            {kVK_ANSI_W, KeyboardKey::KEY_W},
            {kVK_ANSI_X, KeyboardKey::KEY_X},
            {kVK_ANSI_Y, KeyboardKey::KEY_Y},
            {kVK_ANSI_Z, KeyboardKey::KEY_Z},

            {kVK_ANSI_0, KeyboardKey::KEY_0},
            {kVK_ANSI_1, KeyboardKey::KEY_1},
            {kVK_ANSI_2, KeyboardKey::KEY_2},
            {kVK_ANSI_3, KeyboardKey::KEY_3},
            {kVK_ANSI_4, KeyboardKey::KEY_4},
            {kVK_ANSI_5, KeyboardKey::KEY_5},
            {kVK_ANSI_6, KeyboardKey::KEY_6},
            {kVK_ANSI_7, KeyboardKey::KEY_7},
            {kVK_ANSI_8, KeyboardKey::KEY_8},
            {kVK_ANSI_9, KeyboardKey::KEY_9},

            {kVK_ANSI_Comma, KeyboardKey::COMMA},
            {kVK_ANSI_Period, KeyboardKey::PERIOD},
            {kVK_PageUp, KeyboardKey::PRIOR},
            {kVK_PageDown, KeyboardKey::NEXT},

            {kVK_ANSI_Keypad0, KeyboardKey::NUMPAD0},
            {kVK_ANSI_Keypad1, KeyboardKey::NUMPAD1},
            {kVK_ANSI_Keypad2, KeyboardKey::NUMPAD2},
            {kVK_ANSI_Keypad3, KeyboardKey::NUMPAD3},
            {kVK_ANSI_Keypad4, KeyboardKey::NUMPAD4},
            {kVK_ANSI_Keypad5, KeyboardKey::NUMPAD5},
            {kVK_ANSI_Keypad6, KeyboardKey::NUMPAD6},
            {kVK_ANSI_Keypad7, KeyboardKey::NUMPAD7},
            {kVK_ANSI_Keypad8, KeyboardKey::NUMPAD8},
            {kVK_ANSI_Keypad9, KeyboardKey::NUMPAD9},

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
            {kVK_ANSI_LeftBracket, KeyboardKey::BRACKET_LEFT},
            {kVK_ANSI_Backslash, KeyboardKey::BACKSLASH},
            {kVK_ANSI_RightBracket, KeyboardKey::BRACKET_RIGHT}
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

        KeyboardKey InputMacOS::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
            {
                return i->second;
            }
            else
            {
                return KeyboardKey::NONE;
            }
        }

        NSUInteger InputMacOS::getKeyMask(uint16_t keyCode)
        {
            auto i = maskMap.find(keyCode);

            if (i != maskMap.end())
            {
                return i->second;
            }
            else
            {
                return 0;
            }
        }

        uint32_t InputMacOS::getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons)
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

        InputMacOS::InputMacOS()
        {
            currentCursor = defaultCursor = [NSCursor arrowCursor];
        }

        InputMacOS::~InputMacOS()
        {
            resourceDeleteSet.clear();
            resources.clear();

            if (hidManager)
            {
                IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
                CFRelease(hidManager);
            }
        }

        bool InputMacOS::init()
        {
            NSArray* criteria = @[
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_Joystick)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_GamePad)},
                                  @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey : @(kHIDUsage_GD_MultiAxisController)}
                                  ];

            hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);

            IOHIDManagerSetDeviceMatchingMultiple(hidManager, (CFArrayRef)criteria);
            if (IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone) != kIOReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to initialize manager";
                return false;
            }
            else
            {
                IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAdded, this);
                IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemoved, this);
                IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            }

            unsigned char data[4] = {0, 0, 0, 0};
            unsigned char* rgba = data;

            NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(1, 1)];
            NSBitmapImageRep* rep = [[NSBitmapImageRep alloc]
                                     initWithBitmapDataPlanes:&rgba
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

            return true;
        }

        void InputMacOS::activateCursorResource(CursorResource* resource)
        {
            Input::activateCursorResource(resource);

            CursorResourceMacOS* cursorMacOS = static_cast<CursorResourceMacOS*>(resource);

            if (cursorMacOS)
            {
                currentCursor = cursorMacOS->getNativeCursor();
                if (!currentCursor) currentCursor = emptyCursor;
            }
            else
            {
                currentCursor = defaultCursor;
            }

            WindowMacOS* windowMacOS = static_cast<WindowMacOS*>(sharedEngine->getWindow());
            [windowMacOS->getNativeView() resetCursorRects];
        }

        CursorResource* InputMacOS::createCursorResource()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::unique_ptr<CursorResourceMacOS> cursorResource(new CursorResourceMacOS());
            CursorResource* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputMacOS::setCursorVisible(bool visible)
        {
            if (visible != cursorVisible)
            {
                cursorVisible = visible;

                sharedEngine->execute([this, visible] {
                    if (visible)
                    {
                        [currentCursor set];
                    }
                    else
                    {
                        [emptyCursor set];
                    }
                });
            }
        }

        bool InputMacOS::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputMacOS::setCursorPosition(const Vector2& position)
        {
            Input::setCursorPosition(position);

            ouzel::Vector2 windowLocation = ouzel::sharedEngine->getWindow()->convertNormalizedToWindowLocation(position);

            sharedEngine->execute([windowLocation] {
                CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

                WindowMacOS* windowMacOS = static_cast<WindowMacOS*>(sharedEngine->getWindow());
                CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

                CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.v[0],
                                                      screenOrigin.y + windowOrigin.y + windowLocation.v[1]));
            });
        }

        void InputMacOS::setCursorLocked(bool locked)
        {
            sharedEngine->execute([locked] {
                CGAssociateMouseAndMouseCursorPosition(!locked);
            });
            cursorLocked = locked;
        }

        bool InputMacOS::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputMacOS::handleGamepadConnected(IOHIDDeviceRef device)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_CONNECT;

            std::unique_ptr<GamepadMacOS> gamepad(new GamepadMacOS(device));

            event.gamepadEvent.gamepad = gamepad.get();

            gamepads.push_back(std::move(gamepad));

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void InputMacOS::handleGamepadDisconnected(IOHIDDeviceRef device)
        {
            std::vector<std::unique_ptr<Gamepad>>::iterator i = std::find_if(gamepads.begin(), gamepads.end(), [device](const std::unique_ptr<Gamepad>& gamepad) {
                GamepadMacOS* currentGamepad = static_cast<GamepadMacOS*>(gamepad.get());
                return currentGamepad->getDevice() == device;
            });

            if (i != gamepads.end())
            {
                Event event;
                event.type = Event::Type::GAMEPAD_DISCONNECT;

                event.gamepadEvent.gamepad = (*i).get();

                sharedEngine->getEventDispatcher()->postEvent(event);

                gamepads.erase(i);
            }
        }
    } // namespace input
} // namespace ouzel
