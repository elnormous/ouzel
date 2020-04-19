// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEDI_HPP
#define OUZEL_INPUT_GAMEPADDEVICEDI_HPP

#include <string>
#include <unordered_map>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <hidusage.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "../Gamepad.hpp"
#include "GamepadDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceDI final: public GamepadDeviceWin
        {
        public:
            GamepadDeviceDI(InputSystem& initInputSystem,
                            DeviceId initId,
                            const DIDEVICEINSTANCEW* aInstance, IDirectInput8W* directInput, HWND window);

            ~GamepadDeviceDI() override;
            void update();

            inline auto getInstance() const noexcept { return instance; }

        private:
            void checkInputBuffered();
            void checkInputPolled();
            void handleAxisChange(LONG oldValue, LONG newValue,
                                  LONG min, LONG range,
                                  Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            std::string name;

            const DIDEVICEINSTANCEW* instance = nullptr;
            IDirectInputDevice8W* device = nullptr;
            DWORD hatValue = 0xFFFFFFFF;

            struct Button final
            {
                Gamepad::Button button = Gamepad::Button::Unknown;
                BYTE value = 0;
            };

            std::unordered_map<DWORD, Button> buttons;

            struct Axis final
            {
                Gamepad::Axis axis = Gamepad::Axis::Unknown;
                LONG min = 0;
                LONG max = 0;
                LONG range = 0;
                LONG value = 0;
                Gamepad::Button negativeButton = Gamepad::Button::Unknown;
                Gamepad::Button positiveButton = Gamepad::Button::Unknown;
            };

            std::unordered_map<DWORD, Axis> axes;

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
            bool buffered = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEDI_HPP
