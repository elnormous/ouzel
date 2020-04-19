// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP
#define OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP

#include "../KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceWin final: public KeyboardDevice
        {
        public:
            KeyboardDeviceWin(InputSystem& initInputSystem,
                              DeviceId initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            std::future<bool> handleKeyPress(Keyboard::Key key);
            void update();

        private:
            bool leftShiftDown = false;
            bool rightShiftDown = false;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP
