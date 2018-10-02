// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputManagerIOS.hpp"
#include "core/ios/NativeWindowIOS.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace input
    {
        InputManagerIOS::InputManagerIOS()
        {
        }

        InputManagerIOS::~InputManagerIOS()
        {
        }

        void InputManagerIOS::showVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowIOS->getTextField();
                [textField becomeFirstResponder];
            });
        }

        void InputManagerIOS::hideVirtualKeyboard()
        {
            engine->executeOnMainThread([]() {
                NativeWindowIOS* windowIOS = static_cast<NativeWindowIOS*>(engine->getWindow()->getNativeWindow());
                UITextField* textField = windowIOS->getTextField();
                [textField resignFirstResponder];
            });
        }
    } // namespace input
} // namespace ouzel
