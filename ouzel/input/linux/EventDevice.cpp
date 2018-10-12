// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
#include "InputSystemLinux.hpp"
#include "input/GamepadConfig.hpp"
#include "input/KeyboardDevice.hpp"
#include "input/GamepadDevice.hpp"
#include "input/MouseDevice.hpp"
#include "input/TouchpadDevice.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const uint32_t BITS_PER_LONG = 8 * sizeof(long);
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_LONG)

static inline bool isBitSet(const unsigned long* array, int bit)
{
    return (array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG))) != 0;
}

namespace ouzel
{
    namespace input
    {
        EventDevice::EventDevice(InputSystemLinux& inputSystem, const std::string& initFilename):
            filename(initFilename)
        {
            fd = open(filename.c_str(), O_RDONLY);

            if (fd == -1)
                throw SystemError("Failed to open device file");

            if (ioctl(fd, EVIOCGRAB, 1) == -1)
                Log(Log::Level::WARN) << "Failed to grab device";

            char deviceName[256];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
                Log(Log::Level::WARN) << "Failed to get device name";
            else
            {
                name = deviceName;
                Log(Log::Level::INFO) << "Got device: " << name;
            }

            unsigned long eventBits[BITS_TO_LONGS(EV_CNT)];
            unsigned long absBits[BITS_TO_LONGS(ABS_CNT)];
            unsigned long relBits[BITS_TO_LONGS(REL_CNT)];
            unsigned long keyBits[BITS_TO_LONGS(KEY_CNT)];

            if (ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relBits)), relBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) == -1)
                throw SystemError("Failed to get device event bits");

            if (isBitSet(eventBits, EV_KEY) && (
                    isBitSet(keyBits, KEY_1) ||
                    isBitSet(keyBits, KEY_2) ||
                    isBitSet(keyBits, KEY_3) ||
                    isBitSet(keyBits, KEY_4) ||
                    isBitSet(keyBits, KEY_5) ||
                    isBitSet(keyBits, KEY_6) ||
                    isBitSet(keyBits, KEY_7) ||
                    isBitSet(keyBits, KEY_8) ||
                    isBitSet(keyBits, KEY_9) ||
                    isBitSet(keyBits, KEY_0)
                ))
                keyboardDevice.reset(new KeyboardDevice(inputSystem, inputSystem.getNextDeviceId()));

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if ((isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN)) || // tablet
                    (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN)) || // touchpad
                    isBitSet(keyBits, BTN_TOUCH)) // touchscreen
                {
                    touchpadDevice.reset(new TouchpadDevice(inputSystem, inputSystem.getNextDeviceId()));

                    input_absinfo info;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchSlots.resize(info.maximum + 1);

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &info) == -1)
                        throw SystemError("Failed to get device info");
                    
                    touchMinX = info.minimum;
                    touchMaxX = info.maximum;
                    touchRangeX = touchMaxX - touchMinX;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &info) == -1)
                        throw SystemError("Failed to get device info");

                    touchMinY = info.minimum;
                    touchMaxY = info.maximum;
                    touchRangeY = touchMaxY - touchMinY;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_PRESSURE), &info) != -1)
                    {
                        touchMinPressure = info.minimum;
                        touchMaxPressure = info.maximum;
                    }
                }
                else if (isBitSet(keyBits, BTN_MOUSE)) // mouse
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
                
            }
            else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
            {
                if (isBitSet(keyBits, BTN_MOUSE))
                    mouseDevice.reset(new MouseDevice(inputSystem, inputSystem.getNextDeviceId()));
            }

            if (isBitSet(keyBits, BTN_JOYSTICK) || // joystick
                isBitSet(keyBits, BTN_GAMEPAD)) // gamepad
            {
                gamepadDevice.reset(new GamepadDevice(inputSystem, inputSystem.getNextDeviceId()));

                struct input_id id;
                if (ioctl(fd, EVIOCGID, &id) == -1)
                    throw SystemError("Failed to get device info");

                if (id.vendor == 0x054C && id.product == 0x0268) // Playstation 3 controller
                {
                    buttonMap = PLAYSTATION_3_CONFIG.buttonMap;
                }
                else if (id.vendor == 0x054C && id.product == 0x05C4) // Playstation 4 controller
                {
                    buttonMap = PLAYSTATION_4_CONFIG.buttonMap;
                }
                else if (id.vendor == 0x045E && id.product == 0x02D1) // Xbox One controller
                {
                    buttonMap = XBOX_ONE_CONFIG.buttonMap;
                }
                else if ((id.vendor == 0x0E6F && id.product == 0x0113) || // AfterglowGamepadforXbox360
                    (id.vendor == 0x0E6F && id.product == 0x0213) || // AfterglowGamepadforXbox360
                    (id.vendor == 0x1BAD && id.product == 0xF900) || // AfterglowGamepadforXbox360
                    (id.vendor == 0x0738 && id.product == 0xCB29) || // AviatorforXbox360PC
                    (id.vendor == 0x15E4 && id.product == 0x3F10) || // BatarangwiredcontrollerXBOX
                    (id.vendor == 0x146B && id.product == 0x0601) || // BigbenControllerBB7201
                    (id.vendor == 0x0738 && id.product == 0xF401) || // Controller
                    (id.vendor == 0x0E6F && id.product == 0xF501) || // Controller
                    (id.vendor == 0x1430 && id.product == 0xF801) || // Controller
                    (id.vendor == 0x1BAD && id.product == 0x028E) || // Controller
                    (id.vendor == 0x1BAD && id.product == 0xFA01) || // Controller
                    (id.vendor == 0x12AB && id.product == 0x0004) || // DDRUniverse2Mat
                    (id.vendor == 0x24C6 && id.product == 0x5B00) || // Ferrari458Racingwheel
                    (id.vendor == 0x1430 && id.product == 0x4734) || // GH4Guitar
                    (id.vendor == 0x046D && id.product == 0xC21D) || // GamepadF310
                    (id.vendor == 0x0E6F && id.product == 0x0301) || // GamepadforXbox360
                    (id.vendor == 0x0E6F && id.product == 0x0401) || // GamepadforXbox360Z (GameStop XBox 360 Controller)
                    (id.vendor == 0x12AB && id.product == 0x0302) || // GamepadforXbox360ZZ
                    (id.vendor == 0x1BAD && id.product == 0xF902) || // GamepadforXbox360ZZZ
                    (id.vendor == 0x1BAD && id.product == 0xF901) || // GamestopXbox360Controller
                    (id.vendor == 0x1430 && id.product == 0x474C) || // GuitarHeroforPCMAC
                    (id.vendor == 0x1BAD && id.product == 0xF501) || // HORIPADEX2TURBO
                    (id.vendor == 0x1BAD && id.product == 0x0003) || // HarmonixDrumKitforXbox360
                    (id.vendor == 0x1BAD && id.product == 0x0002) || // HarmonixGuitarforXbox360
                    (id.vendor == 0x0F0D && id.product == 0x000A) || // HoriCoDOA4FightStick
                    (id.vendor == 0x0F0D && id.product == 0x000D) || // HoriFightingStickEx2
                    (id.vendor == 0x0F0D && id.product == 0x0016) || // HoriRealArcadeProEx
                    (id.vendor == 0x24C6 && id.product == 0x5501) || // HoriRealArcadeProVXSA
                    (id.vendor == 0x24C6 && id.product == 0x5506) || // HoriSOULCALIBURVStick
                    (id.vendor == 0x1BAD && id.product == 0xF02D) || // JoytechNeoSe
                    (id.vendor == 0x162E && id.product == 0xBEEF) || // JoytechNeoSeTake2
                    (id.vendor == 0x046D && id.product == 0xC242) || // LogitechChillStream
                    (id.vendor == 0x046D && id.product == 0xC21E) || // LogitechF510
                    (id.vendor == 0x1BAD && id.product == 0xFD01) || // MadCatz360
                    (id.vendor == 0x0738 && id.product == 0x4740) || // MadCatzBeatPad
                    (id.vendor == 0x1BAD && id.product == 0xF025) || // MadCatzCallofDutyGamePad
                    (id.vendor == 0x1BAD && id.product == 0xF027) || // MadCatzFPSProGamePad
                    (id.vendor == 0x1BAD && id.product == 0xF021) || // MadCatzGhostReconFSGamePad
                    (id.vendor == 0x0738 && id.product == 0x4736) || // MadCatzMicroConGamePadPro
                    (id.vendor == 0x1BAD && id.product == 0xF036) || // MadCatzMicroConGamePadProZ
                    (id.vendor == 0x0738 && id.product == 0x9871) || // MadCatzPortableDrumKit
                    (id.vendor == 0x0738 && id.product == 0x4728) || // MadCatzStreetFighterIVFightPad
                    (id.vendor == 0x0738 && id.product == 0x4718) || // MadCatzStreetFighterIVFightStickSE
                    (id.vendor == 0x0738 && id.product == 0x4716) || // MadCatzXbox360Controller
                    (id.vendor == 0x0738 && id.product == 0x4726) || // MadCatzXbox360Controller
                    (id.vendor == 0x0738 && id.product == 0xBEEF) || // MadCatzXbox360Controller
                    (id.vendor == 0x1BAD && id.product == 0xF016) || // MadCatzXbox360Controller
                    (id.vendor == 0x0738 && id.product == 0xB726) || // MadCatzXboxcontrollerMW2
                    (id.vendor == 0x045E && id.product == 0x028E) || // MicrosoftXbox360Controller
                    (id.vendor == 0x045E && id.product == 0x0719) || // MicrosoftXbox360Controller
                    (id.vendor == 0x12AB && id.product == 0x0301) || // PDPAFTERGLOWAX1
                    (id.vendor == 0x0E6F && id.product == 0x0105) || // PDPDancePad
                    (id.vendor == 0x0E6F && id.product == 0x0201) || // PelicanTSZ360Pad
                    (id.vendor == 0x15E4 && id.product == 0x3F00) || // PowerAMiniProElite
                    (id.vendor == 0x24C6 && id.product == 0x5300) || // PowerAMiniProEliteGlow
                    (id.vendor == 0x1BAD && id.product == 0xF504) || // REALARCADEPROEX
                    (id.vendor == 0x1BAD && id.product == 0xF502) || // REALARCADEProVX
                    (id.vendor == 0x1689 && id.product == 0xFD00) || // RazerOnza
                    (id.vendor == 0x1689 && id.product == 0xFD01) || // RazerOnzaTournamentEdition
                    (id.vendor == 0x1430 && id.product == 0x4748) || // RedOctaneGuitarHeroXplorer
                    (id.vendor == 0x0E6F && id.product == 0x011F) || // RockCandyGamepadforXbox360
                    (id.vendor == 0x12AB && id.product == 0x0006) || // RockRevolutionforXbox360
                    (id.vendor == 0x0738 && id.product == 0xCB02) || // SaitekCyborgRumblePadPCXbox360
                    (id.vendor == 0x0738 && id.product == 0xCB03) || // SaitekP3200RumblePadPCXbox360
                    (id.vendor == 0x1BAD && id.product == 0xF028) || // StreetFighterIVFightPad
                    (id.vendor == 0x0738 && id.product == 0x4738) || // StreetFighterIVFightStickTE
                    (id.vendor == 0x0738 && id.product == 0xF738) || // SuperSFIVFightStickTES
                    (id.vendor == 0x1BAD && id.product == 0xF903) || // TronXbox360controller
                    (id.vendor == 0x1BAD && id.product == 0x5500) || // USBGamepad
                    (id.vendor == 0x1BAD && id.product == 0xF906) || // XB360MortalKombatFightStick
                    (id.vendor == 0x15E4 && id.product == 0x3F0A)) // XboxAirflowiredcontroller
                {
                    buttonMap = XBOX_360_CONFIG.buttonMap;
                }
                else if (id.vendor == 0x0079 && id.product == 0x0006) // Acme GA07
                {
                    buttonMap = ACME_GA07_CONFIG.buttonMap;
                }
                else // Generic (based on Logitech RumblePad 2)
                {
                    buttonMap = GENERIC_CONFIG.buttonMap;
                }
            }
        }

        EventDevice::~EventDevice()
        {
            if (fd != -1)
            {
                if (ioctl(fd, EVIOCGRAB, 0) == -1)
                    Log(Log::Level::WARN) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            input_event events[32];
            ssize_t bytesRead = read(fd, events, sizeof(events));

            if (bytesRead == -1)
                throw SystemError("Failed to read from " + filename); // TODO: disconnect the device

            int count = bytesRead / sizeof(input_event);

            for (int i = 0; i < count; ++i)
            {
                input_event& event = events[i];

                if (keyboardDevice)
                {
                    switch (event.type)
                    {
                        case EV_KEY:
                            if (event.value == 1 || event.value == 2) // press or repeat
                                keyboardDevice->handleKeyPress(InputSystemLinux::convertKeyCode(event.code));
                            else if (event.value == 0) // release
                                keyboardDevice->handleKeyRelease(InputSystemLinux::convertKeyCode(event.code));
                            break;
                    }
                }
                if (mouseDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_X:
                                    cursorPosition.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event.value), 0.0F)).x;
                                    break;
                                case ABS_Y:
                                    cursorPosition.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event.value))).y;
                                    break;
                            }

                            mouseDevice->handleMove(cursorPosition);
                            break;
                        }
                        case EV_REL:
                        {
                            Vector2 relativePos;

                            switch (event.code)
                            {
                                case REL_X:
                                    relativePos.x = static_cast<float>(event.value);
                                    break;
                                case REL_Y:
                                    relativePos.y = static_cast<float>(event.value);
                                    break;
                                case REL_WHEEL:
                                    mouseDevice->handleScroll(Vector2(0.0F, static_cast<float>(event.value)), cursorPosition);
                                    break;
                                case REL_HWHEEL:
                                    mouseDevice->handleScroll(Vector2(static_cast<float>(event.value), 0.0F), cursorPosition);
                                    break;
                            }

                            mouseDevice->handleRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos));
                            break;
                        }
                        case EV_KEY:
                        {
                            Mouse::Button button = Mouse::Button::NONE;

                            switch (event.code)
                            {
                            case BTN_LEFT:
                                button = Mouse::Button::LEFT;
                                break;
                            case BTN_RIGHT:
                                button = Mouse::Button::RIGHT;
                                break;
                            case BTN_MIDDLE:
                                button = Mouse::Button::MIDDLE;
                                break;
                            }

                            if (event.value == 1)
                                mouseDevice->handleButtonPress(button, cursorPosition);
                            else if (event.value == 0)
                                mouseDevice->handleButtonRelease(button, cursorPosition);
                            break;
                        }
                    }
                }
                if (touchpadDevice)
                {
                    switch (event.type)
                    {
                        case EV_ABS:
                        {
                            switch (event.code)
                            {
                                case ABS_MT_SLOT:
                                {
                                    currentTouchSlot = event.value;
                                    break;
                                }
                                case ABS_MT_TRACKING_ID:
                                {
                                    if (event.value >= 0)
                                    {
                                        touchSlots[currentTouchSlot].trackingId = event.value;
                                        touchSlots[currentTouchSlot].action = Slot::Action::BEGIN;
                                    }
                                    else
                                        touchSlots[currentTouchSlot].action = Slot::Action::END;
                                    break;
                                }
                                case ABS_MT_POSITION_X:
                                {
                                    touchSlots[currentTouchSlot].positionX = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                                case ABS_MT_POSITION_Y:
                                {
                                    touchSlots[currentTouchSlot].positionY = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::MOVE;
                                    break;
                                }
                                case ABS_MT_PRESSURE:
                                {
                                    touchSlots[currentTouchSlot].pressure = event.value;
                                    break;
                                }
                            }
                            break;
                        }
                        case EV_SYN:
                        {
                            switch (event.code)
                            {
                                case SYN_REPORT:
                                {
                                    for (Slot& slot : touchSlots)
                                    {
                                        if (slot.action != Slot::Action::NONE)
                                        {
                                            Vector2 position(static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
                                                             static_cast<float>(slot.positionY - touchMinY) / touchRangeY);
                                            float pressure = static_cast<float>(slot.pressure - touchMinPressure) / touchMaxPressure;

                                            switch (slot.action)
                                            {
                                                case Slot::Action::NONE:
                                                    break;
                                                case Slot::Action::BEGIN:
                                                    touchpadDevice->handleTouchBegin(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::END:
                                                    touchpadDevice->handleTouchEnd(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::MOVE:
                                                    touchpadDevice->handleTouchMove(static_cast<uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                            }

                                            slot.action = Slot::Action::NONE;
                                        }
                                    }
                                    break;
                                }
                                case SYN_DROPPED:
                                {
                                    struct input_mt_request_layout
                                    {
                                        __u32 code;
                                        __s32 values[1];
                                    };

                                    size_t size = sizeof(__u32) + sizeof(__s32) * touchSlots.size();
                                    std::vector<uint8_t> data(size);

                                    input_mt_request_layout* request = reinterpret_cast<input_mt_request_layout*>(data.data());

                                    request->code = ABS_MT_TRACKING_ID;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId < 0 &&
                                            request->values[i] >= 0)
                                        {
                                            touchSlots[i].trackingId = request->values[i];
                                            touchSlots[i].action = Slot::Action::BEGIN;
                                        }
                                        else if (touchSlots[i].trackingId >= 0 &&
                                                 request->values[i] < 0)
                                        {
                                            touchSlots[i].trackingId = request->values[i];
                                            touchSlots[i].action = Slot::Action::END;
                                        }
                                    }

                                    request->code = ABS_MT_POSITION_X;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId >= 0 &&
                                            touchSlots[i].positionX != request->values[i])
                                        {
                                            touchSlots[i].positionX = request->values[i];
                                            if (touchSlots[i].action == Slot::Action::NONE)
                                                touchSlots[i].action = Slot::Action::MOVE;
                                        }
                                    }
                                    
                                    request->code = ABS_MT_POSITION_Y;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request) == -1)
                                        throw SystemError("Failed to get device info");

                                    for (size_t i = 0; i < touchSlots.size(); ++i)
                                    {
                                        if (touchSlots[i].trackingId >= 0 &&
                                            touchSlots[i].positionY != request->values[i])
                                        {
                                            touchSlots[i].positionY = request->values[i];
                                            if (touchSlots[i].action == Slot::Action::NONE)
                                                touchSlots[i].action = Slot::Action::MOVE;
                                        }
                                    }

                                    request->code = ABS_MT_PRESSURE;
                                    if (ioctl(fd, EVIOCGABS(size), request) != -1)
                                    {
                                        for (size_t i = 0; i < touchSlots.size(); ++i)
                                        {
                                            if (touchSlots[i].trackingId >= 0 &&
                                                touchSlots[i].pressure != request->values[i])
                                            {
                                                touchSlots[i].pressure = request->values[i];
                                                if (touchSlots[i].action == Slot::Action::NONE)
                                                    touchSlots[i].action = Slot::Action::MOVE;
                                            }
                                        }
                                    }

                                    input_absinfo info;
                                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                                        throw SystemError("Failed to get device info");
                                    currentTouchSlot = info.value;

                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                if (gamepadDevice)
                {
                    switch(event.type)
                    {
                        case EV_ABS:
                        {
                            // TODO: implement
                            if (event.code == ABS_HAT0X)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DPAD_RIGHT : Gamepad::Button::DPAD_LEFT, true, 1.0F);
                                else if (hat0XValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0XValue > 0) ? Gamepad::Button::DPAD_RIGHT : Gamepad::Button::DPAD_LEFT, false, 0.0F);

                                hat0XValue = event.value;
                            }
                            else if (event.code == ABS_HAT0Y)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DPAD_DOWN : Gamepad::Button::DPAD_UP, true, 1.0F);
                                else if (hat0YValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0YValue > 0) ? Gamepad::Button::DPAD_DOWN : Gamepad::Button::DPAD_UP, false, 0.0F);

                                hat0YValue = event.value;
                            }

                            // TODO: implement ABS_X, ABS_Y, ABS_Z, ABS_RX, ABS_RY, ABS_RZ
                            break;
                        }
                        case EV_KEY:
                        {
                            Gamepad::Button button = buttonMap[event.code - BTN_GAMEPAD];

                            if (button != Gamepad::Button::NONE)
                                gamepadDevice->handleButtonValueChange(button,
                                                                       event.value > 0,
                                                                       event.value > 0) ? 1.0F : 0.0F);
                            break;
                        }
                    }
                }
            }
        }
    } // namespace input
} // namespace ouzel
