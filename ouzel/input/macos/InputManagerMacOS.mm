// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#import <Carbon/Carbon.h>
#import <GameController/GameController.h>
#import <objc/message.h>
#include "input/macos/InputManagerMacOS.hpp"
#include "input/macos/NativeCursorMacOS.hpp"
#include "core/macos/NativeWindowMacOS.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

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

        Keyboard::Key InputManagerMacOS::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
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
        }

        InputManagerMacOS::~InputManagerMacOS()
        {
            resourceDeleteSet.clear();
            resources.clear();
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
            /*if (visible != cursorVisible)
            {
                cursorVisible = visible;

                engine->executeOnMainThread([this, visible] {
                    if (visible)
                        [currentCursor set];
                    else
                        [emptyCursor set];
                });
            }*/
        }

        bool InputManagerMacOS::isCursorVisible() const
        {
            //return cursorVisible;
            return true;
        }
    } // namespace input
} // namespace ouzel
