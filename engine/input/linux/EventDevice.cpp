// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <system_error>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "EventDevice.hpp"
#include "InputSystemLinux.hpp"
#include "../GamepadConfig.hpp"
#include "../KeyboardDevice.hpp"
#include "../GamepadDevice.hpp"
#include "../MouseDevice.hpp"
#include "../TouchpadDevice.hpp"
#include "../../core/Engine.hpp"
#include "../../utils/Log.hpp"
#include "../../utils/Utils.hpp"

namespace
{
    constexpr float thumbDeadzone = 0.2F;
    constexpr std::uint32_t bitsPerLong = 8 * sizeof(long);

    constexpr std::size_t bitsToLongs(std::size_t n)
    {
        return (n + bitsPerLong - 1) / bitsPerLong; // rounded up
    }

    inline auto isBitSet(const unsigned long* array, int bit)
    {
        return (array[bit / bitsPerLong] & (1LL << (bit % bitsPerLong))) != 0;
    }

    ouzel::input::Keyboard::Key convertKeyCode(std::uint16_t keyCode)
    {
        switch (keyCode)
        {
            case KEY_ESC: return ouzel::input::Keyboard::Key::escape;
            case KEY_1: return ouzel::input::Keyboard::Key::num1;
            case KEY_2: return ouzel::input::Keyboard::Key::num2;
            case KEY_3: return ouzel::input::Keyboard::Key::num3;
            case KEY_4: return ouzel::input::Keyboard::Key::num4;
            case KEY_5: return ouzel::input::Keyboard::Key::num5;
            case KEY_6: return ouzel::input::Keyboard::Key::num6;
            case KEY_7: return ouzel::input::Keyboard::Key::num7;
            case KEY_8: return ouzel::input::Keyboard::Key::num8;
            case KEY_9: return ouzel::input::Keyboard::Key::num9;
            case KEY_0: return ouzel::input::Keyboard::Key::num0;
            case KEY_MINUS: return ouzel::input::Keyboard::Key::minus;
            case KEY_EQUAL: return ouzel::input::Keyboard::Key::equal;
            case KEY_BACKSPACE: return ouzel::input::Keyboard::Key::backspace;
            case KEY_TAB: return ouzel::input::Keyboard::Key::tab;
            case KEY_Q: return ouzel::input::Keyboard::Key::q;
            case KEY_W: return ouzel::input::Keyboard::Key::w;
            case KEY_E: return ouzel::input::Keyboard::Key::e;
            case KEY_R: return ouzel::input::Keyboard::Key::r;
            case KEY_T: return ouzel::input::Keyboard::Key::t;
            case KEY_Y: return ouzel::input::Keyboard::Key::y;
            case KEY_U: return ouzel::input::Keyboard::Key::u;
            case KEY_I: return ouzel::input::Keyboard::Key::i;
            case KEY_O: return ouzel::input::Keyboard::Key::o;
            case KEY_P: return ouzel::input::Keyboard::Key::p;
            case KEY_LEFTBRACE: return ouzel::input::Keyboard::Key::leftBracket;
            case KEY_RIGHTBRACE: return ouzel::input::Keyboard::Key::rightBracket;
            case KEY_ENTER: return ouzel::input::Keyboard::Key::enter;
            case KEY_LEFTCTRL: return ouzel::input::Keyboard::Key::leftControl;
            case KEY_A: return ouzel::input::Keyboard::Key::a;
            case KEY_S: return ouzel::input::Keyboard::Key::s;
            case KEY_D: return ouzel::input::Keyboard::Key::d;
            case KEY_F: return ouzel::input::Keyboard::Key::f;
            case KEY_G: return ouzel::input::Keyboard::Key::g;
            case KEY_H: return ouzel::input::Keyboard::Key::h;
            case KEY_J: return ouzel::input::Keyboard::Key::j;
            case KEY_K: return ouzel::input::Keyboard::Key::k;
            case KEY_L: return ouzel::input::Keyboard::Key::l;
            case KEY_SEMICOLON: return ouzel::input::Keyboard::Key::semicolon;
            case KEY_APOSTROPHE: return ouzel::input::Keyboard::Key::quote;
            case KEY_GRAVE: return ouzel::input::Keyboard::Key::grave;
            case KEY_LEFTSHIFT: return ouzel::input::Keyboard::Key::leftShift;
            case KEY_BACKSLASH: return ouzel::input::Keyboard::Key::backslash;
            case KEY_Z: return ouzel::input::Keyboard::Key::z;
            case KEY_X: return ouzel::input::Keyboard::Key::x;
            case KEY_C: return ouzel::input::Keyboard::Key::c;
            case KEY_V: return ouzel::input::Keyboard::Key::v;
            case KEY_B: return ouzel::input::Keyboard::Key::b;
            case KEY_N: return ouzel::input::Keyboard::Key::n;
            case KEY_M: return ouzel::input::Keyboard::Key::m;
            case KEY_COMMA: return ouzel::input::Keyboard::Key::comma;
            case KEY_DOT: return ouzel::input::Keyboard::Key::period;
            case KEY_SLASH: return ouzel::input::Keyboard::Key::slash;
            case KEY_RIGHTSHIFT: return ouzel::input::Keyboard::Key::rightShift;
            case KEY_KPASTERISK: return ouzel::input::Keyboard::Key::numpadMultiply;
            case KEY_LEFTALT: return ouzel::input::Keyboard::Key::leftAlt;
            case KEY_SPACE: return ouzel::input::Keyboard::Key::space;
            case KEY_CAPSLOCK: return ouzel::input::Keyboard::Key::capsLock;
            case KEY_F1: return ouzel::input::Keyboard::Key::f1;
            case KEY_F2: return ouzel::input::Keyboard::Key::f2;
            case KEY_F3: return ouzel::input::Keyboard::Key::f3;
            case KEY_F4: return ouzel::input::Keyboard::Key::f4;
            case KEY_F5: return ouzel::input::Keyboard::Key::f5;
            case KEY_F6: return ouzel::input::Keyboard::Key::f6;
            case KEY_F7: return ouzel::input::Keyboard::Key::f7;
            case KEY_F8: return ouzel::input::Keyboard::Key::f8;
            case KEY_F9: return ouzel::input::Keyboard::Key::f9;
            case KEY_F10: return ouzel::input::Keyboard::Key::f10;
            case KEY_NUMLOCK: return ouzel::input::Keyboard::Key::numLock;
            case KEY_SCROLLLOCK: return ouzel::input::Keyboard::Key::scrollLock;
            case KEY_KP7: return ouzel::input::Keyboard::Key::numpad7;
            case KEY_KP8: return ouzel::input::Keyboard::Key::numpad8;
            case KEY_KP9: return ouzel::input::Keyboard::Key::numpad9;
            case KEY_KPMINUS: return ouzel::input::Keyboard::Key::numpadMinus;
            case KEY_KP4: return ouzel::input::Keyboard::Key::numpad4;
            case KEY_KP5: return ouzel::input::Keyboard::Key::numpad5;
            case KEY_KP6: return ouzel::input::Keyboard::Key::numpad6;
            case KEY_KPPLUS: return ouzel::input::Keyboard::Key::numpadPlus;
            case KEY_KP1: return ouzel::input::Keyboard::Key::numpad1;
            case KEY_KP2: return ouzel::input::Keyboard::Key::numpad2;
            case KEY_KP3: return ouzel::input::Keyboard::Key::numpad3;
            case KEY_KP0: return ouzel::input::Keyboard::Key::numpad0;
            case KEY_KPDOT: return ouzel::input::Keyboard::Key::numpadDecimal;

            case KEY_ZENKAKUHANKAKU: return ouzel::input::Keyboard::Key::none; // ??
            case KEY_102ND: return ouzel::input::Keyboard::Key::intlBackslash;
            case KEY_F11: return ouzel::input::Keyboard::Key::f11;
            case KEY_F12: return ouzel::input::Keyboard::Key::f12;
            case KEY_RO: return ouzel::input::Keyboard::Key::ro;
            case KEY_KATAKANA: return ouzel::input::Keyboard::Key::katakana;
            case KEY_HIRAGANA: return ouzel::input::Keyboard::Key::hiragana;
            case KEY_HENKAN: return ouzel::input::Keyboard::Key::henkan;
            case KEY_KATAKANAHIRAGANA: return ouzel::input::Keyboard::Key::katakanaHiragana;
            case KEY_MUHENKAN: return ouzel::input::Keyboard::Key::muhenkan;
            case KEY_KPJPCOMMA: return ouzel::input::Keyboard::Key::numpadJpcomma;
            case KEY_KPENTER: return ouzel::input::Keyboard::Key::numpadEnter;
            case KEY_RIGHTCTRL: return ouzel::input::Keyboard::Key::rightControl;
            case KEY_KPSLASH: return ouzel::input::Keyboard::Key::numpadDivide;
            case KEY_SYSRQ: return ouzel::input::Keyboard::Key::printScreen;
            case KEY_RIGHTALT: return ouzel::input::Keyboard::Key::rightAlt;
            case KEY_LINEFEED: return ouzel::input::Keyboard::Key::none; // ??
            case KEY_HOME: return ouzel::input::Keyboard::Key::home;
            case KEY_UP: return ouzel::input::Keyboard::Key::up;
            case KEY_PAGEUP: return ouzel::input::Keyboard::Key::pageUp;
            case KEY_LEFT: return ouzel::input::Keyboard::Key::left;
            case KEY_RIGHT: return ouzel::input::Keyboard::Key::right;
            case KEY_END: return ouzel::input::Keyboard::Key::end;
            case KEY_DOWN: return ouzel::input::Keyboard::Key::down;
            case KEY_PAGEDOWN: return ouzel::input::Keyboard::Key::pageDown;
            case KEY_INSERT: return ouzel::input::Keyboard::Key::insert;
            case KEY_DELETE: return ouzel::input::Keyboard::Key::del;
            case KEY_MACRO: return ouzel::input::Keyboard::Key::none; // ??
            case KEY_MAIL: return ouzel::input::Keyboard::Key::mail;
            case KEY_MUTE: return ouzel::input::Keyboard::Key::mute;
            case KEY_VOLUMEDOWN: return ouzel::input::Keyboard::Key::volumeDown;
            case KEY_VOLUMEUP: return ouzel::input::Keyboard::Key::volumeUp;
            case KEY_NEXTSONG: return ouzel::input::Keyboard::Key::audioNext;
            case KEY_PLAYPAUSE: return ouzel::input::Keyboard::Key::audioPlay;
            case KEY_PREVIOUSSONG: return ouzel::input::Keyboard::Key::audioPrevious;
            case KEY_STOPCD: return ouzel::input::Keyboard::Key::audioStop;

            case KEY_POWER: return ouzel::input::Keyboard::Key::power;
            case KEY_KPEQUAL: return ouzel::input::Keyboard::Key::numpadEqual;
            case KEY_KPPLUSMINUS: return ouzel::input::Keyboard::Key::numpadPlusminus;
            case KEY_PAUSE: return ouzel::input::Keyboard::Key::pause;
            case KEY_SCALE: return ouzel::input::Keyboard::Key::none; //?

            case KEY_KPCOMMA: return ouzel::input::Keyboard::Key::numpadSeparator;
            case KEY_HANGEUL: return ouzel::input::Keyboard::Key::kana;
            case KEY_HANJA: return ouzel::input::Keyboard::Key::hanja;
            case KEY_YEN: return ouzel::input::Keyboard::Key::yen;
            case KEY_LEFTMETA: return ouzel::input::Keyboard::Key::leftSuper;
            case KEY_RIGHTMETA: return ouzel::input::Keyboard::Key::rightSuper;
            case KEY_COMPOSE: return ouzel::input::Keyboard::Key::none; // ??

            case KEY_F13: return ouzel::input::Keyboard::Key::f13;
            case KEY_F14: return ouzel::input::Keyboard::Key::f14;
            case KEY_F15: return ouzel::input::Keyboard::Key::f15;
            case KEY_F16: return ouzel::input::Keyboard::Key::f16;
            case KEY_F17: return ouzel::input::Keyboard::Key::f17;
            case KEY_F18: return ouzel::input::Keyboard::Key::f18;
            case KEY_F19: return ouzel::input::Keyboard::Key::f19;
            case KEY_F20: return ouzel::input::Keyboard::Key::f20;
            case KEY_F21: return ouzel::input::Keyboard::Key::f21;
            case KEY_F22: return ouzel::input::Keyboard::Key::f22;
            case KEY_F23: return ouzel::input::Keyboard::Key::f23;
            case KEY_F24: return ouzel::input::Keyboard::Key::f24;

            case KEY_MENU: return ouzel::input::Keyboard::Key::menu;
            case KEY_CLEAR: return ouzel::input::Keyboard::Key::clear;
            case KEY_SLEEP: return ouzel::input::Keyboard::Key::sleep;
            case KEY_PROG1: return ouzel::input::Keyboard::Key::app1;
            case KEY_PROG2: return ouzel::input::Keyboard::Key::app2;
            case KEY_HELP: return ouzel::input::Keyboard::Key::help;

            case KEY_BACK: return ouzel::input::Keyboard::Key::back;
            case KEY_FORWARD: return ouzel::input::Keyboard::Key::forward;
            case KEY_REFRESH: return ouzel::input::Keyboard::Key::refresh;
            case KEY_STOP: return ouzel::input::Keyboard::Key::stop;
            case KEY_FIND: return ouzel::input::Keyboard::Key::search;
            case KEY_BOOKMARKS: return ouzel::input::Keyboard::Key::bookmarks;
            case KEY_HOMEPAGE: return ouzel::input::Keyboard::Key::homepage;
            case KEY_MODE: return ouzel::input::Keyboard::Key::modeChange;
            default: return ouzel::input::Keyboard::Key::none;
        }
    }

    ouzel::input::Mouse::Button convertButtonCode(std::uint16_t buttonCode)
    {
        switch (buttonCode)
        {
            case BTN_LEFT: return ouzel::input::Mouse::Button::left;
            case BTN_RIGHT: return ouzel::input::Mouse::Button::right;
            case BTN_MIDDLE: return ouzel::input::Mouse::Button::middle;
            case BTN_SIDE: return ouzel::input::Mouse::Button::x1;
            case BTN_EXTRA: return ouzel::input::Mouse::Button::x2;
            case BTN_FORWARD: return ouzel::input::Mouse::Button::none; //?
            case BTN_BACK: return ouzel::input::Mouse::Button::none; //?
            case BTN_TASK: return ouzel::input::Mouse::Button::none; //?
            default: return ouzel::input::Mouse::Button::none;
        }
    }
}

namespace ouzel::input::linux
{
    EventDevice::EventDevice(InputSystem& inputSystem, const std::string& initFilename):
        filename(initFilename)
    {
        while ((fd = open(filename.c_str(), O_RDONLY)) == -1)
            if (errno != EINTR)
                throw std::system_error(errno, std::system_category(), "Failed to open device file");

        if (ioctl(fd, EVIOCGRAB, 1) == -1)
            logger.log(Log::Level::warning) << "Failed to grab device";

        char deviceName[256];
        if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
            logger.log(Log::Level::warning) << "Failed to get device name";
        else
        {
            name = deviceName;
            logger.log(Log::Level::info) << "Got device: " << name;
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

            const auto& gamepadConfig = getGamepadConfig(id.vendor, id.product);

            for (std::size_t buttonNum = 0; buttonNum < 24; ++buttonNum)
                if (gamepadConfig.buttonMap[buttonNum] != Gamepad::Button::none)
                {
                    Button button;
                    button.button = gamepadConfig.buttonMap[buttonNum];
                    buttons.insert(std::pair(BTN_GAMEPAD + buttonNum, button));
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
                if (gamepadConfig.axisMap[axisNum] != Gamepad::Axis::none)
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
                        case Gamepad::Axis::none:
                            break;
                        case Gamepad::Axis::leftThumbX:
                            axis.negativeButton = Gamepad::Button::leftThumbLeft;
                            axis.positiveButton = Gamepad::Button::leftThumbRight;
                            break;
                        case Gamepad::Axis::leftThumbY:
                            axis.negativeButton = Gamepad::Button::leftThumbUp;
                            axis.positiveButton = Gamepad::Button::leftThumbDown;
                            break;
                        case Gamepad::Axis::rightThumbX:
                            axis.negativeButton = Gamepad::Button::rightThumbLeft;
                            axis.positiveButton = Gamepad::Button::rightThumbRight;
                            break;
                        case Gamepad::Axis::rightThumbY:
                            axis.negativeButton = Gamepad::Button::rightThumbUp;
                            axis.positiveButton = Gamepad::Button::rightThumbDown;
                            break;
                        case Gamepad::Axis::leftTrigger:
                            axis.negativeButton = Gamepad::Button::leftTrigger;
                            axis.positiveButton = Gamepad::Button::leftTrigger;
                            hasLeftTrigger = true;
                            break;
                        case Gamepad::Axis::rightTrigger:
                            axis.negativeButton = Gamepad::Button::rightTrigger;
                            axis.positiveButton = Gamepad::Button::rightTrigger;
                            hasRightTrigger = true;
                            break;
                    }

                    axes.insert(std::pair(usage, axis));
                }
        }
    }

    EventDevice::~EventDevice()
    {
        if (fd != -1)
        {
            if (ioctl(fd, EVIOCGRAB, 0) == -1)
                logger.log(Log::Level::warning) << "Failed to release device";

            close(fd);
        }
    }

    void EventDevice::update()
    {
        ssize_t bytesRead = read(fd, buffer + bufferSize, sizeof(buffer) - bufferSize);
        while (bytesRead == -1 && errno == EINTR)
            bytesRead = read(fd, buffer + bufferSize, sizeof(buffer) - bufferSize);

        if (bytesRead == -1)
            throw std::system_error(errno, std::system_category(), "Failed to read from " + filename);

        bufferSize += static_cast<std::size_t>(bytesRead);

        const auto eventCount = bufferSize / sizeof(input_event);
        for (std::size_t eventNum = 0; eventNum < eventCount; ++eventNum)
        {
            const input_event& event = *bitCast<input_event*>(buffer + eventNum * sizeof(input_event));

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

                        const Vector2F position{
                            static_cast<float>(cursorPosition.x),
                            static_cast<float>(cursorPosition.y)
                        };
                        const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);
                        mouseDevice->handleMove(normalizedPosition);
                        break;
                    }
                    case EV_REL:
                    {
                        switch (event.code)
                        {
                            case REL_X:
                            {
                                const Vector2F position{
                                    static_cast<float>(event.value),
                                    0.0F
                                };
                                const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);
                                mouseDevice->handleRelativeMove(normalizedPosition);
                                break;
                            }
                            case REL_Y:
                            {
                                const Vector2F position{
                                    0.0F,
                                    static_cast<float>(event.value)
                                };
                                const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);
                                mouseDevice->handleRelativeMove(normalizedPosition);
                                break;
                            }
                            case REL_WHEEL:
                            {
                                const Vector2F position{
                                    static_cast<float>(cursorPosition.x),
                                    static_cast<float>(cursorPosition.y)
                                };
                                const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);
                                mouseDevice->handleScroll(Vector2F(0.0F, static_cast<float>(event.value)), normalizedPosition);
                                break;
                            }
                            case REL_HWHEEL:
                            {
                                const Vector2F position{
                                    static_cast<float>(cursorPosition.x),
                                    static_cast<float>(cursorPosition.y)
                                };
                                const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);
                                mouseDevice->handleScroll(Vector2F(static_cast<float>(event.value), 0.0F), normalizedPosition);
                                break;
                            }
                        }
                        break;
                    }
                    case EV_KEY:
                    {
                        const Vector2F position{
                            static_cast<float>(cursorPosition.x),
                            static_cast<float>(cursorPosition.y)
                        };
                        const auto normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(position);

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
                                    touchSlots[currentTouchSlot].action = Slot::Action::begin;
                                }
                                else
                                    touchSlots[currentTouchSlot].action = Slot::Action::end;
                                break;
                            }
                            case ABS_MT_POSITION_X:
                            {
                                touchSlots[currentTouchSlot].positionX = event.value;
                                touchSlots[currentTouchSlot].action = Slot::Action::move;
                                break;
                            }
                            case ABS_MT_POSITION_Y:
                            {
                                touchSlots[currentTouchSlot].positionY = event.value;
                                touchSlots[currentTouchSlot].action = Slot::Action::move;
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
                                    if (slot.action != Slot::Action::none)
                                    {
                                        const Vector2F position{
                                            static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
                                            static_cast<float>(slot.positionY - touchMinY) / touchRangeY
                                        };
                                        const auto pressure = static_cast<float>(slot.pressure - touchMinPressure) / touchMaxPressure;

                                        switch (slot.action)
                                        {
                                            case Slot::Action::none:
                                                break;
                                            case Slot::Action::begin:
                                                touchpadDevice->handleTouchBegin(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                break;
                                            case Slot::Action::end:
                                                touchpadDevice->handleTouchEnd(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                break;
                                            case Slot::Action::move:
                                                touchpadDevice->handleTouchMove(static_cast<std::uint64_t>(slot.trackingId), position, pressure);
                                                break;
                                        }

                                        slot.action = Slot::Action::none;
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

                                const std::size_t size = sizeof(input_mt_request_layout::code) +
                                    sizeof(*input_mt_request_layout::values) * touchSlots.size();

                                using FreeFunction = void(*)(void*);
                                std::unique_ptr<input_mt_request_layout, FreeFunction> request(static_cast<input_mt_request_layout*>(malloc(size)), free);

                                request->code = ABS_MT_TRACKING_ID;
                                if (ioctl(fd, EVIOCGMTSLOTS(size), request.get()) == -1)
                                    throw std::system_error(errno, std::system_category(), "Failed to get device info");

                                for (std::size_t touchNum = 0; touchNum < touchSlots.size(); ++touchNum)
                                {
                                    if (touchSlots[touchNum].trackingId < 0 &&
                                        request->values[touchNum] >= 0)
                                    {
                                        touchSlots[touchNum].trackingId = request->values[touchNum];
                                        touchSlots[touchNum].action = Slot::Action::begin;
                                    }
                                    else if (touchSlots[touchNum].trackingId >= 0 &&
                                                request->values[touchNum] < 0)
                                    {
                                        touchSlots[touchNum].trackingId = request->values[touchNum];
                                        touchSlots[touchNum].action = Slot::Action::end;
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
                                        if (touchSlots[touchNum].action == Slot::Action::none)
                                            touchSlots[touchNum].action = Slot::Action::move;
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
                                        if (touchSlots[touchNum].action == Slot::Action::none)
                                            touchSlots[touchNum].action = Slot::Action::move;
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
                                            if (touchSlots[touchNum].action == Slot::Action::none)
                                                touchSlots[touchNum].action = Slot::Action::move;
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
                switch (event.type)
                {
                    case EV_ABS:
                    {
                        if (event.code == ABS_HAT0X)
                        {
                            if (event.value != 0)
                                gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::dPadRight : Gamepad::Button::dPadLeft, true, 1.0F);
                            else if (hat0XValue != 0)
                                gamepadDevice->handleButtonValueChange((hat0XValue > 0) ? Gamepad::Button::dPadRight : Gamepad::Button::dPadLeft, false, 0.0F);

                            hat0XValue = event.value;
                        }
                        else if (event.code == ABS_HAT0Y)
                        {
                            if (event.value != 0)
                                gamepadDevice->handleButtonValueChange((event.value > 0) ? Gamepad::Button::dPadDown : Gamepad::Button::dPadUp, true, 1.0F);
                            else if (hat0YValue != 0)
                                gamepadDevice->handleButtonValueChange((hat0YValue > 0) ? Gamepad::Button::dPadDown : Gamepad::Button::dPadUp, false, 0.0F);

                            hat0YValue = event.value;
                        }

                        const auto axisIterator = axes.find(event.code);

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
                        const auto buttonIterator = buttons.find(event.code);

                        if (buttonIterator != buttons.end())
                        {
                            Button& button = buttonIterator->second;

                            if ((button.button != Gamepad::Button::leftTrigger || !hasLeftTrigger) &&
                                (button.button != Gamepad::Button::rightTrigger || !hasRightTrigger))
                                gamepadDevice->handleButtonValueChange(button.button, event.value > 0, (event.value > 0) ? 1.0F : 0.0F);

                            button.value = event.value;
                        }
                        break;
                    }
                }
            }
        }

        if (eventCount > 0)
            std::memcpy(buffer, buffer + eventCount * sizeof(input_event),
                        bufferSize - eventCount * sizeof(input_event));
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
                                                       floatValue > thumbDeadzone,
                                                       floatValue);
            }
            else if (floatValue < 0.0F)
            {
                gamepadDevice->handleButtonValueChange(negativeButton,
                                                       -floatValue > thumbDeadzone,
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
}
