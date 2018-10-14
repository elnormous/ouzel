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

            if (vendorId == 0x054C && productId == 0x0268) // Playstation 3 controller
            {
                buttonMap = PLAYSTATION_3_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_Z;
                rightThumbX.offset = DIJOFS_Z;

                rightThumbY.usage = HID_USAGE_GENERIC_RZ;
                rightThumbY.offset = DIJOFS_RZ;

                leftTrigger.usage = HID_USAGE_GENERIC_RX;
                leftTrigger.offset = DIJOFS_RX;

                rightTrigger.usage = HID_USAGE_GENERIC_RY;
                rightTrigger.offset = DIJOFS_RY;
            }
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
            {
                buttonMap = PLAYSTATION_4_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_Z;
                rightThumbX.offset = DIJOFS_Z;

                rightThumbY.usage = HID_USAGE_GENERIC_RZ;
                rightThumbY.offset = DIJOFS_RZ;

                leftTrigger.usage = HID_USAGE_GENERIC_RX;
                leftTrigger.offset = DIJOFS_RX;

                rightTrigger.usage = HID_USAGE_GENERIC_RY;
                rightTrigger.offset = DIJOFS_RY;
            }
            else if (vendorId == 0x045E && productId == 0x02D1) // Xbox One controller
            {
                buttonMap = XBOX_ONE_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_Z;
                rightThumbX.offset = DIJOFS_Z;

                rightThumbY.usage = HID_USAGE_GENERIC_RX;
                rightThumbY.offset = DIJOFS_RX;

                leftTrigger.usage = HID_USAGE_GENERIC_RY;
                leftTrigger.offset = DIJOFS_RY;

                rightTrigger.usage = HID_USAGE_GENERIC_RZ;
                rightTrigger.offset = DIJOFS_RZ;
            }
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
            {
                buttonMap = XBOX_360_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_RX;
                rightThumbX.offset = DIJOFS_RX;

                rightThumbY.usage = HID_USAGE_GENERIC_RY;
                rightThumbY.offset = DIJOFS_RY;

                leftTrigger.usage = HID_USAGE_GENERIC_Z;
                leftTrigger.offset = DIJOFS_Z;

                rightTrigger.usage = HID_USAGE_GENERIC_RZ;
                rightTrigger.offset = DIJOFS_RZ;
            }
            else if (vendorId == 0x0079 && productId == 0x0006) // Acme GA07
            {
                buttonMap = ACME_GA07_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_RX;
                rightThumbX.offset = DIJOFS_RX;

                rightThumbY.usage = HID_USAGE_GENERIC_RY;
                rightThumbY.offset = DIJOFS_RY;
            }
            else // Generic (based on Logitech RumblePad 2)
            {
                buttonMap = GENERIC_CONFIG.buttonMap;

                leftThumbX.usage = HID_USAGE_GENERIC_X;
                leftThumbX.offset = DIJOFS_X;

                leftThumbY.usage = HID_USAGE_GENERIC_Y;
                leftThumbY.offset = DIJOFS_Y;

                rightThumbX.usage = HID_USAGE_GENERIC_Z;
                rightThumbX.offset = DIJOFS_Z;

                rightThumbY.usage = HID_USAGE_GENERIC_RZ;
                rightThumbY.offset = DIJOFS_RZ;

                leftTrigger.usage = HID_USAGE_GENERIC_RX;
                leftTrigger.offset = DIJOFS_RX;

                rightTrigger.usage = HID_USAGE_GENERIC_RY;
                rightTrigger.offset = DIJOFS_RY;
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
                    if (events[e].dwOfs == offsetof(DIJOYSTATE2, rgbButtons) + i)
                    {
                        Gamepad::Button button = buttonMap[i];

                        if (button != Gamepad::Button::NONE &&
                            (button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                            (button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                        {
                            handleButtonValueChange(button,
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

                if (leftThumbX.offset == events[e].dwOfs)
                {
                    checkThumbAxisChange(getAxisValue(diState, leftThumbX.offset),
                                         events[e].dwData,
                                         leftThumbX.min, leftThumbX.range,
                                         Gamepad::Button::LEFT_THUMB_LEFT, Gamepad::Button::LEFT_THUMB_RIGHT);

                    setAxisValue(diState, leftThumbX.offset, events[e].dwData);
                }
                if (leftThumbY.offset == events[e].dwOfs)
                {
                    checkThumbAxisChange(getAxisValue(diState, leftThumbY.offset),
                                         events[e].dwData,
                                         leftThumbY.min, leftThumbY.range,
                                         Gamepad::Button::LEFT_THUMB_UP, Gamepad::Button::LEFT_THUMB_DOWN);

                    setAxisValue(diState, leftThumbY.offset, events[e].dwData);
                }
                if (rightThumbX.offset == events[e].dwOfs)
                {
                    checkThumbAxisChange(getAxisValue(diState, rightThumbX.offset),
                                         events[e].dwData,
                                         rightThumbX.min, rightThumbX.range,
                                         Gamepad::Button::RIGHT_THUMB_LEFT, Gamepad::Button::RIGHT_THUMB_RIGHT);

                    setAxisValue(diState, rightThumbX.offset, events[e].dwData);
                }
                if (rightThumbY.offset == events[e].dwOfs)
                {
                    checkThumbAxisChange(getAxisValue(diState, rightThumbY.offset),
                                         events[e].dwData,
                                         rightThumbY.min, rightThumbY.range,
                                         Gamepad::Button::RIGHT_THUMB_UP, Gamepad::Button::RIGHT_THUMB_DOWN);

                    setAxisValue(diState, rightThumbY.offset, events[e].dwData);
                }
                if (leftTrigger.offset == events[e].dwOfs)
                {
                    checkTriggerChange(getAxisValue(diState, leftTrigger.offset),
                                       events[e].dwData,
                                       leftTrigger.min, leftTrigger.range,
                                       Gamepad::Button::LEFT_TRIGGER);

                    setAxisValue(diState, leftTrigger.offset, events[e].dwData);
                }
                if (rightTrigger.offset == events[e].dwOfs)
                {
                    checkTriggerChange(getAxisValue(diState, rightTrigger.offset),
                                       events[e].dwData,
                                       rightTrigger.min, rightTrigger.range,
                                       Gamepad::Button::RIGHT_TRIGGER);

                    setAxisValue(diState, rightTrigger.offset, events[e].dwData);
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
                if (newDIState.rgbButtons[i] != diState.rgbButtons[i])
                {
                    Gamepad::Button button = buttonMap[i];

                    if (button != Gamepad::Button::NONE &&
                        (button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                        (button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                    {
                        handleButtonValueChange(button,
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

            if (leftThumbX.offset != 0xFFFFFFFF)
            {
                checkThumbAxisChange(getAxisValue(diState, leftThumbX.offset),
                                     getAxisValue(newDIState, leftThumbX.offset),
                                     leftThumbX.min, leftThumbX.range,
                                     Gamepad::Button::LEFT_THUMB_LEFT, Gamepad::Button::LEFT_THUMB_RIGHT);
            }
            if (leftThumbY.offset != 0xFFFFFFFF)
            {
                checkThumbAxisChange(getAxisValue(diState, leftThumbY.offset),
                                     getAxisValue(newDIState, leftThumbY.offset),
                                     leftThumbY.min, leftThumbY.range,
                                     Gamepad::Button::LEFT_THUMB_UP, Gamepad::Button::LEFT_THUMB_DOWN);
            }
            if (rightThumbX.offset != 0xFFFFFFFF)
            {
                checkThumbAxisChange(getAxisValue(diState, rightThumbX.offset),
                                     getAxisValue(newDIState, rightThumbX.offset),
                                     rightThumbX.min, rightThumbX.range,
                                     Gamepad::Button::RIGHT_THUMB_LEFT, Gamepad::Button::RIGHT_THUMB_RIGHT);
            }
            if (rightThumbY.offset != 0xFFFFFFFF)
            {
                checkThumbAxisChange(getAxisValue(diState, rightThumbY.offset),
                                     getAxisValue(newDIState, rightThumbY.offset),
                                     rightThumbY.min, rightThumbY.range,
                                     Gamepad::Button::RIGHT_THUMB_UP, Gamepad::Button::RIGHT_THUMB_DOWN);
            }
            if (leftTrigger.offset != 0xFFFFFFFF)
            {
                checkTriggerChange(getAxisValue(diState, leftTrigger.offset),
                                   getAxisValue(newDIState, leftTrigger.offset),
                                   leftTrigger.min, leftTrigger.range,
                                   Gamepad::Button::LEFT_TRIGGER);
            }
            if (rightTrigger.offset != 0xFFFFFFFF)
            {
                checkTriggerChange(getAxisValue(diState, rightTrigger.offset),
                                   getAxisValue(newDIState, rightTrigger.offset),
                                   rightTrigger.min, rightTrigger.range,
                                   Gamepad::Button::RIGHT_TRIGGER);
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

                if (leftThumbX.usage && didObjectInstance->wUsage == leftThumbX.usage)
                {
                    leftThumbX.min = propertyAxisRange.lMin;
                    leftThumbX.max = propertyAxisRange.lMax;
                    leftThumbX.range = leftThumbX.max - leftThumbX.min;
                }
                if (leftThumbY.usage && didObjectInstance->wUsage == leftThumbY.usage)
                {
                    leftThumbY.min = propertyAxisRange.lMin;
                    leftThumbY.max = propertyAxisRange.lMax;
                    leftThumbY.range = leftThumbY.max - leftThumbY.min;
                }
                else if (rightThumbX.usage && didObjectInstance->wUsage == rightThumbX.usage)
                {
                    rightThumbX.min = propertyAxisRange.lMin;
                    rightThumbX.max = propertyAxisRange.lMax;
                    rightThumbX.range = rightThumbX.max - rightThumbX.min;
                }
                else if (rightThumbY.usage && didObjectInstance->wUsage == rightThumbY.usage)
                {
                    rightThumbY.min = propertyAxisRange.lMin;
                    rightThumbY.max = propertyAxisRange.lMax;
                    rightThumbY.range = rightThumbY.max - rightThumbY.min;
                }
                else if (leftTrigger.usage && didObjectInstance->wUsage == leftTrigger.usage)
                {
                    leftTrigger.min = propertyAxisRange.lMin;
                    leftTrigger.max = propertyAxisRange.lMax;
                    leftTrigger.range = leftTrigger.max - leftTrigger.min;
                    hasLeftTrigger = true;
                }
                else if (rightTrigger.usage && didObjectInstance->wUsage == rightTrigger.usage)
                {
                    rightTrigger.min = propertyAxisRange.lMin;
                    rightTrigger.max = propertyAxisRange.lMax;
                    rightTrigger.range = rightTrigger.max - rightTrigger.min;
                    hasRightTrigger = true;
                }
            }
        }
    } // namespace input
} // namespace ouzel
