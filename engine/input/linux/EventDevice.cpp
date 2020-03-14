// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <system_error>
#include <unordered_map>
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
#include "utils/Log.hpp"

namespace
{
    constexpr float THUMB_DEADZONE = 0.2F;

    constexpr std::uint32_t BITS_PER_LONG = 8 * sizeof(long);

    constexpr std::size_t bitsToLongs(std::size_t n)
    {
        return (n + BITS_PER_LONG - 1) / BITS_PER_LONG; // rounded up
    }

    inline auto isBitSet(const unsigned long* array, int bit)
    {
        return (array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG))) != 0;
    }

    ouzel::input::Keyboard::Key convertKeyCode(std::uint16_t keyCode)
    {
        switch (keyCode)
        {
            case KEY_ESC: return ouzel::input::Keyboard::Key::Escape;
            case KEY_1: return ouzel::input::Keyboard::Key::Num1;
            case KEY_2: return ouzel::input::Keyboard::Key::Num2;
            case KEY_3: return ouzel::input::Keyboard::Key::Num3;
            case KEY_4: return ouzel::input::Keyboard::Key::Num4;
            case KEY_5: return ouzel::input::Keyboard::Key::Num5;
            case KEY_6: return ouzel::input::Keyboard::Key::Num6;
            case KEY_7: return ouzel::input::Keyboard::Key::Num7;
            case KEY_8: return ouzel::input::Keyboard::Key::Num8;
            case KEY_9: return ouzel::input::Keyboard::Key::Num9;
            case KEY_0: return ouzel::input::Keyboard::Key::Num0;
            case KEY_MINUS: return ouzel::input::Keyboard::Key::Minus;
            case KEY_EQUAL: return ouzel::input::Keyboard::Key::Equal;
            case KEY_BACKSPACE: return ouzel::input::Keyboard::Key::Backspace;
            case KEY_TAB: return ouzel::input::Keyboard::Key::Tab;
            case KEY_Q: return ouzel::input::Keyboard::Key::Q;
            case KEY_W: return ouzel::input::Keyboard::Key::W;
            case KEY_E: return ouzel::input::Keyboard::Key::E;
            case KEY_R: return ouzel::input::Keyboard::Key::R;
            case KEY_T: return ouzel::input::Keyboard::Key::T;
            case KEY_Y: return ouzel::input::Keyboard::Key::Y;
            case KEY_U: return ouzel::input::Keyboard::Key::U;
            case KEY_I: return ouzel::input::Keyboard::Key::I;
            case KEY_O: return ouzel::input::Keyboard::Key::O;
            case KEY_P: return ouzel::input::Keyboard::Key::P;
            case KEY_LEFTBRACE: return ouzel::input::Keyboard::Key::LeftBracket;
            case KEY_RIGHTBRACE: return ouzel::input::Keyboard::Key::RightBracket;
            case KEY_ENTER: return ouzel::input::Keyboard::Key::Enter;
            case KEY_LEFTCTRL: return ouzel::input::Keyboard::Key::LeftControl;
            case KEY_A: return ouzel::input::Keyboard::Key::A;
            case KEY_S: return ouzel::input::Keyboard::Key::S;
            case KEY_D: return ouzel::input::Keyboard::Key::D;
            case KEY_F: return ouzel::input::Keyboard::Key::F;
            case KEY_G: return ouzel::input::Keyboard::Key::G;
            case KEY_H: return ouzel::input::Keyboard::Key::H;
            case KEY_J: return ouzel::input::Keyboard::Key::J;
            case KEY_K: return ouzel::input::Keyboard::Key::K;
            case KEY_L: return ouzel::input::Keyboard::Key::L;
            case KEY_SEMICOLON: return ouzel::input::Keyboard::Key::Semicolon;
            case KEY_APOSTROPHE: return ouzel::input::Keyboard::Key::Quote;
            case KEY_GRAVE: return ouzel::input::Keyboard::Key::Grave;
            case KEY_LEFTSHIFT: return ouzel::input::Keyboard::Key::LeftShift;
            case KEY_BACKSLASH: return ouzel::input::Keyboard::Key::Backslash;
            case KEY_Z: return ouzel::input::Keyboard::Key::Z;
            case KEY_X: return ouzel::input::Keyboard::Key::X;
            case KEY_C: return ouzel::input::Keyboard::Key::C;
            case KEY_V: return ouzel::input::Keyboard::Key::V;
            case KEY_B: return ouzel::input::Keyboard::Key::B;
            case KEY_N: return ouzel::input::Keyboard::Key::N;
            case KEY_M: return ouzel::input::Keyboard::Key::M;
            case KEY_COMMA: return ouzel::input::Keyboard::Key::Comma;
            case KEY_DOT: return ouzel::input::Keyboard::Key::Period;
            case KEY_SLASH: return ouzel::input::Keyboard::Key::Slash;
            case KEY_RIGHTSHIFT: return ouzel::input::Keyboard::Key::RightShift;
            case KEY_KPASTERISK: return ouzel::input::Keyboard::Key::NumpadMultiply;
            case KEY_LEFTALT: return ouzel::input::Keyboard::Key::LeftAlt;
            case KEY_SPACE: return ouzel::input::Keyboard::Key::Space;
            case KEY_CAPSLOCK: return ouzel::input::Keyboard::Key::CapsLock;
            case KEY_F1: return ouzel::input::Keyboard::Key::F1;
            case KEY_F2: return ouzel::input::Keyboard::Key::F2;
            case KEY_F3: return ouzel::input::Keyboard::Key::F3;
            case KEY_F4: return ouzel::input::Keyboard::Key::F4;
            case KEY_F5: return ouzel::input::Keyboard::Key::F5;
            case KEY_F6: return ouzel::input::Keyboard::Key::F6;
            case KEY_F7: return ouzel::input::Keyboard::Key::F7;
            case KEY_F8: return ouzel::input::Keyboard::Key::F8;
            case KEY_F9: return ouzel::input::Keyboard::Key::F9;
            case KEY_F10: return ouzel::input::Keyboard::Key::F10;
            case KEY_NUMLOCK: return ouzel::input::Keyboard::Key::NumLock;
            case KEY_SCROLLLOCK: return ouzel::input::Keyboard::Key::ScrollLock;
            case KEY_KP7: return ouzel::input::Keyboard::Key::Numpad7;
            case KEY_KP8: return ouzel::input::Keyboard::Key::Numpad8;
            case KEY_KP9: return ouzel::input::Keyboard::Key::Numpad9;
            case KEY_KPMINUS: return ouzel::input::Keyboard::Key::NumpadMinus;
            case KEY_KP4: return ouzel::input::Keyboard::Key::Numpad4;
            case KEY_KP5: return ouzel::input::Keyboard::Key::Numpad5;
            case KEY_KP6: return ouzel::input::Keyboard::Key::Numpad6;
            case KEY_KPPLUS: return ouzel::input::Keyboard::Key::NumpadPlus;
            case KEY_KP1: return ouzel::input::Keyboard::Key::Numpad1;
            case KEY_KP2: return ouzel::input::Keyboard::Key::Numpad2;
            case KEY_KP3: return ouzel::input::Keyboard::Key::Numpad3;
            case KEY_KP0: return ouzel::input::Keyboard::Key::Numpad0;
            case KEY_KPDOT: return ouzel::input::Keyboard::Key::NumpadDecimal;

            case KEY_ZENKAKUHANKAKU: return ouzel::input::Keyboard::Key::Unknown; // ??
            case KEY_102ND: return ouzel::input::Keyboard::Key::IntlBackslash;
            case KEY_F11: return ouzel::input::Keyboard::Key::F11;
            case KEY_F12: return ouzel::input::Keyboard::Key::F12;
            case KEY_RO: return ouzel::input::Keyboard::Key::Ro;
            case KEY_KATAKANA: return ouzel::input::Keyboard::Key::Katakana;
            case KEY_HIRAGANA: return ouzel::input::Keyboard::Key::Hiragana;
            case KEY_HENKAN: return ouzel::input::Keyboard::Key::Henkan;
            case KEY_KATAKANAHIRAGANA: return ouzel::input::Keyboard::Key::KatakanaHiragana;
            case KEY_MUHENKAN: return ouzel::input::Keyboard::Key::Muhenkan;
            case KEY_KPJPCOMMA: return ouzel::input::Keyboard::Key::NumpadJpcomma;
            case KEY_KPENTER: return ouzel::input::Keyboard::Key::NumpadEnter;
            case KEY_RIGHTCTRL: return ouzel::input::Keyboard::Key::RightControl;
            case KEY_KPSLASH: return ouzel::input::Keyboard::Key::NumpadDivide;
            case KEY_SYSRQ: return ouzel::input::Keyboard::Key::PrintScreen;
            case KEY_RIGHTALT: return ouzel::input::Keyboard::Key::RightAlt;
            case KEY_LINEFEED: return ouzel::input::Keyboard::Key::Unknown; // ??
            case KEY_HOME: return ouzel::input::Keyboard::Key::Home;
            case KEY_UP: return ouzel::input::Keyboard::Key::Up;
            case KEY_PAGEUP: return ouzel::input::Keyboard::Key::PageUp;
            case KEY_LEFT: return ouzel::input::Keyboard::Key::Left;
            case KEY_RIGHT: return ouzel::input::Keyboard::Key::Right;
            case KEY_END: return ouzel::input::Keyboard::Key::End;
            case KEY_DOWN: return ouzel::input::Keyboard::Key::Down;
            case KEY_PAGEDOWN: return ouzel::input::Keyboard::Key::PageDown;
            case KEY_INSERT: return ouzel::input::Keyboard::Key::Insert;
            case KEY_DELETE: return ouzel::input::Keyboard::Key::Del;
            case KEY_MACRO: return ouzel::input::Keyboard::Key::Unknown; // ??
            case KEY_MAIL: return ouzel::input::Keyboard::Key::Mail;
            case KEY_MUTE: return ouzel::input::Keyboard::Key::Mute;
            case KEY_VOLUMEDOWN: return ouzel::input::Keyboard::Key::VolumeDown;
            case KEY_VOLUMEUP: return ouzel::input::Keyboard::Key::VolumeUp;
            case KEY_NEXTSONG: return ouzel::input::Keyboard::Key::AudioNext;
            case KEY_PLAYPAUSE: return ouzel::input::Keyboard::Key::AudioPlay;
            case KEY_PREVIOUSSONG: return ouzel::input::Keyboard::Key::AudioPrevious;
            case KEY_STOPCD: return ouzel::input::Keyboard::Key::AudioStop;

            case KEY_POWER: return ouzel::input::Keyboard::Key::Power;
            case KEY_KPEQUAL: return ouzel::input::Keyboard::Key::NumpadEqual;
            case KEY_KPPLUSMINUS: return ouzel::input::Keyboard::Key::NumpadPlusminus;
            case KEY_PAUSE: return ouzel::input::Keyboard::Key::Pause;
            case KEY_SCALE: return ouzel::input::Keyboard::Key::Unknown; //?

            case KEY_KPCOMMA: return ouzel::input::Keyboard::Key::NumpadSeparator;
            case KEY_HANGEUL: return ouzel::input::Keyboard::Key::Kana;
            case KEY_HANJA: return ouzel::input::Keyboard::Key::Hanja;
            case KEY_YEN: return ouzel::input::Keyboard::Key::Yen;
            case KEY_LEFTMETA: return ouzel::input::Keyboard::Key::LeftSuper;
            case KEY_RIGHTMETA: return ouzel::input::Keyboard::Key::RightSuper;
            case KEY_COMPOSE: return ouzel::input::Keyboard::Key::Unknown; // ??

            case KEY_F13: return ouzel::input::Keyboard::Key::F13;
            case KEY_F14: return ouzel::input::Keyboard::Key::F14;
            case KEY_F15: return ouzel::input::Keyboard::Key::F15;
            case KEY_F16: return ouzel::input::Keyboard::Key::F16;
            case KEY_F17: return ouzel::input::Keyboard::Key::F17;
            case KEY_F18: return ouzel::input::Keyboard::Key::F18;
            case KEY_F19: return ouzel::input::Keyboard::Key::F19;
            case KEY_F20: return ouzel::input::Keyboard::Key::F20;
            case KEY_F21: return ouzel::input::Keyboard::Key::F21;
            case KEY_F22: return ouzel::input::Keyboard::Key::F22;
            case KEY_F23: return ouzel::input::Keyboard::Key::F23;
            case KEY_F24: return ouzel::input::Keyboard::Key::F24;

            case KEY_MENU: return ouzel::input::Keyboard::Key::Menu;
            case KEY_CLEAR: return ouzel::input::Keyboard::Key::Clear;
            case KEY_SLEEP: return ouzel::input::Keyboard::Key::Sleep;
            case KEY_PROG1: return ouzel::input::Keyboard::Key::App1;
            case KEY_PROG2: return ouzel::input::Keyboard::Key::App2;
            case KEY_HELP: return ouzel::input::Keyboard::Key::Help;

            case KEY_BACK: return ouzel::input::Keyboard::Key::Back;
            case KEY_FORWARD: return ouzel::input::Keyboard::Key::Forward;
            case KEY_REFRESH: return ouzel::input::Keyboard::Key::Refresh;
            case KEY_STOP: return ouzel::input::Keyboard::Key::Stop;
            case KEY_FIND: return ouzel::input::Keyboard::Key::Search;
            case KEY_BOOKMARKS: return ouzel::input::Keyboard::Key::Bookmarks;
            case KEY_HOMEPAGE: return ouzel::input::Keyboard::Key::Homepage;
            case KEY_MODE: return ouzel::input::Keyboard::Key::ModeChange;
            default: return ouzel::input::Keyboard::Key::Unknown;
        }
    }

    ouzel::input::Mouse::Button convertButtonCode(std::uint16_t buttonCode)
    {
        switch (buttonCode)
        {
            case BTN_LEFT: return ouzel::input::Mouse::Button::Left;
            case BTN_RIGHT: return ouzel::input::Mouse::Button::Right;
            case BTN_MIDDLE: return ouzel::input::Mouse::Button::Middle;
            case BTN_SIDE: return ouzel::input::Mouse::Button::X1;
            case BTN_EXTRA: return ouzel::input::Mouse::Button::X2;
            case BTN_FORWARD: return ouzel::input::Mouse::Button::Unknown; //?
            case BTN_BACK: return ouzel::input::Mouse::Button::Unknown; //?
            case BTN_TASK: return ouzel::input::Mouse::Button::Unknown; //?
            default: return ouzel::input::Mouse::Button::Unknown;
        }
    }
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
                throw std::system_error(errno, std::system_category(), "Failed to open device file");

            if (ioctl(fd, EVIOCGRAB, 1) == -1)
                engine->log(Log::Level::Warning) << "Failed to grab device";

            char deviceName[256];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
                engine->log(Log::Level::Warning) << "Failed to get device name";
            else
            {
                name = deviceName;
                engine->log(Log::Level::Info) << "Got device: " << name;
            }

            unsigned long eventBits[bitsToLongs(EV_CNT)];
            unsigned long absBits[bitsToLongs(ABS_CNT)];
            unsigned long relBits[bitsToLongs(REL_CNT)];
            unsigned long keyBits[bitsToLongs(KEY_CNT)];

            if (ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relBits)), relBits) == -1 ||
                ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to get device event bits");

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
                keyboardDevice = std::make_unique<KeyboardDevice>(inputSystem, inputSystem.getNextDeviceId());

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if ((isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN)) || // tablet
                    (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN)) || // touchpad
                    isBitSet(keyBits, BTN_TOUCH)) // touchscreen
                {
                    touchpadDevice = std::make_unique<TouchpadDevice>(inputSystem, inputSystem.getNextDeviceId(), false);

                    input_absinfo info;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

                    touchSlots.resize(info.maximum + 1);

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &info) == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

                    touchMinX = info.minimum;
                    touchMaxX = info.maximum;
                    touchRangeX = touchMaxX - touchMinX;

                    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &info) == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

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
                    mouseDevice = std::make_unique<MouseDevice>(inputSystem, inputSystem.getNextDeviceId());
            }
            else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
            {
                if (isBitSet(keyBits, BTN_MOUSE))
                    mouseDevice = std::make_unique<MouseDevice>(inputSystem, inputSystem.getNextDeviceId());
            }

            if (isBitSet(keyBits, BTN_JOYSTICK) || // joystick
                isBitSet(keyBits, BTN_GAMEPAD)) // gamepad
            {
                gamepadDevice = std::make_unique<GamepadDevice>(inputSystem, inputSystem.getNextDeviceId());

                struct input_id id;
                if (ioctl(fd, EVIOCGID, &id) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get device info");

                const GamepadConfig& gamepadConfig = getGamepadConfig(id.vendor, id.product);

                for (std::size_t buttonNum = 0; buttonNum < 24; ++buttonNum)
                {
                    if (gamepadConfig.buttonMap[buttonNum] != Gamepad::Button::Unknown)
                    {
                        Button button;
                        button.button = gamepadConfig.buttonMap[buttonNum];
                        buttons.insert(std::make_pair(BTN_GAMEPAD + buttonNum, button));
                    }
                }

                constexpr std::array<std::uint32_t, 6> axisUsageMap = {
                    ABS_X,
                    ABS_Y,
                    ABS_Z,
                    ABS_RX,
                    ABS_RY,
                    ABS_RZ
                };

                for (std::size_t axisNum = 0; axisNum < 6; ++axisNum)
                {
                    if (gamepadConfig.axisMap[axisNum] != Gamepad::Axis::Unknown)
                    {
                        const std::uint32_t usage = axisUsageMap[axisNum];

                        Axis axis;
                        axis.axis = gamepadConfig.axisMap[axisNum];

                        input_absinfo info;

                        if (ioctl(fd, EVIOCGABS(usage), &info) == -1)
                            throw std::system_error(errno, std::system_category(), "Failed to get device info");

                        axis.min = info.minimum;
                        axis.max = info.maximum;
                        axis.range = info.maximum - info.minimum;

                        switch (gamepadConfig.axisMap[axisNum])
                        {
                            case Gamepad::Axis::Unknown:
                                break;
                            case Gamepad::Axis::LeftThumbX:
                                axis.negativeButton = Gamepad::Button::LeftThumbLeft;
                                axis.positiveButton = Gamepad::Button::LeftThumbRight;
                                break;
                            case Gamepad::Axis::LeftThumbY:
                                axis.negativeButton = Gamepad::Button::LeftThumbUp;
                                axis.positiveButton = Gamepad::Button::LeftThumbDown;
                                break;
                            case Gamepad::Axis::RightThumbX:
                                axis.negativeButton = Gamepad::Button::RightThumbLeft;
                                axis.positiveButton = Gamepad::Button::RightThumbRight;
                                break;
                            case Gamepad::Axis::RightThumbY:
                                axis.negativeButton = Gamepad::Button::RightThumbUp;
                                axis.positiveButton = Gamepad::Button::RightThumbDown;
                                break;
                            case Gamepad::Axis::LeftTrigger:
                                axis.negativeButton = Gamepad::Button::LeftTrigger;
                                axis.positiveButton = Gamepad::Button::LeftTrigger;
                                hasLeftTrigger = true;
                                break;
                            case Gamepad::Axis::RightTrigger:
                                axis.negativeButton = Gamepad::Button::RightTrigger;
                                axis.positiveButton = Gamepad::Button::RightTrigger;
                                hasRightTrigger = true;
                                break;
                        }

                        axes.insert(std::make_pair(usage, axis));
                    }
                }
            }
        }

        EventDevice::~EventDevice()
        {
            if (fd != -1)
            {
                if (ioctl(fd, EVIOCGRAB, 0) == -1)
                    engine->log(Log::Level::Warning) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            input_event events[32];
            ssize_t bytesRead = read(fd, events, sizeof(events));

            if (bytesRead == -1)
                throw std::system_error(errno, std::system_category(), "Failed to read from " + filename);

            const int eventCount = bytesRead / sizeof(input_event);

            for (int eventNum = 0; eventNum < eventCount; ++eventNum)
            {
                input_event& event = events[eventNum];

                if (keyboardDevice)
                {
                    switch (event.type)
                    {
                        case EV_KEY:
                            if (event.value == 1 || event.value == 2) // press or repeat
                                keyboardDevice->handleKeyPress(convertKeyCode(event.code));
                            else if (event.value == 0) // release
                                keyboardDevice->handleKeyRelease(convertKeyCode(event.code));
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
                                    cursorPosition.x = event.value;
                                    break;
                                case ABS_Y:
                                    cursorPosition.y = event.value;
                                    break;
                            }

                            const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                static_cast<float>(cursorPosition.y)));
                            mouseDevice->handleMove(normalizedPosition);
                            break;
                        }
                        case EV_REL:
                        {
                            switch (event.code)
                            {
                                case REL_X:
                                {
                                    const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(event.value), 0.0F));
                                    mouseDevice->handleRelativeMove(normalizedPosition);
                                    break;
                                }
                                case REL_Y:
                                {
                                    const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(0.0F, static_cast<float>(event.value)));
                                    mouseDevice->handleRelativeMove(normalizedPosition);
                                    break;
                                }
                                case REL_WHEEL:
                                {
                                    const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                        static_cast<float>(cursorPosition.y)));
                                    mouseDevice->handleScroll(Vector2F(0.0F, static_cast<float>(event.value)), normalizedPosition);
                                    break;
                                }
                                case REL_HWHEEL:
                                {
                                    const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                        static_cast<float>(cursorPosition.y)));
                                    mouseDevice->handleScroll(Vector2F(static_cast<float>(event.value), 0.0F), normalizedPosition);
                                    break;
                                }
                            }
                            break;
                        }
                        case EV_KEY:
                        {
                            const Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                static_cast<float>(cursorPosition.y)));

                            if (event.value == 1)
                                mouseDevice->handleButtonPress(convertButtonCode(event.code), normalizedPosition);
                            else if (event.value == 0)
                                mouseDevice->handleButtonRelease(convertButtonCode(event.code), normalizedPosition);
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
                                        touchSlots[currentTouchSlot].action = Slot::Action::Begin;
                                    }
                                    else
                                        touchSlots[currentTouchSlot].action = Slot::Action::End;
                                    break;
                                }
                                case ABS_MT_POSITION_X:
                                {
                                    touchSlots[currentTouchSlot].positionX = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::Move;
                                    break;
                                }
                                case ABS_MT_POSITION_Y:
                                {
                                    touchSlots[currentTouchSlot].positionY = event.value;
                                    touchSlots[currentTouchSlot].action = Slot::Action::Move;
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
                                        if (slot.action != Slot::Action::Unknown)
                                        {
                                            Vector2F position(static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
                                                              static_cast<float>(slot.positionY - touchMinY) / touchRangeY);
                                            auto pressure = static_cast<float>(slot.pressure - touchMinPressure) / touchMaxPressure;

                                            switch (slot.action)
                                            {
                                                case Slot::Action::Unknown:
                                                    break;
                                                case Slot::Action::Begin:
                                                    touchpadDevice->handleTouchBegin(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::End:
                                                    touchpadDevice->handleTouchEnd(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                                case Slot::Action::Move:
                                                    touchpadDevice->handleTouchMove(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                    break;
                                            }

                                            slot.action = Slot::Action::Unknown;
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

                                    std::size_t size = sizeof(input_mt_request_layout::code) +
                                        sizeof(*input_mt_request_layout::values) * touchSlots.size();

                                    std::unique_ptr<input_mt_request_layout, decltype(&free)> request(static_cast<input_mt_request_layout*>(malloc(size)), free);

                                    request->code = ABS_MT_TRACKING_ID;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request.get()) == -1)
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

                                    for (std::size_t touchNum = 0; touchNum < touchSlots.size(); ++touchNum)
                                    {
                                        if (touchSlots[touchNum].trackingId < 0 &&
                                            request->values[touchNum] >= 0)
                                        {
                                            touchSlots[touchNum].trackingId = request->values[touchNum];
                                            touchSlots[touchNum].action = Slot::Action::Begin;
                                        }
                                        else if (touchSlots[touchNum].trackingId >= 0 &&
                                                 request->values[touchNum] < 0)
                                        {
                                            touchSlots[touchNum].trackingId = request->values[touchNum];
                                            touchSlots[touchNum].action = Slot::Action::End;
                                        }
                                    }

                                    request->code = ABS_MT_POSITION_X;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request.get()) == -1)
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

                                    for (std::size_t touchNum = 0; touchNum < touchSlots.size(); ++touchNum)
                                    {
                                        if (touchSlots[touchNum].trackingId >= 0 &&
                                            touchSlots[touchNum].positionX != request->values[touchNum])
                                        {
                                            touchSlots[touchNum].positionX = request->values[touchNum];
                                            if (touchSlots[touchNum].action == Slot::Action::Unknown)
                                                touchSlots[touchNum].action = Slot::Action::Move;
                                        }
                                    }

                                    request->code = ABS_MT_POSITION_Y;
                                    if (ioctl(fd, EVIOCGMTSLOTS(size), request.get()) == -1)
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

                                    for (std::size_t touchNum = 0; touchNum < touchSlots.size(); ++touchNum)
                                    {
                                        if (touchSlots[touchNum].trackingId >= 0 &&
                                            touchSlots[touchNum].positionY != request->values[touchNum])
                                        {
                                            touchSlots[touchNum].positionY = request->values[touchNum];
                                            if (touchSlots[touchNum].action == Slot::Action::Unknown)
                                                touchSlots[touchNum].action = Slot::Action::Move;
                                        }
                                    }

                                    request->code = ABS_MT_PRESSURE;
                                    if (ioctl(fd, EVIOCGABS(size), request.get()) != -1)
                                    {
                                        for (std::size_t touchNum = 0; touchNum < touchSlots.size(); ++touchNum)
                                        {
                                            if (touchSlots[touchNum].trackingId >= 0 &&
                                                touchSlots[touchNum].pressure != request->values[touchNum])
                                            {
                                                touchSlots[touchNum].pressure = request->values[touchNum];
                                                if (touchSlots[touchNum].action == Slot::Action::Unknown)
                                                    touchSlots[touchNum].action = Slot::Action::Move;
                                            }
                                        }
                                    }

                                    input_absinfo info;
                                    if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &info) == -1)
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");
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
                            if (event.code == ABS_HAT0X)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DpadRight : Gamepad::Button::DpadLeft, true, 1.0F);
                                else if (hat0XValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0XValue > 0) ? Gamepad::Button::DpadRight : Gamepad::Button::DpadLeft, false, 0.0F);

                                hat0XValue = event.value;
                            }
                            else if (event.code == ABS_HAT0Y)
                            {
                                if (event.value != 0)
                                    gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::DpadDown : Gamepad::Button::DpadUp, true, 1.0F);
                                else if (hat0YValue != 0)
                                    gamepadDevice->handleButtonValueChange((hat0YValue > 0) ? Gamepad::Button::DpadDown : Gamepad::Button::DpadUp, false, 0.0F);

                                hat0YValue = event.value;
                            }

                            auto axisIterator = axes.find(event.code);

                            if (axisIterator != axes.end())
                            {
                                Axis& axis = axisIterator->second;

                                handleAxisChange(axis.value,
                                                 event.value,
                                                 axis.min, axis.range,
                                                 axis.negativeButton, axis.positiveButton);

                                axis.value = event.value;
                            }
                            break;
                        }
                        case EV_KEY:
                        {
                            auto buttonIterator = buttons.find(event.code);

                            if (buttonIterator != buttons.end())
                            {
                                Button& button = buttonIterator->second;

                                if ((button.button != Gamepad::Button::LeftTrigger || !hasLeftTrigger) &&
                                    (button.button != Gamepad::Button::RightTrigger || !hasRightTrigger))
                                {
                                    gamepadDevice->handleButtonValueChange(button.button, event.value > 0, (event.value > 0) ? 1.0F : 0.0F);
                                }

                                button.value = event.value;
                            }
                            break;
                        }
                    }
                }
            }
        }

        void EventDevice::handleAxisChange(std::int32_t oldValue, std::int32_t newValue,
                                           std::int32_t min, std::int32_t range,
                                           Gamepad::Button negativeButton,
                                           Gamepad::Button positiveButton)
        {
            if (negativeButton == positiveButton)
            {
                const auto floatValue = static_cast<float>(newValue - min) / range;

                gamepadDevice->handleButtonValueChange(negativeButton,
                                                       floatValue > 0.0F,
                                                       floatValue);
            }
            else
            {
                const auto floatValue = 2.0F * static_cast<float>(newValue - min) / range - 1.0F;

                if (floatValue > 0.0F)
                {
                    gamepadDevice->handleButtonValueChange(positiveButton,
                                                           floatValue > THUMB_DEADZONE,
                                                           floatValue);
                }
                else if (floatValue < 0.0F)
                {
                    gamepadDevice->handleButtonValueChange(negativeButton,
                                                           -floatValue > THUMB_DEADZONE,
                                                           -floatValue);
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                        gamepadDevice->handleButtonValueChange(positiveButton, false, 0.0F);
                    else
                        gamepadDevice->handleButtonValueChange(negativeButton, false, 0.0F);
                }
            }
        }
    } // namespace input
} // namespace ouzel
