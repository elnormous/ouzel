// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEXI_HPP
#define OUZEL_INPUT_GAMEPADDEVICEXI_HPP

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <Xinput.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "input/Gamepad.hpp"
#include "input/windows/GamepadDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceXI final: public GamepadDeviceWin
        {
        public:
            GamepadDeviceXI(InputSystem& initInputSystem,
                            DeviceId initId,
                            DWORD aPlayerIndex);

            void update();

            int32_t getPlayerIndex() const;

            void setVibration(Gamepad::Motor motor, float speed);
            float getVibration(Gamepad::Motor motor);

        private:
            void checkThumbAxis(SHORT oldValue, SHORT newValue, Gamepad::Button negativeButton, Gamepad::Button positiveButton);
            void checkButton(const XINPUT_STATE& newState, WORD mask, Gamepad::Button button);

            XINPUT_STATE state = {};
            XINPUT_VIBRATION vibration = {};
            DWORD playerIndex;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEXI_HPP
