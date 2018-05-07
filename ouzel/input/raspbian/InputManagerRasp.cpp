// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <unordered_map>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "InputManagerRasp.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "utils/Log.hpp"

static char TEMP_BUFFER[256];

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<uint16_t, KeyboardKey> keyMap = {
            {KEY_ESC, KeyboardKey::ESCAPE},
            {KEY_1, KeyboardKey::NUM_1},
            {KEY_2, KeyboardKey::NUM_2},
            {KEY_3, KeyboardKey::NUM_3},
            {KEY_4, KeyboardKey::NUM_4},
            {KEY_5, KeyboardKey::NUM_5},
            {KEY_6, KeyboardKey::NUM_6},
            {KEY_7, KeyboardKey::NUM_7},
            {KEY_8, KeyboardKey::NUM_8},
            {KEY_9, KeyboardKey::NUM_9},
            {KEY_0, KeyboardKey::NUM_0},
            {KEY_MINUS, KeyboardKey::MINUS},
            {KEY_EQUAL, KeyboardKey::PLUS},
            {KEY_BACKSPACE, KeyboardKey::BACKSPACE},
            {KEY_TAB, KeyboardKey::TAB},
            {KEY_Q, KeyboardKey::Q},
            {KEY_W, KeyboardKey::W},
            {KEY_E, KeyboardKey::E},
            {KEY_R, KeyboardKey::R},
            {KEY_T, KeyboardKey::T},
            {KEY_Y, KeyboardKey::Y},
            {KEY_U, KeyboardKey::U},
            {KEY_I, KeyboardKey::I},
            {KEY_O, KeyboardKey::O},
            {KEY_P, KeyboardKey::P},
            {KEY_LEFTBRACE, KeyboardKey::LEFT_BRACKET},
            {KEY_RIGHTBRACE, KeyboardKey::RIGHT_BRACKET},
            {KEY_ENTER, KeyboardKey::RETURN},
            {KEY_LEFTCTRL, KeyboardKey::LEFT_CONTROL},
            {KEY_A, KeyboardKey::A},
            {KEY_S, KeyboardKey::S},
            {KEY_D, KeyboardKey::D},
            {KEY_F, KeyboardKey::F},
            {KEY_G, KeyboardKey::G},
            {KEY_H, KeyboardKey::H},
            {KEY_J, KeyboardKey::J},
            {KEY_K, KeyboardKey::K},
            {KEY_L, KeyboardKey::L},
            {KEY_SEMICOLON, KeyboardKey::SEMICOLON},
            {KEY_APOSTROPHE, KeyboardKey::QUOTE},
            {KEY_GRAVE, KeyboardKey::GRAVE},
            {KEY_LEFTSHIFT, KeyboardKey::LEFT_SHIFT},
            {KEY_BACKSLASH, KeyboardKey::BACKSLASH},
            {KEY_Z, KeyboardKey::Z},
            {KEY_X, KeyboardKey::X},
            {KEY_C, KeyboardKey::C},
            {KEY_V, KeyboardKey::V},
            {KEY_B, KeyboardKey::B},
            {KEY_N, KeyboardKey::N},
            {KEY_M, KeyboardKey::M},
            {KEY_COMMA, KeyboardKey::COMMA},
            {KEY_DOT, KeyboardKey::PERIOD},
            {KEY_SLASH, KeyboardKey::SLASH},
            {KEY_RIGHTSHIFT, KeyboardKey::RIGHT_SHIFT},
            {KEY_KPASTERISK, KeyboardKey::MULTIPLY},
            {KEY_LEFTALT, KeyboardKey::LEFT_ALT},
            {KEY_SPACE, KeyboardKey::SPACE},
            {KEY_CAPSLOCK, KeyboardKey::CAPITAL},
            {KEY_F1, KeyboardKey::F1},
            {KEY_F2, KeyboardKey::F2},
            {KEY_F3, KeyboardKey::F3},
            {KEY_F4, KeyboardKey::F4},
            {KEY_F5, KeyboardKey::F5},
            {KEY_F6, KeyboardKey::F6},
            {KEY_F7, KeyboardKey::F7},
            {KEY_F8, KeyboardKey::F8},
            {KEY_F9, KeyboardKey::F9},
            {KEY_F10, KeyboardKey::F10},
            {KEY_NUMLOCK, KeyboardKey::NUMLOCK},
            {KEY_SCROLLLOCK, KeyboardKey::SCROLL},
            {KEY_KP7, KeyboardKey::NUMPAD_7},
            {KEY_KP8, KeyboardKey::NUMPAD_8},
            {KEY_KP9, KeyboardKey::NUMPAD_9},
            {KEY_KPMINUS, KeyboardKey::SUBTRACT},
            {KEY_KP4, KeyboardKey::NUMPAD_4},
            {KEY_KP5, KeyboardKey::NUMPAD_5},
            {KEY_KP6, KeyboardKey::NUMPAD_6},
            {KEY_KPPLUS, KeyboardKey::ADD},
            {KEY_KP1, KeyboardKey::NUMPAD_1},
            {KEY_KP2, KeyboardKey::NUMPAD_2},
            {KEY_KP3, KeyboardKey::NUMPAD_3},
            {KEY_KP0, KeyboardKey::NUMPAD_0},
            {KEY_KPDOT, KeyboardKey::DECIMAL},

            {KEY_ZENKAKUHANKAKU, KeyboardKey::NONE}, // ??
            {KEY_102ND, KeyboardKey::LESS},
            {KEY_F11, KeyboardKey::F11},
            {KEY_F12, KeyboardKey::F12},
            {KEY_RO, KeyboardKey::NONE}, // ??
            {KEY_KATAKANA, KeyboardKey::NONE}, // ??
            {KEY_HIRAGANA, KeyboardKey::NONE}, // ??
            {KEY_HENKAN, KeyboardKey::NONE}, // ??
            {KEY_KATAKANAHIRAGANA, KeyboardKey::NONE}, // ??
            {KEY_MUHENKAN, KeyboardKey::NONE}, // ??
            {KEY_KPJPCOMMA, KeyboardKey::NONE}, // ??
            {KEY_KPENTER, KeyboardKey::RETURN},
            {KEY_RIGHTCTRL, KeyboardKey::RIGHT_CONTROL},
            {KEY_KPSLASH, KeyboardKey::DIVIDE},
            {KEY_SYSRQ, KeyboardKey::NONE}, // ??
            {KEY_RIGHTALT, KeyboardKey::RIGHT_ALT},
            {KEY_LINEFEED, KeyboardKey::NONE}, // ??
            {KEY_HOME, KeyboardKey::HOME},
            {KEY_UP, KeyboardKey::UP},
            {KEY_PAGEUP, KeyboardKey::PRIOR},
            {KEY_LEFT, KeyboardKey::LEFT},
            {KEY_RIGHT, KeyboardKey::RIGHT},
            {KEY_END, KeyboardKey::END},
            {KEY_DOWN, KeyboardKey::DOWN},
            {KEY_PAGEDOWN, KeyboardKey::NEXT},
            {KEY_INSERT, KeyboardKey::INSERT},
            {KEY_DELETE, KeyboardKey::DEL},
            {KEY_MACRO, KeyboardKey::NONE}, // ??
            {KEY_MUTE, KeyboardKey::NONE}, // ??
            {KEY_VOLUMEDOWN, KeyboardKey::NONE}, // ??
            {KEY_VOLUMEUP, KeyboardKey::NONE}, // ??
            {KEY_POWER, KeyboardKey::NONE}, // ??
            {KEY_KPEQUAL, KeyboardKey::EQUAL},
            {KEY_KPPLUSMINUS, KeyboardKey::NONE}, // ??
            {KEY_PAUSE, KeyboardKey::PAUSE},
            {KEY_SCALE, KeyboardKey::NONE}, //?

            {KEY_KPCOMMA, KeyboardKey::SEPARATOR},
            {KEY_HANGEUL, KeyboardKey::NONE}, // KEY_HANGUEL
            {KEY_HANJA, KeyboardKey::HANJA},
            {KEY_YEN, KeyboardKey::NONE}, // ??
            {KEY_LEFTMETA, KeyboardKey::LEFT_SUPER},
            {KEY_RIGHTMETA, KeyboardKey::RIGHT_SUPER},
            {KEY_COMPOSE, KeyboardKey::NONE}, // ??

            {KEY_F13, KeyboardKey::F13},
            {KEY_F14, KeyboardKey::F14},
            {KEY_F15, KeyboardKey::F15},
            {KEY_F16, KeyboardKey::F16},
            {KEY_F17, KeyboardKey::F17},
            {KEY_F18, KeyboardKey::F18},
            {KEY_F19, KeyboardKey::F19},
            {KEY_F20, KeyboardKey::F20},
            {KEY_F21, KeyboardKey::F21},
            {KEY_F22, KeyboardKey::F22},
            {KEY_F23, KeyboardKey::F23},
            {KEY_F24, KeyboardKey::F24}
        };

        static KeyboardKey convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
            {
                return i->second;
            }
            else
            {
                return KeyboardKey::NONE;
            }
        }

        InputManagerRasp::InputManagerRasp()
        {
        }

        bool InputManagerRasp::init()
        {
            std::fill(std::begin(keyboardKeyDown), std::end(keyboardKeyDown), false);
            std::fill(std::begin(mouseButtonDown), std::end(mouseButtonDown), false);

            DIR* dir = opendir("/dev/input");

            if (!dir)
            {
                Log(Log::Level::ERR) << "Failed to open directory";
                return false;
            }

            dirent ent;
            dirent* p;

            while (readdir_r(dir, &ent, &p) == 0 && p)
            {
                if (strncmp("event", ent.d_name, 5) == 0)
                {
                    EventDevice inputDevice(std::string("/dev/input/") + ent.d_name);

                    if (inputDevice.getDeviceClass() != EventDevice::CLASS_NONE)
                    {
                        if (inputDevice.getFd() > maxFd) maxFd = inputDevice.getFd();

                        inputDevices.push_back(std::move(inputDevice));
                    }
                }
            }

            closedir(dir);

            return true;
        }

        void InputManagerRasp::update()
        {
            fd_set rfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);

            for (const EventDevice& inputDevice : inputDevices)
            {
                FD_SET(inputDevice.getFd(), &rfds);
            }

            int retval = select(maxFd + 1, &rfds, nullptr, nullptr, &tv);

            if (retval == -1)
            {
                Log(Log::Level::ERR) << "Select failed";
            }
            else if (retval > 0)
            {
                for (const EventDevice& inputDevice : inputDevices)
                {
                    if (FD_ISSET(inputDevice.getFd(), &rfds))
                    {
                        ssize_t bytesRead = read(inputDevice.getFd(), TEMP_BUFFER, sizeof(TEMP_BUFFER));

                        if (bytesRead == -1)
                        {
                            continue;
                        }

                        for (ssize_t i = 0; i < bytesRead - static_cast<ssize_t>(sizeof(input_event)) + 1; i += sizeof(input_event))
                        {
                            input_event* event = reinterpret_cast<input_event*>(TEMP_BUFFER + i);

                            if (inputDevice.getDeviceClass() & EventDevice::CLASS_KEYBOARD)
                            {
                                if (event->type == EV_KEY)
                                {
                                    if (event->value == 1 || event->value == 2) // press or repeat
                                    {
                                        if (event->value >= 0 && event->value < 256) keyboardKeyDown[event->value] = true;
                                        keyPress(convertKeyCode(event->code), getModifiers());
                                    }
                                    else if (event->value == 0) // release
                                    {
                                        if (event->value >= 0 && event->value < 256) keyboardKeyDown[event->value] = false;
                                        keyRelease(convertKeyCode(event->code), getModifiers());
                                    }
                                }
                            }
                            if (inputDevice.getDeviceClass() & EventDevice::CLASS_MOUSE)
                            {
                                if (event->type == EV_ABS)
                                {
                                    Vector2 absolutePos = cursorPosition;

                                    if (event->code == ABS_X)
                                    {
                                        absolutePos.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0F)).x;
                                    }
                                    else if (event->code == ABS_Y)
                                    {
                                        absolutePos.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event->value))).y;
                                    }

                                    mouseMove(absolutePos, getModifiers());
                                }
                                else if (event->type == EV_REL)
                                {
                                    Vector2 relativePos;

                                    if (event->code == REL_X)
                                    {
                                        relativePos.x = static_cast<float>(event->value);
                                    }
                                    else if (event->code == REL_Y)
                                    {
                                        relativePos.y = static_cast<float>(event->value);
                                    }

                                    mouseRelativeMove(engine->getWindow()->convertWindowToNormalizedLocationRelative(relativePos), getModifiers());
                                }
                                else if (event->type == EV_KEY)
                                {
                                    MouseButton button;
                                    int buttonIndex = -1;

                                    switch (event->code)
                                    {
                                    case BTN_LEFT:
                                        button = MouseButton::LEFT;
                                        buttonIndex = 0;
                                        break;
                                    case BTN_RIGHT:
                                        button = MouseButton::RIGHT;
                                        buttonIndex =  1;
                                        break;
                                    case BTN_MIDDLE:
                                        button = MouseButton::MIDDLE;
                                        buttonIndex = 2;
                                        break;
                                    default:
                                        button = MouseButton::NONE;
                                    }

                                    if (event->value == 1)
                                    {
                                        if (buttonIndex >= 0 && buttonIndex < 3) mouseButtonDown[buttonIndex] = true;
                                        mouseButtonPress(button, cursorPosition, getModifiers());
                                    }
                                    else if (event->value == 0)
                                    {
                                        if (buttonIndex >= 0 && buttonIndex < 3) mouseButtonDown[buttonIndex] = false;
                                        mouseButtonRelease(button, cursorPosition, getModifiers());
                                    }
                                }
                            }
                            if (inputDevice.getDeviceClass() & EventDevice::CLASS_TOUCHPAD)
                            {
                                // TODO: implement
                            }
                            if (inputDevice.getDeviceClass() & EventDevice::CLASS_GAMEPAD)
                            {
                                // TODO: implement
                            }
                        }
                    }
                }
            }
        }

        uint32_t InputManagerRasp::getModifiers() const
        {
            uint32_t modifiers = 0;

            if (keyboardKeyDown[KEY_LEFTSHIFT] || keyboardKeyDown[KEY_RIGHTSHIFT]) modifiers |= SHIFT_DOWN;
            if (keyboardKeyDown[KEY_LEFTALT] || keyboardKeyDown[KEY_RIGHTALT]) modifiers |= ALT_DOWN;
            if (keyboardKeyDown[KEY_LEFTCTRL] || keyboardKeyDown[KEY_RIGHTCTRL]) modifiers |= CONTROL_DOWN;
            if (keyboardKeyDown[KEY_LEFTMETA] || keyboardKeyDown[KEY_RIGHTMETA]) modifiers |= SUPER_DOWN;

            if (mouseButtonDown[0]) modifiers |= LEFT_MOUSE_DOWN;
            if (mouseButtonDown[1]) modifiers |= RIGHT_MOUSE_DOWN;
            if (mouseButtonDown[2]) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }
    } // namespace input
} // namespace ouzel
