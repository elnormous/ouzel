// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadDI.h"
#include "InputWin.h"
#include "core/Engine.h"
#include "utils/Log.h"

static const int32_t MIN_AXIS_VALUE = -32768;
static const int32_t MAX_AXIS_VALUE = 32767;
static const float THUMB_DEADZONE = 0.2f;

BOOL CALLBACK enumObjectsCallback(const DIDEVICEOBJECTINSTANCE* didObjectInstance, VOID* context)
{
    ouzel::input::GamepadDI* gamepadDI = static_cast<ouzel::input::GamepadDI*>(context);

    if (didObjectInstance->dwType & DIDFT_AXIS)
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = didObjectInstance->dwType; // Specify the enumerated axis
        diprg.lMin = MIN_AXIS_VALUE;
        diprg.lMax = MAX_AXIS_VALUE;

        // Set the range for the axis
        if (FAILED(gamepadDI->getDevice()->SetProperty(DIPROP_RANGE, &diprg.diph)))
            return DIENUM_STOP;
    }

    return DIENUM_CONTINUE;
}

namespace ouzel
{
    namespace input
    {
        GamepadDI::GamepadDI(const DIDEVICEINSTANCE* aInstance):
            instance(aInstance)
        {
            ZeroMemory(&diState, sizeof(state));

            vendorId = LOWORD(instance->guidProduct.Data1);
            productId = HIWORD(instance->guidProduct.Data1);

            InputWin* inputWin = static_cast<InputWin*>(sharedEngine->getInput());

            if (FAILED(inputWin->getDirectInput()->CreateDevice(instance->guidInstance, &device, nullptr)))
            {
                Log(Log::Level::ERR) << "Failed to create DirectInput device";
                return;
            }

            if (FAILED(device->SetDataFormat(&c_dfDIJoystick2)))
            {
                Log(Log::Level::ERR) << "Failed to set DirectInput device format";
                return;
            }

            if (FAILED(device->EnumObjects(enumObjectsCallback, this, DIDFT_ALL)))
            {
                Log(Log::Level::ERR) << "Failed to enumerate DirectInput device objects";
                return;
            }

            if (vendorId == 0x054C && productId == 0x0268) // Playstation 3 controller
            {
                buttonMap[0] = GamepadButton::BACK; // Select
                buttonMap[1] = GamepadButton::LEFT_THUMB; // L3
                buttonMap[2] = GamepadButton::RIGHT_THUMB; // R3
                buttonMap[3] = GamepadButton::START; // Start
                buttonMap[4] = GamepadButton::DPAD_UP;
                buttonMap[5] = GamepadButton::DPAD_RIGHT;
                buttonMap[6] = GamepadButton::DPAD_DOWN;
                buttonMap[7] = GamepadButton::DPAD_LEFT;
                buttonMap[8] = GamepadButton::LEFT_TRIGGER; // L2
                buttonMap[9] = GamepadButton::RIGHT_TRIGGER; // R2
                buttonMap[10] = GamepadButton::LEFT_SHOULDER; // L1
                buttonMap[11] = GamepadButton::RIGHT_SHOULDER; // R1
                buttonMap[12] = GamepadButton::FACE4; // Triangle
                buttonMap[13] = GamepadButton::FACE2; // Circle
                buttonMap[14] = GamepadButton::FACE1; // Cross
                buttonMap[15] = GamepadButton::FACE3; // Square

                leftThumbXMap = offsetof(DIJOYSTATE2, lX);
                leftThumbYMap = offsetof(DIJOYSTATE2, lY);
                leftTriggerMap = offsetof(DIJOYSTATE2, lRx);
                rightThumbXMap = offsetof(DIJOYSTATE2, lZ);
                rightThumbYMap = offsetof(DIJOYSTATE2, lRz);
                rightTriggerMap = offsetof(DIJOYSTATE2, lRy);
            }
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
            {
                buttonMap[0] = GamepadButton::FACE3; // Square
                buttonMap[1] = GamepadButton::FACE1; // Cross
                buttonMap[2] = GamepadButton::FACE2; // Circle
                buttonMap[3] = GamepadButton::FACE4; // Triangle
                buttonMap[4] = GamepadButton::LEFT_SHOULDER; // L1
                buttonMap[5] = GamepadButton::RIGHT_SHOULDER; // R1
                buttonMap[6] = GamepadButton::LEFT_TRIGGER; // L2
                buttonMap[7] = GamepadButton::RIGHT_TRIGGER; // R2
                buttonMap[8] = GamepadButton::BACK; // Share
                buttonMap[9] = GamepadButton::START; // Options
                buttonMap[10] = GamepadButton::LEFT_THUMB; // L3
                buttonMap[11] = GamepadButton::RIGHT_THUMB; // R3

                leftThumbXMap = offsetof(DIJOYSTATE2, lX);
                leftThumbYMap = offsetof(DIJOYSTATE2, lY);
                leftTriggerMap = offsetof(DIJOYSTATE2, lRx);
                rightThumbXMap = offsetof(DIJOYSTATE2, lZ);
                rightThumbYMap = offsetof(DIJOYSTATE2, lRz);
                rightTriggerMap = offsetof(DIJOYSTATE2, lRy);
            }
            else if (vendorId == 0x045E && productId == 0x02d1) // Xbox One controller
            {
                buttonMap[0] = GamepadButton::FACE1; // A
                buttonMap[1] = GamepadButton::FACE2; // B
                buttonMap[2] = GamepadButton::FACE3; // X
                buttonMap[3] = GamepadButton::FACE4; // Y
                buttonMap[4] = GamepadButton::LEFT_SHOULDER;
                buttonMap[5] = GamepadButton::RIGHT_SHOULDER;
                buttonMap[6] = GamepadButton::LEFT_THUMB;
                buttonMap[7] = GamepadButton::RIGHT_THUMB;
                buttonMap[8] = GamepadButton::BACK; // Menu
                buttonMap[9] = GamepadButton::START; // View
                buttonMap[11] = GamepadButton::DPAD_UP;
                buttonMap[12] = GamepadButton::DPAD_DOWN;
                buttonMap[13] = GamepadButton::DPAD_LEFT;
                buttonMap[14] = GamepadButton::DPAD_RIGHT;

                leftThumbXMap = offsetof(DIJOYSTATE2, lX);
                leftThumbYMap = offsetof(DIJOYSTATE2, lY);
                leftTriggerMap = offsetof(DIJOYSTATE2, lRy);
                rightThumbXMap = offsetof(DIJOYSTATE2, lZ);
                rightThumbYMap = offsetof(DIJOYSTATE2, lRx);
                rightTriggerMap = offsetof(DIJOYSTATE2, lRz);
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
                (vendorId == 0x0E6F && productId == 0x0401) || // GamepadforXbox360Z
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
                (vendorId == 0x15E4 && productId == 0x3F0A) || // XboxAirflowiredcontroller
                (vendorId == 0x0E6F && productId == 0x0401)) // GameStop XBox 360 Controller
            {
                buttonMap[0] = GamepadButton::FACE1; // A
                buttonMap[1] = GamepadButton::FACE2; // B
                buttonMap[2] = GamepadButton::FACE3; // X
                buttonMap[3] = GamepadButton::FACE4; // Y
                buttonMap[4] = GamepadButton::LEFT_SHOULDER;
                buttonMap[5] = GamepadButton::RIGHT_SHOULDER;
                buttonMap[6] = GamepadButton::LEFT_THUMB;
                buttonMap[7] = GamepadButton::RIGHT_THUMB;
                buttonMap[8] = GamepadButton::START;
                buttonMap[9] = GamepadButton::BACK;
                buttonMap[11] = GamepadButton::DPAD_UP;
                buttonMap[12] = GamepadButton::DPAD_DOWN;
                buttonMap[13] = GamepadButton::DPAD_LEFT;
                buttonMap[14] = GamepadButton::DPAD_RIGHT;

                leftThumbXMap = offsetof(DIJOYSTATE2, lX);
                leftThumbYMap = offsetof(DIJOYSTATE2, lY);
                leftTriggerMap = offsetof(DIJOYSTATE2, lZ);
                rightThumbXMap = offsetof(DIJOYSTATE2, lRx);
                rightThumbYMap = offsetof(DIJOYSTATE2, lRy);
                rightTriggerMap = offsetof(DIJOYSTATE2, lRz);
            }
            else // Generic (based on Logitech RumblePad 2)
            {
                buttonMap[0] = GamepadButton::FACE3;
                buttonMap[1] = GamepadButton::FACE1;
                buttonMap[2] = GamepadButton::FACE2;
                buttonMap[3] = GamepadButton::FACE4;
                buttonMap[4] = GamepadButton::LEFT_SHOULDER;
                buttonMap[5] = GamepadButton::RIGHT_SHOULDER;
                buttonMap[6] = GamepadButton::LEFT_TRIGGER;
                buttonMap[7] = GamepadButton::RIGHT_TRIGGER;
                buttonMap[8] = GamepadButton::BACK;
                buttonMap[9] = GamepadButton::START;
                buttonMap[10] = GamepadButton::LEFT_THUMB;
                buttonMap[11] = GamepadButton::RIGHT_THUMB;

                leftThumbXMap = offsetof(DIJOYSTATE2, lX);
                leftThumbYMap = offsetof(DIJOYSTATE2, lY);
                leftTriggerMap = offsetof(DIJOYSTATE2, lRx);
                rightThumbXMap = offsetof(DIJOYSTATE2, lZ);
                rightThumbYMap = offsetof(DIJOYSTATE2, lRz);
                rightTriggerMap = offsetof(DIJOYSTATE2, lRy);
            }
        }

        GamepadDI::~GamepadDI()
        {
            if (device)
            {
                device->Unacquire();
                device->Release();
            }
        }

        bool GamepadDI::update()
        {
            DIJOYSTATE2 newDIState;

            HRESULT hr = device->GetDeviceState(sizeof(newDIState), &newDIState);

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(device->Acquire()))
                {
                    Log(Log::Level::ERR) << "Failed to acquire DirectInput device";
                    return false;
                }

                hr = device->GetDeviceState(sizeof(newDIState), &newDIState);
            }

            if (hr != DI_OK)
            {
                return false;
            }

            for (uint32_t i = 0; i < 24; ++i)
            {
                if (newDIState.rgbButtons[i] != diState.rgbButtons[i] &&
                    buttonMap[i] != GamepadButton::NONE)
                {
                    handleButtonValueChange(buttonMap[i],
                                            newDIState.rgbButtons[i] > 0,
                                            (newDIState.rgbButtons[i] > 0) ? 1.0f : 0.0f);
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

                if ((bitmask & 0x01) != (newBitmask & 0x01)) handleButtonValueChange(GamepadButton::DPAD_UP,
                                                                                     (newBitmask & 0x01),
                                                                                     (newBitmask & 0x01) ? 1.0f : 0.0f);
                if ((bitmask & 0x02) != (newBitmask & 0x02)) handleButtonValueChange(GamepadButton::DPAD_RIGHT,
                                                                                     (newBitmask & 0x02),
                                                                                     (newBitmask & 0x02) ? 1.0f : 0.0f);
                if ((bitmask & 0x04) != (newBitmask & 0x04)) handleButtonValueChange(GamepadButton::DPAD_DOWN,
                                                                                     (newBitmask & 0x04),
                                                                                     (newBitmask & 0x04) ? 1.0f : 0.0f);
                if ((bitmask & 0x08) != (newBitmask & 0x08)) handleButtonValueChange(GamepadButton::DPAD_LEFT,
                                                                                     (newBitmask & 0x08),
                                                                                     (newBitmask & 0x08) ? 1.0f : 0.0f);
            }

            if (leftThumbXMap != 0xFFFFFFFF)
            {
                checkThumbAxisChange(diState, newDIState, leftThumbXMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                     GamepadButton::LEFT_THUMB_LEFT, GamepadButton::LEFT_THUMB_RIGHT);
            }
            if (leftThumbYMap != 0xFFFFFFFF)
            {
                checkThumbAxisChange(diState, newDIState, leftThumbYMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                     GamepadButton::LEFT_THUMB_UP, GamepadButton::LEFT_THUMB_DOWN);
            }
            if (rightThumbXMap != 0xFFFFFFFF)
            {
                checkThumbAxisChange(diState, newDIState, rightThumbXMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                     GamepadButton::RIGHT_THUMB_LEFT, GamepadButton::RIGHT_THUMB_RIGHT);
            }
            if (rightThumbYMap != 0xFFFFFFFF)
            {
                checkThumbAxisChange(diState, newDIState, rightThumbYMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                     GamepadButton::RIGHT_THUMB_UP, GamepadButton::RIGHT_THUMB_DOWN);
            }
            if (leftTriggerMap != 0xFFFFFFFF)
            {
                checkTriggerChange(diState, newDIState, leftTriggerMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                   GamepadButton::LEFT_TRIGGER);
            }
            if (rightTriggerMap != 0xFFFFFFFF)
            {
                checkTriggerChange(diState, newDIState, rightTriggerMap, MIN_AXIS_VALUE, MAX_AXIS_VALUE,
                                   GamepadButton::RIGHT_TRIGGER);
            }

            diState = newDIState;

            return true;
        }

        void GamepadDI::checkThumbAxisChange(const DIJOYSTATE2& oldState, const DIJOYSTATE2& newState,
                                             size_t offset, int64_t min, int64_t max,
                                             GamepadButton negativeButton, GamepadButton positiveButton)
        {
            LONG oldValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&oldState) + offset);
            LONG newValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&newState) + offset);

            if (oldValue != newValue)
            {
                float floatValue = 2.0f * (newValue - min) / (max - min) - 1.0f;

                if (floatValue > 0.0f)
                {
                    handleButtonValueChange(positiveButton,
                        floatValue > THUMB_DEADZONE,
                        floatValue);
                }
                else if (floatValue < 0.0f)
                {
                    handleButtonValueChange(negativeButton,
                        -floatValue > THUMB_DEADZONE,
                        -floatValue);
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                    {
                        handleButtonValueChange(positiveButton, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(negativeButton, false, 0.0f);
                    }
                }
            }
        }

        void GamepadDI::checkTriggerChange(const DIJOYSTATE2& oldState, const DIJOYSTATE2& newState,
                                           size_t offset, int64_t min, int64_t max,
                                           GamepadButton button)
        {
            LONG oldValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&oldState) + offset);
            LONG newValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&newState) + offset);

            if (oldValue != newValue)
            {
                float floatValue = 2.0f * (newValue - min) / (max - min) - 1.0f;

                handleButtonValueChange(button,
                                        floatValue > 0.0f,
                                        floatValue);
            }
        }
    } // namespace input
} // namespace ouzel
