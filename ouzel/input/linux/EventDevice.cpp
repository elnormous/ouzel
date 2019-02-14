// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

static const std::unordered_map<uint16_t, ouzel::input::Keyboard::Key> keyMap = {
    {KEY_ESC, ouzel::input::Keyboard::Key::ESCAPE},
    {KEY_1, ouzel::input::Keyboard::Key::NUM_1},
    {KEY_2, ouzel::input::Keyboard::Key::NUM_2},
    {KEY_3, ouzel::input::Keyboard::Key::NUM_3},
    {KEY_4, ouzel::input::Keyboard::Key::NUM_4},
    {KEY_5, ouzel::input::Keyboard::Key::NUM_5},
    {KEY_6, ouzel::input::Keyboard::Key::NUM_6},
    {KEY_7, ouzel::input::Keyboard::Key::NUM_7},
    {KEY_8, ouzel::input::Keyboard::Key::NUM_8},
    {KEY_9, ouzel::input::Keyboard::Key::NUM_9},
    {KEY_0, ouzel::input::Keyboard::Key::NUM_0},
    {KEY_MINUS, ouzel::input::Keyboard::Key::MINUS},
    {KEY_EQUAL, ouzel::input::Keyboard::Key::EQUAL},
    {KEY_BACKSPACE, ouzel::input::Keyboard::Key::BACKSPACE},
    {KEY_TAB, ouzel::input::Keyboard::Key::TAB},
    {KEY_Q, ouzel::input::Keyboard::Key::Q},
    {KEY_W, ouzel::input::Keyboard::Key::W},
    {KEY_E, ouzel::input::Keyboard::Key::E},
    {KEY_R, ouzel::input::Keyboard::Key::R},
    {KEY_T, ouzel::input::Keyboard::Key::T},
    {KEY_Y, ouzel::input::Keyboard::Key::Y},
    {KEY_U, ouzel::input::Keyboard::Key::U},
    {KEY_I, ouzel::input::Keyboard::Key::I},
    {KEY_O, ouzel::input::Keyboard::Key::O},
    {KEY_P, ouzel::input::Keyboard::Key::P},
    {KEY_LEFTBRACE, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {KEY_RIGHTBRACE, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {KEY_ENTER, ouzel::input::Keyboard::Key::ENTER},
    {KEY_LEFTCTRL, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {KEY_A, ouzel::input::Keyboard::Key::A},
    {KEY_S, ouzel::input::Keyboard::Key::S},
    {KEY_D, ouzel::input::Keyboard::Key::D},
    {KEY_F, ouzel::input::Keyboard::Key::F},
    {KEY_G, ouzel::input::Keyboard::Key::G},
    {KEY_H, ouzel::input::Keyboard::Key::H},
    {KEY_J, ouzel::input::Keyboard::Key::J},
    {KEY_K, ouzel::input::Keyboard::Key::K},
    {KEY_L, ouzel::input::Keyboard::Key::L},
    {KEY_SEMICOLON, ouzel::input::Keyboard::Key::SEMICOLON},
    {KEY_APOSTROPHE, ouzel::input::Keyboard::Key::QUOTE},
    {KEY_GRAVE, ouzel::input::Keyboard::Key::GRAVE},
    {KEY_LEFTSHIFT, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {KEY_BACKSLASH, ouzel::input::Keyboard::Key::BACKSLASH},
    {KEY_Z, ouzel::input::Keyboard::Key::Z},
    {KEY_X, ouzel::input::Keyboard::Key::X},
    {KEY_C, ouzel::input::Keyboard::Key::C},
    {KEY_V, ouzel::input::Keyboard::Key::V},
    {KEY_B, ouzel::input::Keyboard::Key::B},
    {KEY_N, ouzel::input::Keyboard::Key::N},
    {KEY_M, ouzel::input::Keyboard::Key::M},
    {KEY_COMMA, ouzel::input::Keyboard::Key::COMMA},
    {KEY_DOT, ouzel::input::Keyboard::Key::PERIOD},
    {KEY_SLASH, ouzel::input::Keyboard::Key::SLASH},
    {KEY_RIGHTSHIFT, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {KEY_KPASTERISK, ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {KEY_LEFTALT, ouzel::input::Keyboard::Key::LEFT_ALT},
    {KEY_SPACE, ouzel::input::Keyboard::Key::SPACE},
    {KEY_CAPSLOCK, ouzel::input::Keyboard::Key::CAPS_LOCK},
    {KEY_F1, ouzel::input::Keyboard::Key::F1},
    {KEY_F2, ouzel::input::Keyboard::Key::F2},
    {KEY_F3, ouzel::input::Keyboard::Key::F3},
    {KEY_F4, ouzel::input::Keyboard::Key::F4},
    {KEY_F5, ouzel::input::Keyboard::Key::F5},
    {KEY_F6, ouzel::input::Keyboard::Key::F6},
    {KEY_F7, ouzel::input::Keyboard::Key::F7},
    {KEY_F8, ouzel::input::Keyboard::Key::F8},
    {KEY_F9, ouzel::input::Keyboard::Key::F9},
    {KEY_F10, ouzel::input::Keyboard::Key::F10},
    {KEY_NUMLOCK, ouzel::input::Keyboard::Key::NUM_LOCK},
    {KEY_SCROLLLOCK, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {KEY_KP7, ouzel::input::Keyboard::Key::NUMPAD_7},
    {KEY_KP8, ouzel::input::Keyboard::Key::NUMPAD_8},
    {KEY_KP9, ouzel::input::Keyboard::Key::NUMPAD_9},
    {KEY_KPMINUS, ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {KEY_KP4, ouzel::input::Keyboard::Key::NUMPAD_4},
    {KEY_KP5, ouzel::input::Keyboard::Key::NUMPAD_5},
    {KEY_KP6, ouzel::input::Keyboard::Key::NUMPAD_6},
    {KEY_KPPLUS, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {KEY_KP1, ouzel::input::Keyboard::Key::NUMPAD_1},
    {KEY_KP2, ouzel::input::Keyboard::Key::NUMPAD_2},
    {KEY_KP3, ouzel::input::Keyboard::Key::NUMPAD_3},
    {KEY_KP0, ouzel::input::Keyboard::Key::NUMPAD_0},
    {KEY_KPDOT, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},

    {KEY_ZENKAKUHANKAKU, ouzel::input::Keyboard::Key::NONE}, // ??
    {KEY_102ND, ouzel::input::Keyboard::Key::INTL_BACKSLASH},
    {KEY_F11, ouzel::input::Keyboard::Key::F11},
    {KEY_F12, ouzel::input::Keyboard::Key::F12},
    {KEY_RO, ouzel::input::Keyboard::Key::RO},
    {KEY_KATAKANA, ouzel::input::Keyboard::Key::KATAKANA},
    {KEY_HIRAGANA, ouzel::input::Keyboard::Key::HIRAGANA},
    {KEY_HENKAN, ouzel::input::Keyboard::Key::HENKAN},
    {KEY_KATAKANAHIRAGANA, ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA},
    {KEY_MUHENKAN, ouzel::input::Keyboard::Key::MUHENKAN},
    {KEY_KPJPCOMMA, ouzel::input::Keyboard::Key::NUMPAD_JPCOMMA},
    {KEY_KPENTER, ouzel::input::Keyboard::Key::NUMPAD_ENTER},
    {KEY_RIGHTCTRL, ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {KEY_KPSLASH, ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},
    {KEY_SYSRQ, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {KEY_RIGHTALT, ouzel::input::Keyboard::Key::RIGHT_ALT},
    {KEY_LINEFEED, ouzel::input::Keyboard::Key::NONE}, // ??
    {KEY_HOME, ouzel::input::Keyboard::Key::HOME},
    {KEY_UP, ouzel::input::Keyboard::Key::UP},
    {KEY_PAGEUP, ouzel::input::Keyboard::Key::PAGE_UP},
    {KEY_LEFT, ouzel::input::Keyboard::Key::LEFT},
    {KEY_RIGHT, ouzel::input::Keyboard::Key::RIGHT},
    {KEY_END, ouzel::input::Keyboard::Key::END},
    {KEY_DOWN, ouzel::input::Keyboard::Key::DOWN},
    {KEY_PAGEDOWN, ouzel::input::Keyboard::Key::PAGE_DOWN},
    {KEY_INSERT, ouzel::input::Keyboard::Key::INSERT},
    {KEY_DELETE, ouzel::input::Keyboard::Key::DEL},
    {KEY_MACRO, ouzel::input::Keyboard::Key::NONE}, // ??
    {KEY_MAIL, ouzel::input::Keyboard::Key::MAIL},
    {KEY_MUTE, ouzel::input::Keyboard::Key::MUTE},
    {KEY_VOLUMEDOWN, ouzel::input::Keyboard::Key::VOLUME_DOWN},
    {KEY_VOLUMEUP, ouzel::input::Keyboard::Key::VOLUME_UP},
    {KEY_NEXTSONG, ouzel::input::Keyboard::Key::AUDIO_NEXT},
    {KEY_PLAYPAUSE, ouzel::input::Keyboard::Key::AUDIO_PLAY},
    {KEY_PREVIOUSSONG, ouzel::input::Keyboard::Key::AUDIO_PREVIOUS},
    {KEY_STOPCD, ouzel::input::Keyboard::Key::AUDIO_STOP},

    {KEY_POWER, ouzel::input::Keyboard::Key::POWER},
    {KEY_KPEQUAL, ouzel::input::Keyboard::Key::NUMPAD_EQUAL},
    {KEY_KPPLUSMINUS, ouzel::input::Keyboard::Key::NUMPAD_PLUSMINUS},
    {KEY_PAUSE, ouzel::input::Keyboard::Key::PAUSE},
    {KEY_SCALE, ouzel::input::Keyboard::Key::NONE}, //?

    {KEY_KPCOMMA, ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR},
    {KEY_HANGEUL, ouzel::input::Keyboard::Key::KANA},
    {KEY_HANJA, ouzel::input::Keyboard::Key::HANJA},
    {KEY_YEN, ouzel::input::Keyboard::Key::YEN},
    {KEY_LEFTMETA, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {KEY_RIGHTMETA, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {KEY_COMPOSE, ouzel::input::Keyboard::Key::NONE}, // ??

    {KEY_F13, ouzel::input::Keyboard::Key::F13},
    {KEY_F14, ouzel::input::Keyboard::Key::F14},
    {KEY_F15, ouzel::input::Keyboard::Key::F15},
    {KEY_F16, ouzel::input::Keyboard::Key::F16},
    {KEY_F17, ouzel::input::Keyboard::Key::F17},
    {KEY_F18, ouzel::input::Keyboard::Key::F18},
    {KEY_F19, ouzel::input::Keyboard::Key::F19},
    {KEY_F20, ouzel::input::Keyboard::Key::F20},
    {KEY_F21, ouzel::input::Keyboard::Key::F21},
    {KEY_F22, ouzel::input::Keyboard::Key::F22},
    {KEY_F23, ouzel::input::Keyboard::Key::F23},
    {KEY_F24, ouzel::input::Keyboard::Key::F24},

    {KEY_MENU, ouzel::input::Keyboard::Key::MENU},
    {KEY_CLEAR, ouzel::input::Keyboard::Key::CLEAR},
    {KEY_SLEEP, ouzel::input::Keyboard::Key::SLEEP},
    {KEY_PROG1, ouzel::input::Keyboard::Key::APP1},
    {KEY_PROG2, ouzel::input::Keyboard::Key::APP2},
    {KEY_HELP, ouzel::input::Keyboard::Key::HELP},

    {KEY_BACK, ouzel::input::Keyboard::Key::BACK},
    {KEY_FORWARD, ouzel::input::Keyboard::Key::FORWARD},
    {KEY_REFRESH, ouzel::input::Keyboard::Key::REFRESH},
    {KEY_STOP, ouzel::input::Keyboard::Key::STOP},
    {KEY_FIND, ouzel::input::Keyboard::Key::SEARCH},
    {KEY_BOOKMARKS, ouzel::input::Keyboard::Key::BOOKMARKS},
    {KEY_HOMEPAGE, ouzel::input::Keyboard::Key::HOMEPAGE},
    {KEY_MODE, ouzel::input::Keyboard::Key::MODE_CHANGE}
};

static const std::unordered_map<uint16_t, ouzel::input::Mouse::Button> buttonMap = {
    {BTN_LEFT, ouzel::input::Mouse::Button::LEFT},
    {BTN_RIGHT, ouzel::input::Mouse::Button::RIGHT},
    {BTN_MIDDLE, ouzel::input::Mouse::Button::MIDDLE},
    {BTN_SIDE, ouzel::input::Mouse::Button::X1},
    {BTN_EXTRA, ouzel::input::Mouse::Button::X2},
    {BTN_FORWARD, ouzel::input::Mouse::Button::NONE}, //?
    {BTN_BACK, ouzel::input::Mouse::Button::NONE}, //?
    {BTN_TASK, ouzel::input::Mouse::Button::NONE} //?
};

static ouzel::input::Keyboard::Key convertKeyCode(uint16_t keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

static ouzel::input::Mouse::Button convertButtonCode(uint16_t buttonCode)
{
    auto i = buttonMap.find(buttonCode);

    if (i != buttonMap.end())
        return i->second;
    else
        return ouzel::input::Mouse::Button::NONE;
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
