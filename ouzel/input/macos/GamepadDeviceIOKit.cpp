// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <vector>
#include "GamepadDeviceIOKit.hpp"
#include "InputSystemMacOS.hpp"
#include "input/GamepadConfig.hpp"
#include "core/Setup.h"
#include "utils/Errors.hpp"

static const float THUMB_DEADZONE = 0.2F;

static void deviceInput(void* ctx, IOReturn, void*, IOHIDValueRef value)
{
    ouzel::input::GamepadDeviceIOKit* gamepadDevice = reinterpret_cast<ouzel::input::GamepadDeviceIOKit*>(ctx);
    gamepadDevice->handleInput(value);
}

namespace ouzel
{
    namespace input
    {
        GamepadDeviceIOKit::GamepadDeviceIOKit(InputSystem& initInputSystem,
                                               uint32_t initId,
                                               IOHIDDeviceRef initDevice):
            GamepadDeviceMacOS(initInputSystem, initId),
            device(initDevice)
        {
            IOReturn ret = IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
            if (ret != kIOReturnSuccess)
                throw SystemError("Failed to open HID device, error: " + std::to_string(ret));

            CFStringRef productName = static_cast<CFStringRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
            if (productName)
            {
                const char* deviceName = CFStringGetCStringPtr(productName, kCFStringEncodingUTF8);
                if (!deviceName)
                {
                    CFIndex stringLength = CFStringGetLength(productName);
                    std::vector<char> temp(static_cast<size_t>(CFStringGetMaximumSizeForEncoding(stringLength, kCFStringEncodingUTF8)) + 1);
                    CFStringGetCString(productName, temp.data(), static_cast<CFIndex>(temp.size()), kCFStringEncodingUTF8);
                    deviceName = temp.data();
                }
                name = deviceName;
            }

            CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
            if (vendor)
                CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

            CFNumberRef product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
            if (product)
                CFNumberGetValue(product, kCFNumberSInt32Type, &productId);

            uint32_t leftThumbXMap = 0;
            uint32_t leftThumbYMap = 0;
            uint32_t leftTriggerMap = 0;
            uint32_t rightThumbXMap = 0;
            uint32_t rightThumbYMap = 0;
            uint32_t rightTriggerMap = 0;
            std::array<Gamepad::Button, 24> usageMap;

            if (vendorId == 0x054C && productId == 0x0268) // Playstation 3 controller
            {
                usageMap = PLAYSTATION_3_BUTTON_MAP;

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x054C && productId == 0x05C4) // Playstation 4 controller
            {
                usageMap = PLAYSTATION_4_BUTTON_MAP;

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }
            else if (vendorId == 0x045E && productId == 0x02D1) // Xbox One controller
            {
                usageMap = XBOX_ONE_BUTTON_MAP;

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
                usageMap = XBOX_360_BUTTON_MAP;

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Z;
                rightThumbXMap = kHIDUsage_GD_Rx;
                rightThumbYMap = kHIDUsage_GD_Ry;
                rightTriggerMap = kHIDUsage_GD_Rz;
            }
            else if (vendorId == 0x0079 && productId == 0x0006) // Acme GA07
            {
                usageMap = ACME_GA07_BUTTON_MAP;

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                rightThumbXMap = kHIDUsage_GD_Rx;
                rightThumbYMap = kHIDUsage_GD_Ry;
            }
            else // Generic (based on Logitech RumblePad 2)
            {
                usageMap = GENERIC_BUTTON_MAP;

                leftThumbXMap = kHIDUsage_GD_X;
                leftThumbYMap = kHIDUsage_GD_Y;
                leftTriggerMap = kHIDUsage_GD_Rx;
                rightThumbXMap = kHIDUsage_GD_Z;
                rightThumbYMap = kHIDUsage_GD_Rz;
                rightTriggerMap = kHIDUsage_GD_Ry;
            }

            CFArrayRef elementArray = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);
            CFIndex count = CFArrayGetCount(elementArray);

            for (CFIndex i = 0; i < count; ++i)
            {
                Element element;
                element.element = static_cast<IOHIDElementRef>(const_cast<void*>(CFArrayGetValueAtIndex(elementArray, i)));
                element.type = IOHIDElementGetType(element.element);
                element.usagePage = IOHIDElementGetUsagePage(element.element);
                element.usage = IOHIDElementGetUsage(element.element);

                if (element.usage > 0 && element.usage < 25)
                    element.button = usageMap[element.usage - 1];

                if ((element.type == kIOHIDElementTypeInput_Misc || element.type == kIOHIDElementTypeInput_Axis) &&
                    element.usagePage == kHIDPage_GenericDesktop)
                {
                    if (element.usage == leftThumbXMap) leftThumbX = element.element;
                    else if (element.usage == leftThumbYMap) leftThumbY = element.element;
                    else if (element.usage == leftTriggerMap) leftTrigger = element.element;
                    else if (element.usage == rightThumbXMap) rightThumbX = element.element;
                    else if (element.usage == rightThumbYMap) rightThumbY = element.element;
                    else if (element.usage == rightTriggerMap) rightTrigger = element.element;
                }

                if ((element.type == kIOHIDElementTypeInput_Button && element.usagePage == kHIDPage_Button && element.usage > 0 && element.usage < 25) ||
                    ((element.type == kIOHIDElementTypeInput_Misc || element.type == kIOHIDElementTypeInput_Axis) && element.usagePage == kHIDPage_GenericDesktop))
                {
                    element.min = IOHIDElementGetLogicalMin(element.element);
                    element.max = IOHIDElementGetLogicalMax(element.element);

                    elements.insert(std::make_pair(element.element, element));
                }
            }

            CFRelease(elementArray);

            IOHIDDeviceRegisterInputValueCallback(device, deviceInput, this);
        }

        void GamepadDeviceIOKit::handleInput(IOHIDValueRef value)
        {
            IOHIDElementRef elementRef = IOHIDValueGetElement(value);

            auto i = elements.find(elementRef);

            if (i != elements.end())
            {
                Element& element = i->second;

                CFIndex newValue = IOHIDValueGetIntegerValue(value);

                if (element.usagePage == kHIDPage_Button)
                {
                    if (element.button != Gamepad::Button::NONE &&
                        (element.button != Gamepad::Button::LEFT_TRIGGER || !leftThumbX) && // don't send digital trigger if analog trigger exists
                        (element.button != Gamepad::Button::RIGHT_TRIGGER || !rightThumbY))
                        handleButtonValueChange(element.button, newValue > 0, (newValue > 0) ? 1.0F : 0.0F);
                }
                else if (elementRef == leftThumbX)
                {
                    handleThumbAxisChange(element.value, newValue,
                                          element.min, element.max,
                                          Gamepad::Button::LEFT_THUMB_LEFT, Gamepad::Button::LEFT_THUMB_RIGHT);
                }
                else if (elementRef == leftThumbY)
                {
                    handleThumbAxisChange(element.value, newValue,
                                          element.min, element.max,
                                          Gamepad::Button::LEFT_THUMB_UP, Gamepad::Button::LEFT_THUMB_DOWN);
                }
                else if (elementRef == leftTrigger)
                {
                    float floatValue = static_cast<float>(newValue - element.min) / (element.max - element.min);

                    handleButtonValueChange(Gamepad::Button::LEFT_TRIGGER, newValue > 0, floatValue);
                }
                else if (elementRef == rightThumbX)
                {
                    handleThumbAxisChange(element.value, newValue,
                                          element.min, element.max,
                                          Gamepad::Button::RIGHT_THUMB_LEFT, Gamepad::Button::RIGHT_THUMB_RIGHT);
                }
                else if (elementRef == rightThumbY)
                {
                    handleThumbAxisChange(element.value, newValue,
                                          element.min, element.max,
                                          Gamepad::Button::RIGHT_THUMB_UP, Gamepad::Button::RIGHT_THUMB_DOWN);
                }
                else if (elementRef == rightTrigger)
                {
                    float floatValue = static_cast<float>(newValue - element.min) / (element.max - element.min);

                    handleButtonValueChange(Gamepad::Button::RIGHT_TRIGGER, newValue > 0, floatValue);
                }
                else if (element.usage == kHIDUsage_GD_Hatswitch)
                {
                    uint32_t bitmask = (element.value >= 8) ? 0 : (1 << (element.value / 2)) | // first bit
                        (1 << (element.value / 2 + element.value % 2)) % 4; // second bit

                    uint32_t newBitmask = (newValue >= 8) ? 0 : (1 << (newValue / 2)) | // first bit
                        (1 << (newValue / 2 + newValue % 2)) % 4; // second bit

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

                element.value = newValue;
            }
        }

        void GamepadDeviceIOKit::handleThumbAxisChange(int64_t oldValue, int64_t newValue,
                                                       int64_t min, int64_t max,
                                                       Gamepad::Button negativeButton,
                                                       Gamepad::Button positiveButton)
        {
            float floatValue = 2.0F * (newValue - min) / (max - min) - 1.0F;

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
    } // namespace input
} // namespace ouzel
