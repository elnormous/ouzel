// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#import <Foundation/Foundation.h>
#include "GamepadMacOS.h"
#include "core/Engine.h"
#include "core/CompileConfig.h"
#include "events/EventDispatcher.h"
#include "utils/Log.h"
#include "utils/Utils.h"

static const float THUMB_DEADZONE = 0.2f;

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
            if (IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone) != kIOReturnSuccess)
            {
                Log(Log::Level::ERR) << "Failed to open HID device";
                return;
            }

            NSString* productName = (NSString*)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));
            if (productName)
            {
                name = [productName cStringUsingEncoding:NSUTF8StringEncoding];
            }

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
                usageMap[13] = GamepadButton::FACE4; // Triangle
                usageMap[14] = GamepadButton::FACE2; // Circle
                usageMap[15] = GamepadButton::FACE1; // Cross
                usageMap[16] = GamepadButton::FACE3; // Square

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
            {
                usageMap[1] = GamepadButton::FACE3; // Square
                usageMap[2] = GamepadButton::FACE1; // Cross
                usageMap[3] = GamepadButton::FACE2; // Circle
                usageMap[4] = GamepadButton::FACE4; // Triangle
                usageMap[5] = GamepadButton::LEFT_SHOULDER; // L1
                usageMap[6] = GamepadButton::RIGHT_SHOULDER; // R1
                usageMap[7] = GamepadButton::LEFT_TRIGGER; // L2
                usageMap[8] = GamepadButton::RIGHT_TRIGGER; // R2
                usageMap[9] = GamepadButton::BACK; // Share
                usageMap[10] = GamepadButton::START; // Options
                usageMap[11] = GamepadButton::LEFT_THUMB; // L3
                usageMap[12] = GamepadButton::RIGHT_THUMB; // R3

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x045E && productId == 0x02d1) // Xbox One controller
            {
                usageMap[1] = GamepadButton::FACE1; // A
                usageMap[2] = GamepadButton::FACE2; // B
                usageMap[3] = GamepadButton::FACE3; // X
                usageMap[4] = GamepadButton::FACE4; // Y
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

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Ry;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rx;
                rightTriggerMap = kHIDUsage_GD_Rz;
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
                usageMap[1] = GamepadButton::FACE1; // A
                usageMap[2] = GamepadButton::FACE2; // B
                usageMap[3] = GamepadButton::FACE3; // X
                usageMap[4] = GamepadButton::FACE4; // Y
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

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Z;
                rightThumbXMap = kHIDUsage_GD_Rx;
                rightThumbYMap = kHIDUsage_GD_Ry;
                rightTriggerMap = kHIDUsage_GD_Rz;
            }
            else // Generic (based on Logitech RumblePad 2)
            {
                usageMap[1] = GamepadButton::FACE3;
                usageMap[2] = GamepadButton::FACE1;
                usageMap[3] = GamepadButton::FACE2;
                usageMap[4] = GamepadButton::FACE4;
                usageMap[5] = GamepadButton::LEFT_SHOULDER;
                usageMap[6] = GamepadButton::RIGHT_SHOULDER;
                usageMap[7] = GamepadButton::LEFT_TRIGGER;
                usageMap[8] = GamepadButton::RIGHT_TRIGGER;
                usageMap[9] = GamepadButton::BACK;
                usageMap[10] = GamepadButton::START;
                usageMap[11] = GamepadButton::LEFT_THUMB;
                usageMap[12] = GamepadButton::RIGHT_THUMB;
                
                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }

            CFArrayRef elementArray = IOHIDDeviceCopyMatchingElements(device, NULL, kIOHIDOptionsTypeNone);

            for (CFIndex i = 0; i < CFArrayGetCount(elementArray); i++)
            {
                IOHIDElementRef elementRef = (IOHIDElementRef)CFArrayGetValueAtIndex(elementArray, i);

                Element element;
                element.type = IOHIDElementGetType(elementRef);
                element.usagePage = IOHIDElementGetUsagePage(elementRef);
                element.usage = IOHIDElementGetUsage(elementRef);

                if ((element.type == kIOHIDElementTypeInput_Button && element.usagePage == kHIDPage_Button && element.usage < 24) ||
                    ((element.type == kIOHIDElementTypeInput_Misc || element.type == kIOHIDElementTypeInput_Axis) && element.usagePage == kHIDPage_GenericDesktop))
                {
                    element.min = IOHIDElementGetLogicalMin(elementRef);
                    element.max = IOHIDElementGetLogicalMax(elementRef);

                    elements.insert(std::make_pair(elementRef, element));
                }
            }
            
            CFRelease(elementArray);

            IOHIDDeviceRegisterInputValueCallback(device, deviceInput, this);
        }

        void GamepadMacOS::handleInput(IOHIDValueRef value)
        {
            IOHIDElementRef elementRef = IOHIDValueGetElement(value);

            std::map<IOHIDElementRef, Element>::const_iterator i = elements.find(elementRef);

            if (i != elements.end())
            {
                State newState = state;

                const Element& element = i->second;

                CFIndex newValue = IOHIDValueGetIntegerValue(value);

                if (element.usagePage == kHIDPage_Button)
                {
                    GamepadButton button = usageMap[element.usage];

                    if (button != GamepadButton::NONE)
                    {
                        handleButtonValueChange(button, newValue > 0, (newValue > 0) ? 1.0f : 0.0f);
                    }
                }
                else if (element.usage == leftThumbXMap)
                {
                    newState.leftThumbX = newValue;

                    handleThumbAxisChange(state.leftThumbX, newState.leftThumbX,
                                          element.min, element.max,
                                          GamepadButton::LEFT_THUMB_LEFT, GamepadButton::LEFT_THUMB_RIGHT);
                }
                else if (element.usage == leftThumbYMap)
                {
                    newState.leftThumbY = newValue;

                    handleThumbAxisChange(state.leftThumbY, newState.leftThumbY,
                                          element.min, element.max,
                                          GamepadButton::LEFT_THUMB_UP, GamepadButton::LEFT_THUMB_DOWN);
                }
                else if (element.usage == leftTriggerMap)
                {
                    float floatValue = static_cast<float>(newValue - element.min) / (element.max - element.min);

                    handleButtonValueChange(GamepadButton::LEFT_TRIGGER, newValue > 0, floatValue);
                }
                else if (element.usage == rightThumbXMap)
                {
                    newState.rightThumbX = newValue;

                    handleThumbAxisChange(state.rightThumbX, newState.rightThumbX,
                                          element.min, element.max,
                                          GamepadButton::RIGHT_THUMB_LEFT, GamepadButton::RIGHT_THUMB_RIGHT);
                }
                else if (element.usage == rightThumbYMap)
                {
                    newState.rightThumbY = newValue;

                    handleThumbAxisChange(state.rightThumbY, newState.rightThumbY,
                                          element.min, element.max,
                                          GamepadButton::RIGHT_THUMB_UP, GamepadButton::RIGHT_THUMB_DOWN);
                }
                else if (element.usage == rightTriggerMap)
                {
                    float floatValue = static_cast<float>(newValue - element.min) / (element.max - element.min);

                    handleButtonValueChange(GamepadButton::RIGHT_TRIGGER, newValue > 0, floatValue);
                }
                else if (element.usage == kHIDUsage_GD_Hatswitch)
                {
                    switch (newValue)
                    {
                        case 0:
                            newState.dpadLeft = false;
                            newState.dpadRight = false;
                            newState.dpadUp = true;
                            newState.dpadDown = false;
                            break;
                        case 1:
                            newState.dpadLeft = false;
                            newState.dpadRight = true;
                            newState.dpadUp = true;
                            newState.dpadDown = false;
                            break;
                        case 2:
                            newState.dpadLeft = false;
                            newState.dpadRight = true;
                            newState.dpadUp = false;
                            newState.dpadDown = false;
                            break;
                        case 3:
                            newState.dpadLeft = false;
                            newState.dpadRight = true;
                            newState.dpadUp = false;
                            newState.dpadDown = true;
                            break;
                        case 4:
                            newState.dpadLeft = false;
                            newState.dpadRight = false;
                            newState.dpadUp = false;
                            newState.dpadDown = true;
                            break;
                        case 5:
                            newState.dpadLeft = true;
                            newState.dpadRight = false;
                            newState.dpadUp = false;
                            newState.dpadDown = true;
                            break;
                        case 6:
                            newState.dpadLeft = true;
                            newState.dpadRight = false;
                            newState.dpadUp = false;
                            newState.dpadDown = false;
                            break;
                        case 7:
                            newState.dpadLeft = true;
                            newState.dpadRight = false;
                            newState.dpadUp = true;
                            newState.dpadDown = false;
                            break;
                        case 8:
                            newState.dpadLeft = false;
                            newState.dpadRight = false;
                            newState.dpadUp = false;
                            newState.dpadDown = false;
                            break;
                    }

                    if (newState.dpadLeft != state.dpadLeft) handleButtonValueChange(GamepadButton::DPAD_LEFT,
                                                                                     newState.dpadLeft,
                                                                                     newState.dpadLeft ? 1.0f : 0.0f);
                    if (newState.dpadRight != state.dpadRight) handleButtonValueChange(GamepadButton::DPAD_RIGHT,
                                                                                       newState.dpadRight,
                                                                                       newState.dpadRight ? 1.0f : 0.0f);
                    if (newState.dpadUp != state.dpadUp) handleButtonValueChange(GamepadButton::DPAD_UP,
                                                                                 newState.dpadUp,
                                                                                 newState.dpadUp ? 1.0f : 0.0f);
                    if (newState.dpadDown != state.dpadDown) handleButtonValueChange(GamepadButton::DPAD_DOWN,
                                                                                     newState.dpadDown,
                                                                                     newState.dpadDown ? 1.0f : 0.0f);
                }

                state = newState;
            }
        }

        void GamepadMacOS::handleThumbAxisChange(int64_t oldValue, int64_t newValue,
                                                 int64_t min, int64_t max,
                                                 GamepadButton negativeButton, GamepadButton positiveButton)
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
    } // namespace input
} // namespace ouzel
