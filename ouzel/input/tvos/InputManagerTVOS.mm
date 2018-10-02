// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputManagerTVOS.hpp"
#include "GamepadDeviceTVOS.hpp"
#include "core/tvos/NativeWindowTVOS.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerTVOS::InputManagerTVOS()
        {
        }

        InputManagerTVOS::~InputManagerTVOS()
        {
        }

        void InputManagerTVOS::showVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowTVOS->getTextField();
                [textField becomeFirstResponder];
            });
        }

        void InputManagerTVOS::hideVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowTVOS* windowTVOS = static_cast<NativeWindowTVOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowTVOS->getTextField();
                [textField resignFirstResponder];
            });
        }
    } // namespace input
} // namespace ouzel
