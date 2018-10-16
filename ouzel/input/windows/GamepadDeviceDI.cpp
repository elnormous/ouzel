// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceDI.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "input/GamepadConfig.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const float THUMB_DEADZONE = 0.2F;
static const size_t INPUT_QUEUE_SIZE = 32;

BOOL CALLBACK enumObjectsCallback(const DIDEVICEOBJECTINSTANCEW* didObjectInstance, VOID* context)
{
    ouzel::input::GamepadDeviceDI* gamepadDI = static_cast<ouzel::input::GamepadDeviceDI*>(context);
    gamepadDI->handleObject(didObjectInstance);

    return DIENUM_CONTINUE;
}

namespace ouzel
{
    namespace input
    {
        GamepadDeviceDI::GamepadDeviceDI(InputSystem& initInputSystem,
                                         uint32_t initId,
                                         const DIDEVICEINSTANCEW* initInstance, IDirectInput8W* directInput, HWND window):
            GamepadDeviceWin(initInputSystem, initId),
            instance(initInstance)
        {
            ZeroMemory(&diState, sizeof(diState));

            int32_t vendorId = LOWORD(instance->guidProduct.Data1);
            int32_t productId = HIWORD(instance->guidProduct.Data1);

            int bytesNeeded = WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, nullptr, 0, nullptr, nullptr);
            name.resize(bytesNeeded);
            WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, &name.front(), bytesNeeded, nullptr, nullptr);

            HRESULT hr = directInput->CreateDevice(instance->guidInstance, &device, nullptr);
            if (FAILED(hr))
                throw SystemError("Failed to create DirectInput device, error: " + std::to_string(hr));
            
            GamepadConfig gamepadConfig;

            if (vendorId == 0x054C && productId == 0x0268) // Playstation 3 controller
                gamepadConfig = PLAYSTATION_3_CONFIG;
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
                gamepadConfig = PLAYSTATION_4_CONFIG;
            else if (vendorId == 0x045E && productId == 0x02D1) // Xbox One controller
                gamepadConfig = XBOX_ONE_CONFIG;
            else if ((vendorId == 0x0E6F && productId == 0x0113) || // AfterglowGamepadforXbox360
                (vendorId == 0x0E6F && productId == 0x0213) || // AfterglowGamepadforXbox360
                (vendorId == 0x1BAD && productId == 0xF900) || // AfterglowGamepadforXbox360
                (vendorId == 0x0738 && productId == 0xCB29) || // AviatorforXbox360PC
                (vendorId == 0x15E4 && productId == 0x3F10) || // BatarangwiredcontrollerXBOX
                (vendorId == 0x146B && productId == 0x0601) || // BigbenControllerBB7201
                (vendorId == 0x0738 && productId == 0xF401) || // Controller
                (vendorId == 0x0E6F && productId == 0xF501) || // Controller
                (vendorId == 0x1430 && productId == 0xF801) || // Controller
                (vendorId == 0x1BAD && productId == 0x028E) || // Controller
                (vendorId == 0x1BAD && productId == 0xFA01) || // Controller
                (vendorId == 0x12AB && productId == 0x0004) || // DDRUniverse2Mat
                (vendorId == 0x24C6 && productId == 0x5B00) || // Ferrari458Racingwheel
                (vendorId == 0x1430 && productId == 0x4734) || // GH4Guitar
                (vendorId == 0x046D && productId == 0xC21D) || // GamepadF310
                (vendorId == 0x0E6F && productId == 0x0301) || // GamepadforXbox360
                (vendorId == 0x0E6F && productId == 0x0401) || // GamepadforXbox360Z (GameStop XBox 360 Controller)
                (vendorId == 0x12AB && productId == 0x0302) || // GamepadforXbox360ZZ
                (vendorId == 0x1BAD && productId == 0xF902) || // GamepadforXbox360ZZZ
                (vendorId == 0x1BAD && productId == 0xF901) || // GamestopXbox360Controller
                (vendorId == 0x1430 && productId == 0x474C) || // GuitarHeroforPCMAC
                (vendorId == 0x1BAD && productId == 0xF501) || // HORIPADEX2TURBO
                (vendorId == 0x1BAD && productId == 0x0003) || // HarmonixDrumKitforXbox360
                (vendorId == 0x1BAD && productId == 0x0002) || // HarmonixGuitarforXbox360
                (vendorId == 0x0F0D && productId == 0x000A) || // HoriCoDOA4FightStick
                (vendorId == 0x0F0D && productId == 0x000D) || // HoriFightingStickEx2
                (vendorId == 0x0F0D && productId == 0x0016) || // HoriRealArcadeProEx
                (vendorId == 0x24C6 && productId == 0x5501) || // HoriRealArcadeProVXSA
                (vendorId == 0x24C6 && productId == 0x5506) || // HoriSOULCALIBURVStick
                (vendorId == 0x1BAD && productId == 0xF02D) || // JoytechNeoSe
                (vendorId == 0x162E && productId == 0xBEEF) || // JoytechNeoSeTake2
                (vendorId == 0x046D && productId == 0xC242) || // LogitechChillStream
                (vendorId == 0x046D && productId == 0xC21E) || // LogitechF510
                (vendorId == 0x1BAD && productId == 0xFD01) || // MadCatz360
                (vendorId == 0x0738 && productId == 0x4740) || // MadCatzBeatPad
                (vendorId == 0x1BAD && productId == 0xF025) || // MadCatzCallofDutyGamePad
                (vendorId == 0x1BAD && productId == 0xF027) || // MadCatzFPSProGamePad
                (vendorId == 0x1BAD && productId == 0xF021) || // MadCatzGhostReconFSGamePad
                (vendorId == 0x0738 && productId == 0x4736) || // MadCatzMicroConGamePadPro
                (vendorId == 0x1BAD && productId == 0xF036) || // MadCatzMicroConGamePadProZ
                (vendorId == 0x0738 && productId == 0x9871) || // MadCatzPortableDrumKit
                (vendorId == 0x0738 && productId == 0x4728) || // MadCatzStreetFighterIVFightPad
                (vendorId == 0x0738 && productId == 0x4718) || // MadCatzStreetFighterIVFightStickSE
                (vendorId == 0x0738 && productId == 0x4716) || // MadCatzXbox360Controller
                (vendorId == 0x0738 && productId == 0x4726) || // MadCatzXbox360Controller
                (vendorId == 0x0738 && productId == 0xBEEF) || // MadCatzXbox360Controller
                (vendorId == 0x1BAD && productId == 0xF016) || // MadCatzXbox360Controller
                (vendorId == 0x0738 && productId == 0xB726) || // MadCatzXboxcontrollerMW2
                (vendorId == 0x045E && productId == 0x028E) || // MicrosoftXbox360Controller
                (vendorId == 0x045E && productId == 0x0719) || // MicrosoftXbox360Controller
                (vendorId == 0x12AB && productId == 0x0301) || // PDPAFTERGLOWAX1
                (vendorId == 0x0E6F && productId == 0x0105) || // PDPDancePad
                (vendorId == 0x0E6F && productId == 0x0201) || // PelicanTSZ360Pad
                (vendorId == 0x15E4 && productId == 0x3F00) || // PowerAMiniProElite
                (vendorId == 0x24C6 && productId == 0x5300) || // PowerAMiniProEliteGlow
                (vendorId == 0x1BAD && productId == 0xF504) || // REALARCADEPROEX
                (vendorId == 0x1BAD && productId == 0xF502) || // REALARCADEProVX
                (vendorId == 0x1689 && productId == 0xFD00) || // RazerOnza
                (vendorId == 0x1689 && productId == 0xFD01) || // RazerOnzaTournamentEdition
                (vendorId == 0x1430 && productId == 0x4748) || // RedOctaneGuitarHeroXplorer
                (vendorId == 0x0E6F && productId == 0x011F) || // RockCandyGamepadforXbox360
                (vendorId == 0x12AB && productId == 0x0006) || // RockRevolutionforXbox360
                (vendorId == 0x0738 && productId == 0xCB02) || // SaitekCyborgRumblePadPCXbox360
                (vendorId == 0x0738 && productId == 0xCB03) || // SaitekP3200RumblePadPCXbox360
                (vendorId == 0x1BAD && productId == 0xF028) || // StreetFighterIVFightPad
                (vendorId == 0x0738 && productId == 0x4738) || // StreetFighterIVFightStickTE
                (vendorId == 0x0738 && productId == 0xF738) || // SuperSFIVFightStickTES
                (vendorId == 0x1BAD && productId == 0xF903) || // TronXbox360controller
                (vendorId == 0x1BAD && productId == 0x5500) || // USBGamepad
                (vendorId == 0x1BAD && productId == 0xF906) || // XB360MortalKombatFightStick
                (vendorId == 0x15E4 && productId == 0x3F0A)) // XboxAirflowiredcontroller
                gamepadConfig = XBOX_360_CONFIG;
            else if (vendorId == 0x0079 && productId == 0x0006) // Acme GA07
                gamepadConfig = ACME_GA07_CONFIG;
            else // Generic (based on Logitech RumblePad 2)
                gamepadConfig = GENERIC_CONFIG;

            for (size_t i = 0; i < 24; ++i)
            {
                if (gamepadConfig.buttonMap[i] != Gamepad::Button::NONE)
                {
                    buttons[i].offset = offsetof(DIJOYSTATE2, rgbButtons) + i;
                    buttons[i].button = gamepadConfig.buttonMap[i];
                }
            }

            std::array<std::pair<USAGE, size_t>, 6> axisUsageMap = {
                std::make_pair(HID_USAGE_GENERIC_X, DIJOFS_X),
                std::make_pair(HID_USAGE_GENERIC_Y, DIJOFS_Y),
                std::make_pair(HID_USAGE_GENERIC_Z, DIJOFS_Z),
                std::make_pair(HID_USAGE_GENERIC_RX, DIJOFS_RX),
                std::make_pair(HID_USAGE_GENERIC_RY, DIJOFS_RY),
                std::make_pair(HID_USAGE_GENERIC_RZ, DIJOFS_RZ)
            };

            for (size_t i = 0; i < 6; ++i)
            {
                if (gamepadConfig.axisMap[i] != Gamepad::Axis::NONE)
                {
                    axis[i].axis = gamepadConfig.axisMap[i];
                    axis[i].usage = axisUsageMap[i].first;
                    axis[i].offset = axisUsageMap[i].second;

                    switch (gamepadConfig.axisMap[i])
                    {
                        case Gamepad::Axis::LEFT_THUMB_X:
                            axis[i].negativeButton = Gamepad::Button::LEFT_THUMB_LEFT;
                            axis[i].positiveButton = Gamepad::Button::LEFT_THUMB_RIGHT;
                            break;
                        case Gamepad::Axis::LEFT_THUMB_Y:
                            axis[i].negativeButton = Gamepad::Button::LEFT_THUMB_UP;
                            axis[i].positiveButton = Gamepad::Button::LEFT_THUMB_DOWN;
                            break;
                        case Gamepad::Axis::RIGHT_THUMB_X:
                            axis[i].negativeButton = Gamepad::Button::RIGHT_THUMB_LEFT;
                            axis[i].positiveButton = Gamepad::Button::RIGHT_THUMB_RIGHT;
                            break;
                        case Gamepad::Axis::RIGHT_THUMB_Y:
                            axis[i].negativeButton = Gamepad::Button::RIGHT_THUMB_UP;
                            axis[i].positiveButton = Gamepad::Button::RIGHT_THUMB_DOWN;
                            break;
                        case Gamepad::Axis::LEFT_TRIGGER:
                            axis[i].negativeButton = Gamepad::Button::LEFT_TRIGGER;
                            axis[i].positiveButton = Gamepad::Button::LEFT_TRIGGER;
                            break;
                        case Gamepad::Axis::RIGHT_TRIGGER:
                            axis[i].negativeButton = Gamepad::Button::RIGHT_TRIGGER;
                            axis[i].positiveButton = Gamepad::Button::RIGHT_TRIGGER;
                            break;
                    }
                }
            }

            // Exclusive access is needed for force feedback
            hr = device->SetCooperativeLevel(window,
                                             DISCL_BACKGROUND | DISCL_EXCLUSIVE);
            if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device format, error: " + std::to_string(hr));

            hr = device->SetDataFormat(&c_dfDIJoystick2);
            if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device format, error: " + std::to_string(hr));

            DIDEVCAPS capabilities;
            capabilities.dwSize = sizeof(capabilities);
            hr = device->GetCapabilities(&capabilities);
            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device capabilities, error: " + std::to_string(hr));

            if (capabilities.dwFlags & DIDC_FORCEFEEDBACK)
            {
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->SendForceFeedbackCommand(DISFFC_RESET);
                if (FAILED(hr))
                    throw SystemError("Failed to set DirectInput device force feedback command, error: " + std::to_string(hr));

                hr = device->Unacquire();
                if (FAILED(hr))
                    throw SystemError("Failed to unacquire DirectInput device, error: " + std::to_string(hr));

                DIPROPDWORD propertyAutoCenter;
                propertyAutoCenter.diph.dwSize = sizeof(propertyAutoCenter);
                propertyAutoCenter.diph.dwHeaderSize = sizeof(propertyAutoCenter.diph);
                propertyAutoCenter.diph.dwHow = DIPH_DEVICE;
                propertyAutoCenter.diph.dwObj = 0;
                propertyAutoCenter.dwData = DIPROPAUTOCENTER_ON;

                hr = device->SetProperty(DIPROP_AUTOCENTER, &propertyAutoCenter.diph);
                if (FAILED(hr))
                    Log(Log::Level::WARN) << "Failed to set DirectInput device autocenter property, error: " << hr;
            }

            hr = device->EnumObjects(enumObjectsCallback, this, DIDFT_ALL);
            if (FAILED(hr))
                throw SystemError("Failed to enumerate DirectInput device objects, error: " + std::to_string(hr));

            DIPROPDWORD propertyBufferSize;
            propertyBufferSize.diph.dwSize = sizeof(propertyBufferSize);
            propertyBufferSize.diph.dwHeaderSize = sizeof(propertyBufferSize.diph);
            propertyBufferSize.diph.dwHow = DIPH_DEVICE;
            propertyBufferSize.diph.dwObj = 0;
            propertyBufferSize.dwData = INPUT_QUEUE_SIZE;

            hr = device->SetProperty(DIPROP_BUFFERSIZE, &propertyBufferSize.diph);

            if (hr == DI_POLLEDDEVICE)
                buffered = false;
            else if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device buffer size property, error: " + std::to_string(hr));
            else
                buffered = true;
        }

        GamepadDeviceDI::~GamepadDeviceDI()
        {
            if (device)
            {
                device->Unacquire();
                device->Release();
            }
        }

        void GamepadDeviceDI::update()
        {
            HRESULT result = device->Poll();

            if (result == DIERR_NOTACQUIRED)
            {
                HRESULT hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                result = device->Poll();
            }

            return buffered ? checkInputBuffered() : checkInputPolled();
        }

        static LONG getAxisValue(const DIJOYSTATE2& state, size_t offset)
        {
            return *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&state) + offset);
        }

        static void setAxisValue(DIJOYSTATE2& state, size_t offset, LONG value)
        {
            *reinterpret_cast<LONG*>(reinterpret_cast<uint8_t*>(&state) + offset) = value;
        }

        void GamepadDeviceDI::checkInputBuffered()
        {
            DWORD eventCount = INPUT_QUEUE_SIZE;
            DIDEVICEOBJECTDATA events[INPUT_QUEUE_SIZE];

            HRESULT hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);

            if (hr == DIERR_NOTACQUIRED)
            {
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);
            }

            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device state, error: " + std::to_string(hr));

            for (DWORD e = 0; e < eventCount; ++e)
            {
                for (uint32_t i = 0; i < 24; ++i)
                {
                    if (buttons[i].button != Gamepad::Button::NONE &&
                        buttons[i].offset == events[e].dwOfs)
                    {
                        if ((buttons[i].button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                            (buttons[i].button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                        {
                            handleButtonValueChange(buttons[i].button,
                                                    events[e].dwData > 0,
                                                    (events[e].dwData > 0) ? 1.0F : 0.0F);
                        }

                        diState.rgbButtons[i] = static_cast<BYTE>(events[e].dwData);
                    }
                }

                if (events[e].dwOfs == offsetof(DIJOYSTATE2, rgdwPOV[0]))
                {
                    uint32_t oldHatValue = static_cast<uint32_t>(diState.rgdwPOV[0]);
                    if (oldHatValue == 0xffffffff)
                        oldHatValue = 8;
                    else
                    {
                        // round up
                        oldHatValue += 4500 / 2;
                        oldHatValue %= 36000;
                        oldHatValue /= 4500;
                    }

                    uint32_t hatValue = static_cast<uint32_t>(events[e].dwData);
                    if (hatValue == 0xffffffff)
                        hatValue = 8;
                    else
                    {
                        // round up
                        hatValue += 4500 / 2;
                        hatValue %= 36000;
                        hatValue /= 4500;
                    }

                    uint32_t bitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                        (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                    uint32_t newBitmask = (hatValue >= 8) ? 0 : (1 << (hatValue / 2)) | // first bit
                        (1 << (hatValue / 2 + hatValue % 2)) % 4; // second bit

                    if ((bitmask & 0x01) != (newBitmask & 0x01)) 
                        handleButtonValueChange(Gamepad::Button::DPAD_UP,
                                                (newBitmask & 0x01) > 0,
                                                (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                    if ((bitmask & 0x02) != (newBitmask & 0x02))
                        handleButtonValueChange(Gamepad::Button::DPAD_RIGHT,
                                                (newBitmask & 0x02) > 0,
                                                (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                    if ((bitmask & 0x04) != (newBitmask & 0x04))
                        handleButtonValueChange(Gamepad::Button::DPAD_DOWN,
                                                (newBitmask & 0x04) > 0,
                                                (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                    if ((bitmask & 0x08) != (newBitmask & 0x08))
                        handleButtonValueChange(Gamepad::Button::DPAD_LEFT,
                                                (newBitmask & 0x08) > 0,
                                                (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                    diState.rgdwPOV[0] = events[e].dwData;
                }

                for (uint32_t i = 0; i < 6; ++i)
                {
                    if (axis[i].axis != Gamepad::Axis::NONE &&
                        axis[i].offset == events[e].dwOfs)
                    {
                        if (axis[i].negativeButton != axis[i].positiveButton)
                        {
                            checkThumbAxisChange(getAxisValue(diState, axis[i].offset),
                                                 events[e].dwData,
                                                 axis[i].min, axis[i].range,
                                                 axis[i].negativeButton, axis[i].positiveButton);

                            setAxisValue(diState, axis[i].offset, events[e].dwData);
                        }
                        else
                        {
                            checkTriggerChange(getAxisValue(diState, axis[i].offset),
                                               events[e].dwData,
                                               axis[i].min, axis[i].range,
                                               axis[i].negativeButton);

                            setAxisValue(diState, axis[i].offset, events[e].dwData);
                        }
                    }
                }
            }
        }

        void GamepadDeviceDI::checkInputPolled()
        {
            DIJOYSTATE2 newDIState;

            HRESULT hr = device->GetDeviceState(sizeof(newDIState), &newDIState);

            if (hr == DIERR_NOTACQUIRED)
            {
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->GetDeviceState(sizeof(newDIState), &newDIState);
            }

            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device state, error: " + std::to_string(hr));

            for (uint32_t i = 0; i < 24; ++i)
            {
                if (buttons[i].button != Gamepad::Button::NONE &&
                    newDIState.rgbButtons[i] != diState.rgbButtons[i])
                {
                    if ((buttons[i].button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                        (buttons[i].button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                    {
                        handleButtonValueChange(buttons[i].button,
                                                newDIState.rgbButtons[i] > 0,
                                                (newDIState.rgbButtons[i] > 0) ? 1.0F : 0.0F);
                    }
                }
            }

            if (newDIState.rgdwPOV[0] != diState.rgdwPOV[0])
            {
                uint32_t oldHatValue = static_cast<uint32_t>(diState.rgdwPOV[0]);
                if (oldHatValue == 0xffffffff)
                    oldHatValue = 8;
                else
                {
                    // round up
                    oldHatValue += 4500 / 2;
                    oldHatValue %= 36000;
                    oldHatValue /= 4500;
                }

                uint32_t hatValue = static_cast<uint32_t>(newDIState.rgdwPOV[0]);
                if (hatValue == 0xffffffff)
                    hatValue = 8;
                else
                {
                    // round up
                    hatValue += 4500 / 2;
                    hatValue %= 36000;
                    hatValue /= 4500;
                }

                uint32_t bitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                    (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                uint32_t newBitmask = (hatValue >= 8) ? 0 : (1 << (hatValue / 2)) | // first bit
                    (1 << (hatValue / 2 + hatValue % 2)) % 4; // second bit

                if ((bitmask & 0x01) != (newBitmask & 0x01))
                    handleButtonValueChange(Gamepad::Button::DPAD_UP,
                                            (newBitmask & 0x01) > 0,
                                            (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                if ((bitmask & 0x02) != (newBitmask & 0x02))
                    handleButtonValueChange(Gamepad::Button::DPAD_RIGHT,
                                            (newBitmask & 0x02) > 0,
                                            (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                if ((bitmask & 0x04) != (newBitmask & 0x04))
                    handleButtonValueChange(Gamepad::Button::DPAD_DOWN,
                                            (newBitmask & 0x04) > 0,
                                            (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                if ((bitmask & 0x08) != (newBitmask & 0x08))
                    handleButtonValueChange(Gamepad::Button::DPAD_LEFT,
                                            (newBitmask & 0x08) > 0,
                                            (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);
            }

            for (uint32_t i = 0; i < 6; ++i)
            {
                if (axis[i].axis != Gamepad::Axis::NONE)
                {
                    if (axis[i].negativeButton != axis[i].positiveButton)
                    {
                        checkThumbAxisChange(getAxisValue(diState, axis[i].offset),
                                             getAxisValue(newDIState, axis[i].offset),
                                             axis[i].min, axis[i].range,
                                             axis[i].negativeButton, axis[i].positiveButton);
                    }
                    else
                    {
                        checkTriggerChange(getAxisValue(diState, axis[i].offset),
                                           getAxisValue(newDIState, axis[i].offset),
                                           axis[i].min, axis[i].range,
                                           axis[i].negativeButton);
                    }
                }
            }

            diState = newDIState;
        }

        void GamepadDeviceDI::checkThumbAxisChange(LONG oldValue, LONG newValue,
                                                   int64_t min, int64_t range,
                                                   Gamepad::Button negativeButton, Gamepad::Button positiveButton)
        {
            if (oldValue != newValue)
            {
                float floatValue = 2.0F * (newValue - min) / range - 1.0F;

                if (floatValue > 0.0F)
                {
                    handleButtonValueChange(positiveButton,
                                            floatValue > THUMB_DEADZONE,
                                            floatValue);
                }
                else if (floatValue < 0.0F)
                {
                    handleButtonValueChange(negativeButton,
                                            -floatValue > THUMB_DEADZONE,
                                            -floatValue);
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                        handleButtonValueChange(positiveButton, false, 0.0F);
                    else
                        handleButtonValueChange(negativeButton, false, 0.0F);
                }
            }
        }

        void GamepadDeviceDI::checkTriggerChange(LONG oldValue, LONG newValue,
                                                 int64_t min, int64_t range,
                                                 Gamepad::Button button)
        {
            if (oldValue != newValue)
            {
                float floatValue = 2.0F * (newValue - min) / range - 1.0F;

                handleButtonValueChange(button,
                                        floatValue > 0.0F,
                                        floatValue);
            }
        }

        void GamepadDeviceDI::handleObject(const DIDEVICEOBJECTINSTANCEW* didObjectInstance)
        {
            if (didObjectInstance->dwType & DIDFT_AXIS)
            {
                DIPROPDWORD propertyDeadZone;
                propertyDeadZone.diph.dwSize = sizeof(propertyDeadZone);
                propertyDeadZone.diph.dwHeaderSize = sizeof(propertyDeadZone.diph);
                propertyDeadZone.diph.dwObj = didObjectInstance->dwType;
                propertyDeadZone.diph.dwHow = DIPH_BYID;
                propertyDeadZone.dwData = 0;

                // Set the range for the axis
                HRESULT hr = device->SetProperty(DIPROP_DEADZONE, &propertyDeadZone.diph);
                if (FAILED(hr))
                    Log(Log::Level::WARN) << "Failed to set DirectInput device dead zone property, error: " << hr;

                DIPROPRANGE propertyAxisRange;
                propertyAxisRange.diph.dwSize = sizeof(propertyAxisRange);
                propertyAxisRange.diph.dwHeaderSize = sizeof(propertyAxisRange.diph);
                propertyAxisRange.diph.dwObj = didObjectInstance->dwType;
                propertyAxisRange.diph.dwHow = DIPH_BYID;

                hr = device->GetProperty(DIPROP_RANGE, &propertyAxisRange.diph);
                if (FAILED(hr))
                    throw SystemError("Failed to get DirectInput device axis range property, error: " + std::to_string(hr));

                for (uint32_t i = 0; i < 6; ++i)
                {
                    if (axis[i].axis != Gamepad::Axis::NONE &&
                        axis[i].usage == didObjectInstance->wUsage)
                    {
                        axis[i].min = propertyAxisRange.lMin;
                        axis[i].max = propertyAxisRange.lMax;
                        axis[i].range = propertyAxisRange.lMax - propertyAxisRange.lMin;

                        if (axis[i].axis == Gamepad::Axis::LEFT_TRIGGER) hasLeftTrigger = true;
                        else if (axis[i].axis == Gamepad::Axis::RIGHT_TRIGGER) hasRightTrigger = true;
                    }
                }
            }
        }
    } // namespace input
} // namespace ouzel
