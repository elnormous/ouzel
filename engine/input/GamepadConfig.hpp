// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_GAMEPADCONFIG_HPP
#define OUZEL_INPUT_GAMEPADCONFIG_HPP

#include <array>
#include "Gamepad.hpp"

namespace ouzel::input
{
    struct GamepadConfig final
    {
        const std::array<Gamepad::Button, 24> buttonMap;
        const std::array<Gamepad::Axis, 6> axisMap; // X, Y, Z, RX, RY, RZ
    };

    // Playstation 3 controller
    constexpr GamepadConfig playstation3Config = {
        {
            Gamepad::Button::back, // Select
            Gamepad::Button::leftThumb, // L3
            Gamepad::Button::rightThumb, // R3
            Gamepad::Button::start, // Start
            Gamepad::Button::dPadUp,
            Gamepad::Button::dPadRight,
            Gamepad::Button::dPadDown,
            Gamepad::Button::dPadLeft,
            Gamepad::Button::leftTrigger, // L2
            Gamepad::Button::rightTrigger, // R2
            Gamepad::Button::leftShoulder, // L1
            Gamepad::Button::rightShoulder, // R1
            Gamepad::Button::faceTop, // Triangle
            Gamepad::Button::faceRight, // Circle
            Gamepad::Button::faceBottom, // Cross
            Gamepad::Button::faceLeft, // Square
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::leftTrigger,
            Gamepad::Axis::rightTrigger,
            Gamepad::Axis::rightThumbY
        }
    };

    // Playstation 4 controller
    constexpr GamepadConfig playstation4Config = {
        {
            Gamepad::Button::faceLeft, // Square
            Gamepad::Button::faceBottom, // Cross
            Gamepad::Button::faceRight, // Circle
            Gamepad::Button::faceTop, // Triangle
            Gamepad::Button::leftShoulder, // L1
            Gamepad::Button::rightShoulder, // R1
            Gamepad::Button::leftTrigger, // L2
            Gamepad::Button::rightTrigger, // R2
            Gamepad::Button::back, // Share
            Gamepad::Button::start, // Options
            Gamepad::Button::leftThumb, // L3
            Gamepad::Button::rightThumb, // R3
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::leftTrigger,
            Gamepad::Axis::rightTrigger,
            Gamepad::Axis::rightThumbY
        }
    };

    // Xbox One controller
    constexpr GamepadConfig xboxOneConfig = {
        {
            Gamepad::Button::faceBottom, // A
            Gamepad::Button::faceRight, // B
            Gamepad::Button::faceLeft, // X
            Gamepad::Button::faceTop, // Y
            Gamepad::Button::leftShoulder,
            Gamepad::Button::rightShoulder,
            Gamepad::Button::leftThumb,
            Gamepad::Button::rightThumb,
            Gamepad::Button::back, // Menu
            Gamepad::Button::start, // View
            Gamepad::Button::dPadUp,
            Gamepad::Button::dPadDown,
            Gamepad::Button::dPadLeft,
            Gamepad::Button::dPadRight,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::rightThumbY,
            Gamepad::Axis::leftTrigger,
            Gamepad::Axis::rightTrigger
        }
    };

    // Xbox 360 controller
    constexpr GamepadConfig xbox360Config = {
        {
            Gamepad::Button::faceBottom, // A
            Gamepad::Button::faceRight, // B
            Gamepad::Button::faceLeft, // X
            Gamepad::Button::faceTop, // Y
            Gamepad::Button::leftShoulder,
            Gamepad::Button::rightShoulder,
            Gamepad::Button::leftThumb,
            Gamepad::Button::rightThumb,
            Gamepad::Button::start,
            Gamepad::Button::back,
            Gamepad::Button::dPadUp,
            Gamepad::Button::dPadDown,
            Gamepad::Button::dPadLeft,
            Gamepad::Button::dPadRight,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::leftTrigger,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::rightThumbY,
            Gamepad::Axis::rightTrigger
        }
    };

    // Acme GA07 controller
    constexpr GamepadConfig acmeGa07Config = {
        {
            Gamepad::Button::faceTop, // 1
            Gamepad::Button::faceRight, // 2
            Gamepad::Button::faceBottom, // 3
            Gamepad::Button::faceLeft, // 4
            Gamepad::Button::leftShoulder,
            Gamepad::Button::rightShoulder,
            Gamepad::Button::leftTrigger,
            Gamepad::Button::rightTrigger,
            Gamepad::Button::back,
            Gamepad::Button::start,
            Gamepad::Button::leftThumb,
            Gamepad::Button::rightThumb,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::none,
            Gamepad::Axis::none,
            Gamepad::Axis::rightThumbY
        }
    };

    // Generic controller
    constexpr GamepadConfig genericConfig = {
        {
            Gamepad::Button::faceLeft,
            Gamepad::Button::faceBottom,
            Gamepad::Button::faceRight,
            Gamepad::Button::faceTop,
            Gamepad::Button::leftShoulder,
            Gamepad::Button::rightShoulder,
            Gamepad::Button::leftTrigger,
            Gamepad::Button::rightTrigger,
            Gamepad::Button::back,
            Gamepad::Button::start,
            Gamepad::Button::leftThumb,
            Gamepad::Button::rightThumb,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none,
            Gamepad::Button::none
        },
        {
            Gamepad::Axis::leftThumbX,
            Gamepad::Axis::leftThumbY,
            Gamepad::Axis::rightThumbX,
            Gamepad::Axis::leftTrigger,
            Gamepad::Axis::rightTrigger,
            Gamepad::Axis::rightThumbY
        }
    };

    inline auto& getGamepadConfig(std::int32_t vendorId, std::int32_t productId)
    {
        if (vendorId == 0x054C && (productId == 0x0268 || productId == 0x02EA)) // Sony Playstation 3 controller
            return playstation3Config;
        else if (vendorId == 0x054C && (productId == 0x05C4 || productId == 0x09CC)) // Sony Playstation 4 controller
            return playstation4Config;
        else if (vendorId == 0x045E && (productId == 0x02D1 || // Microsoft Xbox One controller
                                        productId == 0x02DD || // Microsoft Xbox One Controller (Firmware 2015)
                                        productId == 0x02E3 || // Microsoft Xbox One Elite Controller
                                        productId == 0x02EA || // Microsoft Xbox One S Controller
                                        productId == 0x02FD)) // Microsoft Xbox One S Controller [Bluetooth]
            return xboxOneConfig;
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
                 (vendorId == 0x045E && productId == 0x028E) || // Microsoft Xbox360 Controller
                 (vendorId == 0x045E && productId == 0x028F) || // Microsoft Xbox360 Wireless Controller
                 (vendorId == 0x045E && productId == 0x0719) || // Microsoft Xbox 360 Wireless Adapter
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
            return xbox360Config;
        else if (vendorId == 0x0079 && productId == 0x0006) // Acme GA07
            return acmeGa07Config;
        else // Generic (based on Logitech RumblePad 2)
            return genericConfig;
    }
}

#endif // OUZEL_INPUT_GAMEPADCONFIG_HPP
