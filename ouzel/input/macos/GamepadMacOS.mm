// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Foundation/Foundation.h>
#include "GamepadMacOS.h"
#include "core/Engine.h"
#include "core/CompileConfig.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"
#include "utils/Utils.h"

static void deviceInput(void* ctx, IOReturn, void*, IOHIDValueRef value)
{
    ouzel::input::GamepadMacOS* gamepad = reinterpret_cast<ouzel::input::GamepadMacOS*>(ctx);
    gamepad->handleInput(value);
}

namespace ouzel
{
    namespace input
    {
        GamepadMacOS::GamepadMacOS(IOHIDDeviceRef aDevice):
            device(aDevice)
        {
            NSNumber* vendor = (NSNumber*)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey));
            if (vendor)
            {
                vendorId = [vendor unsignedIntValue];
            }

            NSNumber* product = (NSNumber*)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey));
            if (product)
            {
                productId = [product unsignedIntValue];
            }

            std::fill(std::begin(usageMap), std::end(usageMap), GamepadButton::NONE);

            if (vendorId == 0x054C && productId == 0x0268) // Playstation 3 controller
            {
                usageMap[1] = GamepadButton::BACK; // Select
                usageMap[2] = GamepadButton::LEFT_THUMB; // L3
                usageMap[3] = GamepadButton::RIGHT_THUMB; // R3
                usageMap[4] = GamepadButton::START; // Start
                usageMap[5] = GamepadButton::DPAD_UP;
                usageMap[6] = GamepadButton::DPAD_RIGHT;
                usageMap[7] = GamepadButton::DPAD_DOWN;
                usageMap[8] = GamepadButton::DPAD_LEFT;
                usageMap[9] = GamepadButton::LEFT_TRIGGER; // L2
                usageMap[10] = GamepadButton::RIGHT_TRIGGER; // R2
                usageMap[11] = GamepadButton::LEFT_SHOULDER; // L1
                usageMap[12] = GamepadButton::RIGHT_SHOULDER; // R1
                usageMap[13] = GamepadButton::Y; // Triangle
                usageMap[14] = GamepadButton::B; // Circle
                usageMap[15] = GamepadButton::A; // Cross
                usageMap[16] = GamepadButton::X; // Square

                leftAnalogXMap = kHIDUsage_GD_X;
                leftAnalogYMap = kHIDUsage_GD_Y;
                leftTriggerAnalogMap = kHIDUsage_GD_Rx;
                rightAnalogXMap = kHIDUsage_GD_Z;
                rightAnalogYMap = kHIDUsage_GD_Rz;
                rightTriggerAnalogMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
            {
                usageMap[1] = GamepadButton::X; // Square
                usageMap[2] = GamepadButton::A; // Cross
                usageMap[3] = GamepadButton::B; // Circle
                usageMap[4] = GamepadButton::Y; // Triangle
                usageMap[5] = GamepadButton::LEFT_SHOULDER; // L1
                usageMap[6] = GamepadButton::RIGHT_SHOULDER; // R1
                usageMap[7] = GamepadButton::LEFT_TRIGGER; // L2
                usageMap[8] = GamepadButton::RIGHT_TRIGGER; // R2
                usageMap[9] = GamepadButton::BACK; // Share
                usageMap[10] = GamepadButton::START; // Options
                usageMap[11] = GamepadButton::LEFT_THUMB; // L3
                usageMap[12] = GamepadButton::RIGHT_THUMB; // R3

                leftAnalogXMap = kHIDUsage_GD_X;
                leftAnalogYMap = kHIDUsage_GD_Y;
                leftTriggerAnalogMap = kHIDUsage_GD_Rx;
                rightAnalogXMap = kHIDUsage_GD_Z;
                rightAnalogYMap = kHIDUsage_GD_Rz;
                rightTriggerAnalogMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x045E && productId == 0x02d1) // Xbox One controller
            {
                usageMap[1] = GamepadButton::A;
                usageMap[2] = GamepadButton::B;
                usageMap[3] = GamepadButton::X;
                usageMap[4] = GamepadButton::Y;
                usageMap[5] = GamepadButton::LEFT_SHOULDER;
                usageMap[6] = GamepadButton::RIGHT_SHOULDER;
                usageMap[7] = GamepadButton::LEFT_THUMB;
                usageMap[8] = GamepadButton::RIGHT_THUMB;
                usageMap[9] = GamepadButton::BACK; // Menu
                usageMap[10] = GamepadButton::START; // View
                usageMap[12] = GamepadButton::DPAD_UP;
                usageMap[13] = GamepadButton::DPAD_DOWN;
                usageMap[14] = GamepadButton::DPAD_LEFT;
                usageMap[15] = GamepadButton::DPAD_RIGHT;

                leftAnalogXMap = kHIDUsage_GD_X;
                leftAnalogYMap = kHIDUsage_GD_Y;
                leftTriggerAnalogMap = kHIDUsage_GD_Ry;
                rightAnalogXMap = kHIDUsage_GD_Z;
                rightAnalogYMap = kHIDUsage_GD_Rx;
                rightTriggerAnalogMap = kHIDUsage_GD_Rz;
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
                usageMap[1] = GamepadButton::A;
                usageMap[2] = GamepadButton::B;
                usageMap[3] = GamepadButton::X;
                usageMap[4] = GamepadButton::Y;
                usageMap[5] = GamepadButton::LEFT_SHOULDER;
                usageMap[6] = GamepadButton::RIGHT_SHOULDER;
                usageMap[7] = GamepadButton::LEFT_THUMB;
                usageMap[8] = GamepadButton::RIGHT_THUMB;
                usageMap[9] = GamepadButton::START;
                usageMap[10] = GamepadButton::BACK;
                usageMap[12] = GamepadButton::DPAD_UP;
                usageMap[13] = GamepadButton::DPAD_DOWN;
                usageMap[14] = GamepadButton::DPAD_LEFT;
                usageMap[15] = GamepadButton::DPAD_RIGHT;

                leftAnalogXMap = kHIDUsage_GD_X;
                leftAnalogYMap = kHIDUsage_GD_Y;
                leftTriggerAnalogMap = kHIDUsage_GD_Z;
                rightAnalogXMap = kHIDUsage_GD_Rx;
                rightAnalogYMap = kHIDUsage_GD_Ry;
                rightTriggerAnalogMap = kHIDUsage_GD_Rz;
            }
            else // Generic (based on Logitech Rum/blePad 2)
            {
                usageMap[1] = GamepadButton::X;
                usageMap[2] = GamepadButton::A;
                usageMap[3] = GamepadButton::B;
                usageMap[4] = GamepadButton::Y;
                usageMap[5] = GamepadButton::LEFT_SHOULDER;
                usageMap[6] = GamepadButton::RIGHT_SHOULDER;
                usageMap[7] = GamepadButton::LEFT_TRIGGER;
                usageMap[8] = GamepadButton::RIGHT_TRIGGER;
                usageMap[9] = GamepadButton::BACK;
                usageMap[10] = GamepadButton::START;
                usageMap[11] = GamepadButton::LEFT_THUMB;
                usageMap[12] = GamepadButton::RIGHT_THUMB;
                
                leftAnalogXMap = kHIDUsage_GD_X;
                leftAnalogYMap = kHIDUsage_GD_Y;
                leftTriggerAnalogMap = kHIDUsage_GD_Rx;
                rightAnalogXMap = kHIDUsage_GD_Z;
                rightAnalogYMap = kHIDUsage_GD_Rz;
                rightTriggerAnalogMap = kHIDUsage_GD_Ry;
            }

            IOHIDDeviceRegisterInputValueCallback(device, deviceInput, this);
        }

        void GamepadMacOS::handleInput(IOHIDValueRef value)
        {
            IOHIDElementRef element = IOHIDValueGetElement(value);

            IOHIDElementType elementType = IOHIDElementGetType(element);
            uint32_t usage = IOHIDElementGetUsage(element);
            uint32_t usagePage = IOHIDElementGetUsagePage(element);

            CFIndex min = IOHIDElementGetPhysicalMin(element);
            CFIndex max = IOHIDElementGetPhysicalMax(element);

            min = IOHIDElementGetPhysicalMin(element);
            max = IOHIDElementGetPhysicalMax(element);

            if (elementType == kIOHIDElementTypeInput_Misc ||
                elementType == kIOHIDElementTypeInput_Axis ||
                elementType == kIOHIDElementTypeInput_Button)
            {
                if (max - min == 1 ||
                    usagePage == kHIDPage_Button)
                {
                    if (usage > 0 && usage < 24)
                    {
                        GamepadButton button = usageMap[usage];

                        if (button != GamepadButton::NONE)
                        {
                            CFIndex integerValue = IOHIDValueGetIntegerValue(value);

                            handleButtonValueChange(button, integerValue > 0, integerValue);
                        }
                    }
                }
            }
        }
    } // namespace input
} // namespace ouzel
