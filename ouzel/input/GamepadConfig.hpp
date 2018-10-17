// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <array>
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        struct GamepadConfig
        {
            std::array<Gamepad::Button, 24> buttonMap;
            std::array<Gamepad::Axis, 6> axisMap; // X, Y, Z, RX, RY, RZ
        };

        // Playstation 3 controller
        // vendorId = 0x054C, productId = 0x0268
        static const GamepadConfig PLAYSTATION_3_CONFIG = {
            {
                Gamepad::Button::BACK, // Select
                Gamepad::Button::LEFT_THUMB, // L3
                Gamepad::Button::RIGHT_THUMB, // R3
                Gamepad::Button::START, // Start
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::LEFT_TRIGGER, // L2
                Gamepad::Button::RIGHT_TRIGGER, // R2
                Gamepad::Button::LEFT_SHOULDER, // L1
                Gamepad::Button::RIGHT_SHOULDER, // R1
                Gamepad::Button::FACE_TOP, // Triangle
                Gamepad::Button::FACE_RIGHT, // Circle
                Gamepad::Button::FACE_BOTTOM, // Cross
                Gamepad::Button::FACE_LEFT, // Square
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Playstation 4 controller
        // vendorId = 0x054C, productId = 0x05C4
        static const GamepadConfig PLAYSTATION_4_CONFIG = {
            {
                Gamepad::Button::FACE_LEFT, // Square
                Gamepad::Button::FACE_BOTTOM, // Cross
                Gamepad::Button::FACE_RIGHT, // Circle
                Gamepad::Button::FACE_TOP, // Triangle
                Gamepad::Button::LEFT_SHOULDER, // L1
                Gamepad::Button::RIGHT_SHOULDER, // R1
                Gamepad::Button::LEFT_TRIGGER, // L2
                Gamepad::Button::RIGHT_TRIGGER, // R2
                Gamepad::Button::BACK, // Share
                Gamepad::Button::START, // Options
                Gamepad::Button::LEFT_THUMB, // L3
                Gamepad::Button::RIGHT_THUMB, // R3
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Xbox One controller
        // vendorId = 0x045E, productId = 0x02D1
        static const GamepadConfig XBOX_ONE_CONFIG = {
            {
                Gamepad::Button::FACE_BOTTOM, // A
                Gamepad::Button::FACE_RIGHT, // B
                Gamepad::Button::FACE_LEFT, // X
                Gamepad::Button::FACE_TOP, // Y
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::BACK, // Menu
                Gamepad::Button::START, // View
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::RIGHT_THUMB_Y,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER
            }
        };

        // Xbox 360 controller
        static const GamepadConfig XBOX_360_CONFIG = {
            {
                Gamepad::Button::FACE_BOTTOM, // A
                Gamepad::Button::FACE_RIGHT, // B
                Gamepad::Button::FACE_LEFT, // X
                Gamepad::Button::FACE_TOP, // Y
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::START,
                Gamepad::Button::BACK,
                Gamepad::Button::DPAD_UP,
                Gamepad::Button::DPAD_DOWN,
                Gamepad::Button::DPAD_LEFT,
                Gamepad::Button::DPAD_RIGHT,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::RIGHT_THUMB_Y,
                Gamepad::Axis::RIGHT_TRIGGER
            }
        };

        // Acme GA07 controller
        // vendorId = 0x0079, productId = 0x0006
        static const GamepadConfig ACME_GA07_CONFIG = {
            {
                Gamepad::Button::FACE_TOP, // 1
                Gamepad::Button::FACE_RIGHT, // 2
                Gamepad::Button::FACE_BOTTOM, // 3
                Gamepad::Button::FACE_LEFT, // 4
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_TRIGGER,
                Gamepad::Button::RIGHT_TRIGGER,
                Gamepad::Button::BACK,
                Gamepad::Button::START,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::NONE,
                Gamepad::Axis::NONE,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        // Generic controller
        static const GamepadConfig GENERIC_CONFIG = {
            {
                Gamepad::Button::FACE_LEFT,
                Gamepad::Button::FACE_BOTTOM,
                Gamepad::Button::FACE_RIGHT,
                Gamepad::Button::FACE_TOP,
                Gamepad::Button::LEFT_SHOULDER,
                Gamepad::Button::RIGHT_SHOULDER,
                Gamepad::Button::LEFT_TRIGGER,
                Gamepad::Button::RIGHT_TRIGGER,
                Gamepad::Button::BACK,
                Gamepad::Button::START,
                Gamepad::Button::LEFT_THUMB,
                Gamepad::Button::RIGHT_THUMB,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE,
                Gamepad::Button::NONE
            },
            {
                Gamepad::Axis::LEFT_THUMB_X,
                Gamepad::Axis::LEFT_THUMB_Y,
                Gamepad::Axis::RIGHT_THUMB_X,
                Gamepad::Axis::LEFT_TRIGGER,
                Gamepad::Axis::RIGHT_TRIGGER,
                Gamepad::Axis::RIGHT_THUMB_Y
            }
        };

        inline const GamepadConfig& getGamepadConfig(int32_t vendorId, int32_t productId)
        {
            if (vendorId == 0x054C && (productId == 0x0268 || productId == 0x02EA)) // Sony Playstation 3 controller
                return PLAYSTATION_3_CONFIG;
            else if (vendorId == 0x054C && (productId == 0x05C4 || productId == 0x09CC)) // Sony Playstation 4 controller
                return PLAYSTATION_4_CONFIG;
            else if (vendorId == 0x045E && (productId == 0x02D1 || // Microsoft Xbox One controller
                                            productId == 0x02DD || // Microsoft Xbox One Controller (Firmware 2015)
                                            productId == 0x02E3 || // Microsoft Xbox One Elite Controller
                                            productId == 0x02EA || // Microsoft Xbox One S Controller
                                            productId == 0x02FD)) // Microsoft Xbox One S Controller [Bluetooth]
                return XBOX_ONE_CONFIG;
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
                return XBOX_360_CONFIG;
            else if (vendorId == 0x0079 && productId == 0x0006) // Acme GA07
                return ACME_GA07_CONFIG;
            else // Generic (based on Logitech RumblePad 2)
                return GENERIC_CONFIG;
        }
    }
}
