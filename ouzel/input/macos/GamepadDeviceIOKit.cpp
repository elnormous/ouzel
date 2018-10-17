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

            int32_t vendorId;
            CFNumberRef vendor = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
            if (!vendor)
                throw SystemError("Failed to get vendor ID");

            CFNumberGetValue(vendor, kCFNumberSInt32Type, &vendorId);

            int32_t productId;
            CFNumberRef product = static_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
            if (!product)
                throw SystemError("Failed to get product ID");

            CFNumberGetValue(product, kCFNumberSInt32Type, &productId);

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

            static const std::unordered_map<uint32_t, size_t> axisUsageMap = {
                {kHIDUsage_GD_X, 0},
                {kHIDUsage_GD_Y, 1},
                {kHIDUsage_GD_Z, 2},
                {kHIDUsage_GD_Rx, 3},
                {kHIDUsage_GD_Ry, 4},
                {kHIDUsage_GD_Rz, 5}
            };

            CFArrayRef elementArray = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);
            CFIndex count = CFArrayGetCount(elementArray);

            for (CFIndex i = 0; i < count; ++i)
            {
                IOHIDElementRef element = static_cast<IOHIDElementRef>(const_cast<void*>(CFArrayGetValueAtIndex(elementArray, i)));
                IOHIDElementType type = IOHIDElementGetType(element);
                uint32_t usagePage = IOHIDElementGetUsagePage(element);
                uint32_t usage = IOHIDElementGetUsage(element);

                if (usage == kHIDUsage_GD_Hatswitch)
                    hatElement = element;

                if (usage >= kHIDUsage_Button_1 && usage < kHIDUsage_Button_1 + 24)
                {
                    Button button;
                    button.button = gamepadConfig.buttonMap[usage - kHIDUsage_Button_1];
                    buttons.insert(std::make_pair(element, button));
                }

                if ((type == kIOHIDElementTypeInput_Misc || type == kIOHIDElementTypeInput_Axis) &&
                    usagePage == kHIDPage_GenericDesktop)
                {
                    auto usageIterator = axisUsageMap.find(usage);

                    if (usageIterator != axisUsageMap.end())
                    {
                        size_t index = usageIterator->second;

                        Axis axis;
                        axis.axis = gamepadConfig.axisMap[index];
                        axis.min = IOHIDElementGetLogicalMin(element);
                        axis.max = IOHIDElementGetLogicalMax(element);
                        axis.range = axis.max - axis.min;

                        switch (gamepadConfig.axisMap[index])
                        {
                            case Gamepad::Axis::NONE:
                                break;
                            case Gamepad::Axis::LEFT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::LEFT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::LEFT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::LEFT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::LEFT_TRIGGER;
                                hasLeftTrigger = true;
                                break;
                            case Gamepad::Axis::RIGHT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::RIGHT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::RIGHT_TRIGGER;
                                hasRightTrigger = true;
                                break;
                        }

                        axes.insert(std::make_pair(element, axis));
                    }
                }
            }

            CFRelease(elementArray);

            IOHIDDeviceRegisterInputValueCallback(device, deviceInput, this);
        }

        void GamepadDeviceIOKit::handleInput(IOHIDValueRef value)
        {
            IOHIDElementRef element = IOHIDValueGetElement(value);
            CFIndex newValue = IOHIDValueGetIntegerValue(value);

            if (element == hatElement)
            {
                uint32_t oldHatValue = static_cast<uint32_t>(hatValue);
                uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                    (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                uint32_t newBitmask = (newValue >= 8) ? 0 : (1 << (newValue / 2)) | // first bit
                (1 << (newValue / 2 + newValue % 2)) % 4; // second bit

                if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                    handleButtonValueChange(Gamepad::Button::DPAD_UP,
                                            (newBitmask & 0x01) > 0,
                                            (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                    handleButtonValueChange(Gamepad::Button::DPAD_RIGHT,
                                            (newBitmask & 0x02) > 0,
                                            (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                    handleButtonValueChange(Gamepad::Button::DPAD_DOWN,
                                            (newBitmask & 0x04) > 0,
                                            (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                    handleButtonValueChange(Gamepad::Button::DPAD_LEFT,
                                            (newBitmask & 0x08) > 0,
                                            (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                hatValue = newValue;
            }

            auto buttonIterator = buttons.find(element);

            if (buttonIterator != buttons.end())
            {
                Button& button = buttonIterator->second;

                if ((button.button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                    (button.button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
                {
                    handleButtonValueChange(button.button, newValue > 0, (newValue > 0) ? 1.0F : 0.0F);
                }

                button.value = newValue;
            }

            auto axisIterator = axes.find(element);

            if (axisIterator != axes.end())
            {
                Axis& axis = axisIterator->second;

                handleAxisChange(axis.value,
                                 newValue,
                                 axis.min, axis.range,
                                 axis.negativeButton, axis.positiveButton);

                axis.value = newValue;
            }
        }

        void GamepadDeviceIOKit::handleAxisChange(CFIndex oldValue, CFIndex newValue,
                                                  CFIndex min, CFIndex range,
                                                  Gamepad::Button negativeButton,
                                                  Gamepad::Button positiveButton)
        {
            if (negativeButton == positiveButton)
            {
                float floatValue = static_cast<float>(newValue - min) / range;

                handleButtonValueChange(negativeButton,
                                        floatValue > 0.0F,
                                        floatValue);
            }
            else
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
    } // namespace input
} // namespace ouzel
