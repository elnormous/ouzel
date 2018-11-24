// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMWIN_HPP
#define OUZEL_INPUT_INPUTSYSTEMWIN_HPP

#include <memory>
#include <system_error>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include "input/InputSystem.hpp"
#include "input/windows/GamepadDeviceWin.hpp"
#include "input/windows/KeyboardDeviceWin.hpp"
#include "input/windows/MouseDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class DirectInputErrorCategory: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "DirectInput";
            }

            std::string message(int condition) const override
            {
                switch (condition)
                {
                    case DIERR_ACQUIRED: return "DIERR_ACQUIRED";
                    case DIERR_ALREADYINITIALIZED: return "DIERR_ALREADYINITIALIZED";
                    case DIERR_BADDRIVERVER: return "DIERR_BADDRIVERVER";
                    case DIERR_BETADIRECTINPUTVERSION: return "DIERR_BETADIRECTINPUTVERSION";
                    case DIERR_DEVICEFULL: return "DIERR_DEVICEFULL";
                    case DIERR_DEVICENOTREG: return "DIERR_DEVICENOTREG";
                    case DIERR_EFFECTPLAYING: return "DIERR_EFFECTPLAYING";
                    case DIERR_GENERIC: return "DIERR_GENERIC";
                    case DIERR_HANDLEEXISTS: return "DIERR_HANDLEEXISTS";
                    case DIERR_HASEFFECTS: return "DIERR_HASEFFECTS";
                    case DIERR_INCOMPLETEEFFECT: return "DIERR_INCOMPLETEEFFECT";
                    case DIERR_INPUTLOST: return "DIERR_INPUTLOST";
                    case DIERR_INVALIDPARAM: return "DIERR_INVALIDPARAM";
                    case DIERR_MAPFILEFAIL: return "DIERR_MAPFILEFAIL";
                    case DIERR_MOREDATA: return "DIERR_MOREDATA";
                    case DIERR_NOAGGREGATION: return "DIERR_NOAGGREGATION";
                    case DIERR_NOINTERFACE: return "DIERR_NOINTERFACE";
                    case DIERR_NOTACQUIRED: return "DIERR_NOTACQUIRED";
                    case DIERR_NOTBUFFERED: return "DIERR_NOTBUFFERED";
                    case DIERR_NOTDOWNLOADED: return "DIERR_NOTDOWNLOADED";
                    case DIERR_NOTEXCLUSIVEACQUIRED: return "DIERR_NOTEXCLUSIVEACQUIRED";
                    case DIERR_NOTFOUND: return "DIERR_NOTFOUND";
                    case DIERR_NOTINITIALIZED: return "DIERR_NOTINITIALIZED";
                    case DIERR_OLDDIRECTINPUTVERSION: return "DIERR_OLDDIRECTINPUTVERSION";
                    case DIERR_OUTOFMEMORY: return "DIERR_OUTOFMEMORY";
                    case DIERR_REPORTFULL: return "DIERR_REPORTFULL";
                    case DIERR_UNPLUGGED: return "DIERR_UNPLUGGED";
                    case DIERR_UNSUPPORTED: return "DIERR_UNSUPPORTED";
                    case E_HANDLE: return "E_HANDLE";
                    case E_PENDING: return "E_PENDING";
                    case E_POINTER: return "E_POINTER";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        extern const DirectInputErrorCategory directInputErrorCategory;

        class GamepadDeviceDI;
        class GamepadDeviceXI;
        class NativeCursorWin;

        class InputSystemWin final: public InputSystem
        {
        public:
            explicit InputSystemWin(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemWin();

            void executeCommand(const Command& command) override;

            KeyboardDeviceWin* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceWin* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            void update();

            IDirectInput8W* getDirectInput() const { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

            void updateCursor() const;

        private:
            bool discovering = false;

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDeviceWin> keyboardDevice;
            std::unique_ptr<MouseDeviceWin> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            IDirectInput8W* directInput = nullptr;
            std::vector<std::unique_ptr<GamepadDeviceDI>> gamepadsDI;
            std::unique_ptr<GamepadDeviceXI> gamepadsXI[XUSER_MAX_COUNT];

            std::vector<std::unique_ptr<NativeCursorWin>> cursors;

            HCURSOR defaultCursor = nullptr;
        };
    }
}

#endif // OUZEL_INPUT_INPUTSYSTEMWIN_HPP
