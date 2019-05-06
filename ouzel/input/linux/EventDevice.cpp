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

static constexpr float THUMB_DEADZONE = 0.2F;

static constexpr uint32_t BITS_PER_LONG = 8 * sizeof(long);

static constexpr size_t bitsToLongs(size_t n)
{
    return (n + BITS_PER_LONG - 1) / BITS_PER_LONG; // rounded up
}

static inline bool isBitSet(const unsigned long* array, int bit)
{
    return (array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG))) != 0;
}

static ouzel::input::Keyboard::Key convertKeyCode(uint16_t keyCode)
{
    switch (keyCode)
    {
        case KEY_ESC: return ouzel::input::Keyboard::Key::ESCAPE;
        case KEY_1: return ouzel::input::Keyboard::Key::NUM_1;
        case KEY_2: return ouzel::input::Keyboard::Key::NUM_2;
        case KEY_3: return ouzel::input::Keyboard::Key::NUM_3;
        case KEY_4: return ouzel::input::Keyboard::Key::NUM_4;
        case KEY_5: return ouzel::input::Keyboard::Key::NUM_5;
        case KEY_6: return ouzel::input::Keyboard::Key::NUM_6;
        case KEY_7: return ouzel::input::Keyboard::Key::NUM_7;
        case KEY_8: return ouzel::input::Keyboard::Key::NUM_8;
        case KEY_9: return ouzel::input::Keyboard::Key::NUM_9;
        case KEY_0: return ouzel::input::Keyboard::Key::NUM_0;
        case KEY_MINUS: return ouzel::input::Keyboard::Key::MINUS;
        case KEY_EQUAL: return ouzel::input::Keyboard::Key::EQUAL;
        case KEY_BACKSPACE: return ouzel::input::Keyboard::Key::BACKSPACE;
        case KEY_TAB: return ouzel::input::Keyboard::Key::TAB;
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
        case KEY_LEFTBRACE: return ouzel::input::Keyboard::Key::LEFT_BRACKET;
        case KEY_RIGHTBRACE: return ouzel::input::Keyboard::Key::RIGHT_BRACKET;
        case KEY_ENTER: return ouzel::input::Keyboard::Key::ENTER;
        case KEY_LEFTCTRL: return ouzel::input::Keyboard::Key::LEFT_CONTROL;
        case KEY_A: return ouzel::input::Keyboard::Key::A;
        case KEY_S: return ouzel::input::Keyboard::Key::S;
        case KEY_D: return ouzel::input::Keyboard::Key::D;
        case KEY_F: return ouzel::input::Keyboard::Key::F;
        case KEY_G: return ouzel::input::Keyboard::Key::G;
        case KEY_H: return ouzel::input::Keyboard::Key::H;
        case KEY_J: return ouzel::input::Keyboard::Key::J;
        case KEY_K: return ouzel::input::Keyboard::Key::K;
        case KEY_L: return ouzel::input::Keyboard::Key::L;
        case KEY_SEMICOLON: return ouzel::input::Keyboard::Key::SEMICOLON;
        case KEY_APOSTROPHE: return ouzel::input::Keyboard::Key::QUOTE;
        case KEY_GRAVE: return ouzel::input::Keyboard::Key::GRAVE;
        case KEY_LEFTSHIFT: return ouzel::input::Keyboard::Key::LEFT_SHIFT;
        case KEY_BACKSLASH: return ouzel::input::Keyboard::Key::BACKSLASH;
        case KEY_Z: return ouzel::input::Keyboard::Key::Z;
        case KEY_X: return ouzel::input::Keyboard::Key::X;
        case KEY_C: return ouzel::input::Keyboard::Key::C;
        case KEY_V: return ouzel::input::Keyboard::Key::V;
        case KEY_B: return ouzel::input::Keyboard::Key::B;
        case KEY_N: return ouzel::input::Keyboard::Key::N;
        case KEY_M: return ouzel::input::Keyboard::Key::M;
        case KEY_COMMA: return ouzel::input::Keyboard::Key::COMMA;
        case KEY_DOT: return ouzel::input::Keyboard::Key::PERIOD;
        case KEY_SLASH: return ouzel::input::Keyboard::Key::SLASH;
        case KEY_RIGHTSHIFT: return ouzel::input::Keyboard::Key::RIGHT_SHIFT;
        case KEY_KPASTERISK: return ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY;
        case KEY_LEFTALT: return ouzel::input::Keyboard::Key::LEFT_ALT;
        case KEY_SPACE: return ouzel::input::Keyboard::Key::SPACE;
        case KEY_CAPSLOCK: return ouzel::input::Keyboard::Key::CAPS_LOCK;
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
        case KEY_NUMLOCK: return ouzel::input::Keyboard::Key::NUM_LOCK;
        case KEY_SCROLLLOCK: return ouzel::input::Keyboard::Key::SCROLL_LOCK;
        case KEY_KP7: return ouzel::input::Keyboard::Key::NUMPAD_7;
        case KEY_KP8: return ouzel::input::Keyboard::Key::NUMPAD_8;
        case KEY_KP9: return ouzel::input::Keyboard::Key::NUMPAD_9;
        case KEY_KPMINUS: return ouzel::input::Keyboard::Key::NUMPAD_MINUS;
        case KEY_KP4: return ouzel::input::Keyboard::Key::NUMPAD_4;
        case KEY_KP5: return ouzel::input::Keyboard::Key::NUMPAD_5;
        case KEY_KP6: return ouzel::input::Keyboard::Key::NUMPAD_6;
        case KEY_KPPLUS: return ouzel::input::Keyboard::Key::NUMPAD_PLUS;
        case KEY_KP1: return ouzel::input::Keyboard::Key::NUMPAD_1;
        case KEY_KP2: return ouzel::input::Keyboard::Key::NUMPAD_2;
        case KEY_KP3: return ouzel::input::Keyboard::Key::NUMPAD_3;
        case KEY_KP0: return ouzel::input::Keyboard::Key::NUMPAD_0;
        case KEY_KPDOT: return ouzel::input::Keyboard::Key::NUMPAD_DECIMAL;

        case KEY_ZENKAKUHANKAKU: return ouzel::input::Keyboard::Key::NONE; // ??
        case KEY_102ND: return ouzel::input::Keyboard::Key::INTL_BACKSLASH;
        case KEY_F11: return ouzel::input::Keyboard::Key::F11;
        case KEY_F12: return ouzel::input::Keyboard::Key::F12;
        case KEY_RO: return ouzel::input::Keyboard::Key::RO;
        case KEY_KATAKANA: return ouzel::input::Keyboard::Key::KATAKANA;
        case KEY_HIRAGANA: return ouzel::input::Keyboard::Key::HIRAGANA;
        case KEY_HENKAN: return ouzel::input::Keyboard::Key::HENKAN;
        case KEY_KATAKANAHIRAGANA: return ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA;
        case KEY_MUHENKAN: return ouzel::input::Keyboard::Key::MUHENKAN;
        case KEY_KPJPCOMMA: return ouzel::input::Keyboard::Key::NUMPAD_JPCOMMA;
        case KEY_KPENTER: return ouzel::input::Keyboard::Key::NUMPAD_ENTER;
        case KEY_RIGHTCTRL: return ouzel::input::Keyboard::Key::RIGHT_CONTROL;
        case KEY_KPSLASH: return ouzel::input::Keyboard::Key::NUMPAD_DIVIDE;
        case KEY_SYSRQ: return ouzel::input::Keyboard::Key::PRINT_SCREEN;
        case KEY_RIGHTALT: return ouzel::input::Keyboard::Key::RIGHT_ALT;
        case KEY_LINEFEED: return ouzel::input::Keyboard::Key::NONE; // ??
        case KEY_HOME: return ouzel::input::Keyboard::Key::HOME;
        case KEY_UP: return ouzel::input::Keyboard::Key::UP;
        case KEY_PAGEUP: return ouzel::input::Keyboard::Key::PAGE_UP;
        case KEY_LEFT: return ouzel::input::Keyboard::Key::LEFT;
        case KEY_RIGHT: return ouzel::input::Keyboard::Key::RIGHT;
        case KEY_END: return ouzel::input::Keyboard::Key::END;
        case KEY_DOWN: return ouzel::input::Keyboard::Key::DOWN;
        case KEY_PAGEDOWN: return ouzel::input::Keyboard::Key::PAGE_DOWN;
        case KEY_INSERT: return ouzel::input::Keyboard::Key::INSERT;
        case KEY_DELETE: return ouzel::input::Keyboard::Key::DEL;
        case KEY_MACRO: return ouzel::input::Keyboard::Key::NONE; // ??
        case KEY_MAIL: return ouzel::input::Keyboard::Key::MAIL;
        case KEY_MUTE: return ouzel::input::Keyboard::Key::MUTE;
        case KEY_VOLUMEDOWN: return ouzel::input::Keyboard::Key::VOLUME_DOWN;
        case KEY_VOLUMEUP: return ouzel::input::Keyboard::Key::VOLUME_UP;
        case KEY_NEXTSONG: return ouzel::input::Keyboard::Key::AUDIO_NEXT;
        case KEY_PLAYPAUSE: return ouzel::input::Keyboard::Key::AUDIO_PLAY;
        case KEY_PREVIOUSSONG: return ouzel::input::Keyboard::Key::AUDIO_PREVIOUS;
        case KEY_STOPCD: return ouzel::input::Keyboard::Key::AUDIO_STOP;

        case KEY_POWER: return ouzel::input::Keyboard::Key::POWER;
        case KEY_KPEQUAL: return ouzel::input::Keyboard::Key::NUMPAD_EQUAL;
        case KEY_KPPLUSMINUS: return ouzel::input::Keyboard::Key::NUMPAD_PLUSMINUS;
        case KEY_PAUSE: return ouzel::input::Keyboard::Key::PAUSE;
        case KEY_SCALE: return ouzel::input::Keyboard::Key::NONE; //?

        case KEY_KPCOMMA: return ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR;
        case KEY_HANGEUL: return ouzel::input::Keyboard::Key::KANA;
        case KEY_HANJA: return ouzel::input::Keyboard::Key::HANJA;
        case KEY_YEN: return ouzel::input::Keyboard::Key::YEN;
        case KEY_LEFTMETA: return ouzel::input::Keyboard::Key::LEFT_SUPER;
        case KEY_RIGHTMETA: return ouzel::input::Keyboard::Key::RIGHT_SUPER;
        case KEY_COMPOSE: return ouzel::input::Keyboard::Key::NONE; // ??

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

        case KEY_MENU: return ouzel::input::Keyboard::Key::MENU;
        case KEY_CLEAR: return ouzel::input::Keyboard::Key::CLEAR;
        case KEY_SLEEP: return ouzel::input::Keyboard::Key::SLEEP;
        case KEY_PROG1: return ouzel::input::Keyboard::Key::APP1;
        case KEY_PROG2: return ouzel::input::Keyboard::Key::APP2;
        case KEY_HELP: return ouzel::input::Keyboard::Key::HELP;

        case KEY_BACK: return ouzel::input::Keyboard::Key::BACK;
        case KEY_FORWARD: return ouzel::input::Keyboard::Key::FORWARD;
        case KEY_REFRESH: return ouzel::input::Keyboard::Key::REFRESH;
        case KEY_STOP: return ouzel::input::Keyboard::Key::STOP;
        case KEY_FIND: return ouzel::input::Keyboard::Key::SEARCH;
        case KEY_BOOKMARKS: return ouzel::input::Keyboard::Key::BOOKMARKS;
        case KEY_HOMEPAGE: return ouzel::input::Keyboard::Key::HOMEPAGE;
        case KEY_MODE: return ouzel::input::Keyboard::Key::MODE_CHANGE;
        default: return ouzel::input::Keyboard::Key::NONE;
    }
}

static ouzel::input::Mouse::Button convertButtonCode(uint16_t buttonCode)
{
    switch (buttonCode)
    {
        case BTN_LEFT: return ouzel::input::Mouse::Button::LEFT;
        case BTN_RIGHT: return ouzel::input::Mouse::Button::RIGHT;
        case BTN_MIDDLE: return ouzel::input::Mouse::Button::MIDDLE;
        case BTN_SIDE: return ouzel::input::Mouse::Button::X1;
        case BTN_EXTRA: return ouzel::input::Mouse::Button::X2;
        case BTN_FORWARD: return ouzel::input::Mouse::Button::NONE; //?
        case BTN_BACK: return ouzel::input::Mouse::Button::NONE; //?
        case BTN_TASK: return ouzel::input::Mouse::Button::NONE; //?
        default: return ouzel::input::Mouse::Button::NONE;
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
                engine->log(Log::Level::WARN) << "Failed to grab device";

            char deviceName[256];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName) - 1), deviceName) == -1)
                engine->log(Log::Level::WARN) << "Failed to get device name";
            else
            {
                name = deviceName;
                engine->log(Log::Level::INFO) << "Got device: " << name;
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
                keyboardDevice.reset(new KeyboardDevice(inputSystem, inputSystem.getNextDeviceId()));

            if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
            {
                if ((isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN)) || // tablet
                    (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN)) || // touchpad
                    isBitSet(keyBits, BTN_TOUCH)) // touchscreen
                {
                    touchpadDevice.reset(new TouchpadDevice(inputSystem, inputSystem.getNextDeviceId(), false));

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
                    throw std::system_error(errno, std::system_category(), "Failed to get device info");

                const GamepadConfig& gamepadConfig = getGamepadConfig(id.vendor, id.product);

                for (size_t i = 0; i < 24; ++i)
                {
                    if (gamepadConfig.buttonMap[i] != Gamepad::Button::NONE)
                    {
                        Button button;
                        button.button = gamepadConfig.buttonMap[i];
                        buttons.insert(std::make_pair(BTN_GAMEPAD + i, button));
                    }
                }

                static constexpr std::array<uint32_t, 6> axisUsageMap = {
                    ABS_X,
                    ABS_Y,
                    ABS_Z,
                    ABS_RX,
                    ABS_RY,
                    ABS_RZ
                };

                for (size_t i = 0; i < 6; ++i)
                {
                    if (gamepadConfig.axisMap[i] != Gamepad::Axis::NONE)
                    {
                        uint32_t usage = axisUsageMap[i];

                        Axis axis;
                        axis.axis = gamepadConfig.axisMap[i];

                        input_absinfo info;

                        if (ioctl(fd, EVIOCGABS(usage), &info) == -1)
                            throw std::system_error(errno, std::system_category(), "Failed to get device info");

                        axis.min = info.minimum;
                        axis.max = info.maximum;
                        axis.range = info.maximum - info.minimum;

                        switch (gamepadConfig.axisMap[i])
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
                    engine->log(Log::Level::WARN) << "Failed to release device";

                close(fd);
            }
        }

        void EventDevice::update()
        {
            input_event events[32];
            ssize_t bytesRead = read(fd, events, sizeof(events));

            if (bytesRead == -1)
                throw std::system_error(errno, std::system_category(), "Failed to read from " + filename);

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

                            Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
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
                                    Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(event.value), 0.0F));
                                    mouseDevice->handleRelativeMove(normalizedPosition);
                                    break;
                                }
                                case REL_Y:
                                {
                                    Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(0.0F, static_cast<float>(event.value)));
                                    mouseDevice->handleRelativeMove(normalizedPosition);
                                    break;
                                }
                                case REL_WHEEL:
                                {
                                    Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                  static_cast<float>(cursorPosition.y)));
                                    mouseDevice->handleScroll(Vector2F(0.0F, static_cast<float>(event.value)), normalizedPosition);
                                    break;
                                }
                                case REL_HWHEEL:
                                {
                                    Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
                                                                                                                                  static_cast<float>(cursorPosition.y)));
                                    mouseDevice->handleScroll(Vector2F(static_cast<float>(event.value), 0.0F), normalizedPosition);
                                    break;
                                }
                            }
                            break;
                        }
                        case EV_KEY:
                        {
                            Vector2F normalizedPosition = engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(cursorPosition.x),
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
                                            Vector2F position(static_cast<float>(slot.positionX - touchMinX) / touchRangeX,
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
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

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
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

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
                                        throw std::system_error(errno, std::system_category(), "Failed to get device info");

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

                                if ((button.button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                                    (button.button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
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

        void EventDevice::handleAxisChange(int32_t oldValue, int32_t newValue,
                                           int32_t min, int32_t range,
                                           Gamepad::Button negativeButton,
                                           Gamepad::Button positiveButton)
        {
            if (negativeButton == positiveButton)
            {
                float floatValue = static_cast<float>(newValue - min) / range;

                gamepadDevice->handleButtonValueChange(negativeButton,
                                                       floatValue > 0.0F,
                                                       floatValue);
            }
            else
            {
                float floatValue = 2.0F * (newValue - min) / range - 1.0F;

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
