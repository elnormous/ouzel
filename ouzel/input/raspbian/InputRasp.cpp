// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glob.h>
#include "InputRasp.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "utils/Log.h"

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define BITS_PER_LONG (8 * sizeof(long))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_LONG)

static inline int isBitSet(const unsigned long* array, int bit)
{
    return !!(array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG)));
}

static const int KEY_ESC = 1;
static const int KEY_1 = 2;
static const int KEY_2 = 3;
static const int KEY_3 = 4;
static const int KEY_4 = 5;
static const int KEY_5 = 6;
static const int KEY_6 = 7;
static const int KEY_7 = 8;
static const int KEY_8 = 9;
static const int KEY_9 = 10;
static const int KEY_0 = 11;
static const int KEY_MINUS = 12;
static const int KEY_EQUAL = 13;
static const int KEY_BACKSPACE = 14;
static const int KEY_TAB = 15;
static const int KEY_Q = 16;
static const int KEY_W = 17;
static const int KEY_E = 18;
static const int KEY_R = 19;
static const int KEY_T = 20;
static const int KEY_Y = 21;
static const int KEY_U = 22;
static const int KEY_I = 23;
static const int KEY_O = 24;
static const int KEY_P = 25;
static const int KEY_LEFTBRACE = 26;
static const int KEY_RIGHTBRACE = 27;
static const int KEY_ENTER = 28;
static const int KEY_LEFTCTRL = 29;
static const int KEY_A = 30;
static const int KEY_S = 31;
static const int KEY_D = 32;
static const int KEY_F = 33;
static const int KEY_G = 34;
static const int KEY_H = 35;
static const int KEY_J = 36;
static const int KEY_K = 37;
static const int KEY_L = 38;
static const int KEY_SEMICOLON = 39;
static const int KEY_APOSTROPHE = 40;
static const int KEY_GRAVE = 41;
static const int KEY_LEFTSHIFT = 42;
static const int KEY_BACKSLASH = 43;
static const int KEY_Z = 44;
static const int KEY_X = 45;
static const int KEY_C = 46;
static const int KEY_V = 47;
static const int KEY_B = 48;
static const int KEY_N = 49;
static const int KEY_M = 50;
static const int KEY_COMMA = 51;
static const int KEY_DOT = 52;
static const int KEY_SLASH = 53;
static const int KEY_RIGHTSHIFT = 54;
static const int KEY_KPASTERISK = 55;
static const int KEY_LEFTALT = 56;
static const int KEY_SPACE = 57;
static const int KEY_CAPSLOCK = 58;
static const int KEY_F1 = 59;
static const int KEY_F2 = 60;
static const int KEY_F3 = 61;
static const int KEY_F4 = 62;
static const int KEY_F5 = 63;
static const int KEY_F6 = 64;
static const int KEY_F7 = 65;
static const int KEY_F8 = 66;
static const int KEY_F9 = 67;
static const int KEY_F10 = 68;
static const int KEY_NUMLOCK = 69;
static const int KEY_SCROLLLOCK = 70;
static const int KEY_KP7 = 71;
static const int KEY_KP8 = 72;
static const int KEY_KP9 = 73;
static const int KEY_KPMINUS = 74;
static const int KEY_KP4 = 75;
static const int KEY_KP5 = 76;
static const int KEY_KP6 = 77;
static const int KEY_KPPLUS = 78;
static const int KEY_KP1 = 79;
static const int KEY_KP2 = 80;
static const int KEY_KP3 = 81;
static const int KEY_KP0 = 82;
static const int KEY_KPDOT = 83;

static const int KEY_ZENKAKUHANKAKU = 85;
static const int KEY_102ND = 86;
static const int KEY_F11 = 87;
static const int KEY_F12 = 88;
static const int KEY_RO = 89;
static const int KEY_KATAKANA = 90;
static const int KEY_HIRAGANA = 91;
static const int KEY_HENKAN = 92;
static const int KEY_KATAKANAHIRAGANA = 93;
static const int KEY_MUHENKAN = 94;
static const int KEY_KPJPCOMMA = 95;
static const int KEY_KPENTER = 96;
static const int KEY_RIGHTCTRL = 97;
static const int KEY_KPSLASH = 98;
static const int KEY_SYSRQ = 99;
static const int KEY_RIGHTALT = 100;
static const int KEY_LINEFEED = 101;
static const int KEY_HOME = 102;
static const int KEY_UP = 103;
static const int KEY_PAGEUP = 104;
static const int KEY_LEFT = 105;
static const int KEY_RIGHT = 106;
static const int KEY_END = 107;
static const int KEY_DOWN = 108;
static const int KEY_PAGEDOWN = 109;
static const int KEY_INSERT = 110;
static const int KEY_DELETE = 111;
static const int KEY_MACRO = 112;
static const int KEY_MUTE = 113;
static const int KEY_VOLUMEDOWN = 114;
static const int KEY_VOLUMEUP = 115;
static const int KEY_POWER = 116; /* SC System Power Down */
static const int KEY_KPEQUAL = 117;
static const int KEY_KPPLUSMINUS = 118;
static const int KEY_PAUSE = 119;
static const int KEY_SCALE = 120; /* AL Compiz Scale (Expose) */

static const int KEY_KPCOMMA = 121;
static const int KEY_HANGEUL = 122;
static const int KEY_HANGUEL = KEY_HANGEUL;
static const int KEY_HANJA = 123;
static const int KEY_YEN = 124;
static const int KEY_LEFTMETA = 125;
static const int KEY_RIGHTMETA = 126;
static const int KEY_COMPOSE = 127;

static const int KEY_F13 = 183;
static const int KEY_F14 = 184;
static const int KEY_F15 = 185;
static const int KEY_F16 = 186;
static const int KEY_F17 = 187;
static const int KEY_F18 = 188;
static const int KEY_F19 = 189;
static const int KEY_F20 = 190;
static const int KEY_F21 = 191;
static const int KEY_F22 = 192;
static const int KEY_F23 = 193;
static const int KEY_F24 = 194;

namespace ouzel
{
    namespace input
    {
        static KeyboardKey convertKeyCode(int keyCode)
        {
            switch(keyCode)
            {
                case KEY_ESC: return KeyboardKey::ESCAPE;
                case KEY_1: return KeyboardKey::KEY_1;
                case KEY_2: return KeyboardKey::KEY_2;
                case KEY_3: return KeyboardKey::KEY_3;
                case KEY_4: return KeyboardKey::KEY_4;
                case KEY_5: return KeyboardKey::KEY_5;
                case KEY_6: return KeyboardKey::KEY_6;
                case KEY_7: return KeyboardKey::KEY_7;
                case KEY_8: return KeyboardKey::KEY_8;
                case KEY_9: return KeyboardKey::KEY_9;
                case KEY_0: return KeyboardKey::KEY_0;
                case KEY_MINUS: return KeyboardKey::MINUS;
                case KEY_EQUAL: return KeyboardKey::PLUS;
                case KEY_BACKSPACE: return KeyboardKey::BACKSPACE;
                case KEY_TAB: return KeyboardKey::TAB;
                case KEY_Q: return KeyboardKey::KEY_Q;
                case KEY_W: return KeyboardKey::KEY_W;
                case KEY_E: return KeyboardKey::KEY_E;
                case KEY_R: return KeyboardKey::KEY_R;
                case KEY_T: return KeyboardKey::KEY_T;
                case KEY_Y: return KeyboardKey::KEY_Y;
                case KEY_U: return KeyboardKey::KEY_U;
                case KEY_I: return KeyboardKey::KEY_I;
                case KEY_O: return KeyboardKey::KEY_O;
                case KEY_P: return KeyboardKey::KEY_P;
                case KEY_LEFTBRACE: return KeyboardKey::BRACKET_LEFT;
                case KEY_RIGHTBRACE: return KeyboardKey::BRACKET_RIGHT;
                case KEY_ENTER: return KeyboardKey::RETURN;
                case KEY_LEFTCTRL: return KeyboardKey::LCONTROL;
                case KEY_A: return KeyboardKey::KEY_A;
                case KEY_S: return KeyboardKey::KEY_S;
                case KEY_D: return KeyboardKey::KEY_D;
                case KEY_F: return KeyboardKey::KEY_F;
                case KEY_G: return KeyboardKey::KEY_G;
                case KEY_H: return KeyboardKey::KEY_H;
                case KEY_J: return KeyboardKey::KEY_J;
                case KEY_K: return KeyboardKey::KEY_K;
                case KEY_L: return KeyboardKey::KEY_L;
                case KEY_SEMICOLON: return KeyboardKey::SEMICOLON;
                case KEY_APOSTROPHE: return KeyboardKey::QUOTE;
                case KEY_GRAVE: return KeyboardKey::GRAVE;
                case KEY_LEFTSHIFT: return KeyboardKey::LSHIFT;
                case KEY_BACKSLASH: return KeyboardKey::BACKSLASH;
                case KEY_Z: return KeyboardKey::KEY_Z;
                case KEY_X: return KeyboardKey::KEY_X;
                case KEY_C: return KeyboardKey::KEY_C;
                case KEY_V: return KeyboardKey::KEY_V;
                case KEY_B: return KeyboardKey::KEY_B;
                case KEY_N: return KeyboardKey::KEY_N;
                case KEY_M: return KeyboardKey::KEY_M;
                case KEY_COMMA: return KeyboardKey::COMMA;
                case KEY_DOT: return KeyboardKey::PERIOD;
                case KEY_SLASH: return KeyboardKey::SLASH;
                case KEY_RIGHTSHIFT: return KeyboardKey::RSHIFT;
                case KEY_KPASTERISK: return KeyboardKey::MULTIPLY;
                case KEY_LEFTALT: return KeyboardKey::LALT;
                case KEY_SPACE: return KeyboardKey::SPACE;
                case KEY_CAPSLOCK: return KeyboardKey::CAPITAL;
                case KEY_F1: return KeyboardKey::F1;
                case KEY_F2: return KeyboardKey::F2;
                case KEY_F3: return KeyboardKey::F3;
                case KEY_F4: return KeyboardKey::F4;
                case KEY_F5: return KeyboardKey::F5;
                case KEY_F6: return KeyboardKey::F6;
                case KEY_F7: return KeyboardKey::F7;
                case KEY_F8: return KeyboardKey::F8;
                case KEY_F9: return KeyboardKey::F9;
                case KEY_F10: return KeyboardKey::F10;
                case KEY_NUMLOCK: return KeyboardKey::NUMLOCK;
                case KEY_SCROLLLOCK: return KeyboardKey::SCROLL;
                case KEY_KP7: return KeyboardKey::NUMPAD7;
                case KEY_KP8: return KeyboardKey::NUMPAD8;
                case KEY_KP9: return KeyboardKey::NUMPAD9;
                case KEY_KPMINUS: return KeyboardKey::SUBTRACT;
                case KEY_KP4: return KeyboardKey::NUMPAD4;
                case KEY_KP5: return KeyboardKey::NUMPAD5;
                case KEY_KP6: return KeyboardKey::NUMPAD6;
                case KEY_KPPLUS: return KeyboardKey::ADD;
                case KEY_KP1: return KeyboardKey::NUMPAD1;
                case KEY_KP2: return KeyboardKey::NUMPAD2;
                case KEY_KP3: return KeyboardKey::NUMPAD3;
                case KEY_KP0: return KeyboardKey::NUMPAD0;
                case KEY_KPDOT: return KeyboardKey::DECIMAL;

                case KEY_ZENKAKUHANKAKU: return KeyboardKey::NONE; // ??
                case KEY_102ND: return KeyboardKey::LESS;
                case KEY_F11: return KeyboardKey::F11;
                case KEY_F12: return KeyboardKey::F12;
                case KEY_RO: return KeyboardKey::NONE; // ??
                case KEY_KATAKANA: return KeyboardKey::NONE; // ??
                case KEY_HIRAGANA: return KeyboardKey::NONE; // ??
                case KEY_HENKAN: return KeyboardKey::NONE; // ??
                case KEY_KATAKANAHIRAGANA: return KeyboardKey::NONE; // ??
                case KEY_MUHENKAN: return KeyboardKey::NONE; // ??
                case KEY_KPJPCOMMA: return KeyboardKey::NONE; // ??
                case KEY_KPENTER: return KeyboardKey::RETURN;
                case KEY_RIGHTCTRL: return KeyboardKey::RCONTROL;
                case KEY_KPSLASH: return KeyboardKey::DIVIDE;
                case KEY_SYSRQ: return KeyboardKey::NONE; // ??
                case KEY_RIGHTALT: return KeyboardKey::RALT;
                case KEY_LINEFEED: return KeyboardKey::NONE; // ??
                case KEY_HOME: return KeyboardKey::HOME;
                case KEY_UP: return KeyboardKey::UP;
                case KEY_PAGEUP: return KeyboardKey::PRIOR;
                case KEY_LEFT: return KeyboardKey::LEFT;
                case KEY_RIGHT: return KeyboardKey::RIGHT;
                case KEY_END: return KeyboardKey::END;
                case KEY_DOWN: return KeyboardKey::DOWN;
                case KEY_PAGEDOWN: return KeyboardKey::NEXT;
                case KEY_INSERT: return KeyboardKey::INSERT;
                case KEY_DELETE: return KeyboardKey::DEL;
                case KEY_MACRO: return KeyboardKey::NONE; // ??
                case KEY_MUTE: return KeyboardKey::NONE; // ??
                case KEY_VOLUMEDOWN: return KeyboardKey::NONE; // ??
                case KEY_VOLUMEUP: return KeyboardKey::NONE; // ??
                case KEY_POWER: return KeyboardKey::NONE; // ??
                case KEY_KPEQUAL: return KeyboardKey::EQUAL;
                case KEY_KPPLUSMINUS: return KeyboardKey::NONE; // ??
                case KEY_PAUSE: return KeyboardKey::PAUSE;
                case KEY_SCALE: return KeyboardKey::NONE; //?

                case KEY_KPCOMMA: return KeyboardKey::SEPARATOR;
                case KEY_HANGEUL: return KeyboardKey::NONE; // KEY_HANGUEL
                case KEY_HANJA: return KeyboardKey::HANJA;
                case KEY_YEN: return KeyboardKey::NONE; // ??
                case KEY_LEFTMETA: return KeyboardKey::LSUPER;
                case KEY_RIGHTMETA: return KeyboardKey::RSUPER;
                case KEY_COMPOSE: return KeyboardKey::NONE; // ??

                case KEY_F13: return KeyboardKey::F13;
                case KEY_F14: return KeyboardKey::F14;
                case KEY_F15: return KeyboardKey::F15;
                case KEY_F16: return KeyboardKey::F16;
                case KEY_F17: return KeyboardKey::F17;
                case KEY_F18: return KeyboardKey::F18;
                case KEY_F19: return KeyboardKey::F19;
                case KEY_F20: return KeyboardKey::F20;
                case KEY_F21: return KeyboardKey::F21;
                case KEY_F22: return KeyboardKey::F22;
                case KEY_F23: return KeyboardKey::F23;
                case KEY_F24: return KeyboardKey::F24;
                default: return KeyboardKey::NONE;
            }
        }
    } // namespace input
} // namespace ouzel

#include <linux/input.h>

static char TEMP_BUFFER[256];

namespace ouzel
{
    namespace input
    {
        InputRasp::InputRasp()
        {
            std::fill(std::begin(keyboardKeyDown), std::end(keyboardKeyDown), false);
            std::fill(std::begin(mouseButtonDown), std::end(mouseButtonDown), false);

            glob_t g;
            int result = glob("/dev/input/event*", GLOB_NOSORT, NULL, &g);

            if (result == GLOB_NOMATCH)
            {
                Log(Log::Level::WARN) << "No event devices found";
                return;
            }
            else if (result)
            {
                Log(Log::Level::ERR) << "Could not read /dev/input/event*";
                return;
            }

            for (size_t i = 0; i < g.gl_pathc; i++)
            {
                InputDeviceRasp inputDevice;

                inputDevice.fd = open(g.gl_pathv[i], O_RDONLY);
                if (inputDevice.fd == -1)
                {
                    Log(Log::Level::WARN) << "Failed to open device file descriptor";
                    continue;
                }

                if (ioctl(inputDevice.fd, EVIOCGRAB, (void *)1) == -1)
                {
                    Log(Log::Level::WARN) << "Failed to get grab device";
                }

                memset(TEMP_BUFFER, 0, sizeof(TEMP_BUFFER));
                if (ioctl(inputDevice.fd, EVIOCGNAME(sizeof(TEMP_BUFFER) - 1), TEMP_BUFFER) == -1)
                {
                    Log(Log::Level::WARN) << "Failed to get device name";
                }
                else
                {
                    Log(Log::Level::INFO) << "Got device: " << TEMP_BUFFER;
                }

                unsigned long eventBits[BITS_TO_LONGS(EV_CNT)];
                unsigned long absBits[BITS_TO_LONGS(ABS_CNT)];
                unsigned long relBits[BITS_TO_LONGS(REL_CNT)];
                unsigned long keyBits[BITS_TO_LONGS(KEY_CNT)];

                if (ioctl(inputDevice.fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) == -1 ||
                    ioctl(inputDevice.fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) == -1 ||
                    ioctl(inputDevice.fd, EVIOCGBIT(EV_REL, sizeof(relBits)), relBits) == -1 ||
                    ioctl(inputDevice.fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) == -1)
                {
                    Log(Log::Level::WARN) << "Failed to get device event bits";
                    continue;
                }

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
                {
                    Log(Log::Level::INFO) << "Device class: keyboard";
                    inputDevice.deviceClass = InputDeviceRasp::CLASS_KEYBOARD;
                }

                if (isBitSet(eventBits, EV_ABS) && isBitSet(absBits, ABS_X) && isBitSet(absBits, ABS_Y))
                {
                    if (isBitSet(keyBits, BTN_STYLUS) || isBitSet(keyBits, BTN_TOOL_PEN))
                    {
                        Log(Log::Level::INFO) << "Device class: tablet";
                        inputDevice.deviceClass |= InputDeviceRasp::CLASS_TOUCHPAD;
                    }
                    else if (isBitSet(keyBits, BTN_TOOL_FINGER) && !isBitSet(keyBits, BTN_TOOL_PEN))
                    {
                        Log(Log::Level::INFO) << "Device class: touchpad";
                        inputDevice.deviceClass |= InputDeviceRasp::CLASS_TOUCHPAD;
                    }
                    else if (isBitSet(keyBits, BTN_MOUSE))
                    {
                        Log(Log::Level::INFO) << "Device class: mouse";
                        inputDevice.deviceClass |= InputDeviceRasp::CLASS_MOUSE;
                    }
                    else if (isBitSet(keyBits, BTN_TOUCH))
                    {
                        Log(Log::Level::INFO) << "Device class: touchscreen";
                        inputDevice.deviceClass |= InputDeviceRasp::CLASS_TOUCHPAD;
                    }
                }
                else if (isBitSet(eventBits, EV_REL) && isBitSet(relBits, REL_X) && isBitSet(relBits, REL_Y))
                {
                    if (isBitSet(keyBits, BTN_MOUSE))
                    {
                        Log(Log::Level::INFO) << "Device class: mouse";
                        inputDevice.deviceClass |= InputDeviceRasp::CLASS_MOUSE;
                    }
                }

                if (isBitSet(keyBits, BTN_JOYSTICK))
                {
                    Log(Log::Level::INFO) << "Device class: joystick";
                    inputDevice.deviceClass = InputDeviceRasp::CLASS_GAMEPAD;
                }

                if (isBitSet(keyBits, BTN_GAMEPAD))
                {
                    Log(Log::Level::INFO) << "Device class: gamepad";
                    inputDevice.deviceClass = InputDeviceRasp::CLASS_GAMEPAD;
                }

                if (inputDevice.fd > maxFd)
                {
                    maxFd = inputDevice.fd;
                }

                inputDevices.push_back(inputDevice);
            }

            globfree(&g);
        }

        InputRasp::~InputRasp()
        {
            for (const InputDeviceRasp& inputDevice : inputDevices)
            {
                if (ioctl(inputDevice.fd, EVIOCGRAB, (void*)0) == -1)
                {
                    Log(Log::Level::WARN) << "Failed to release device";
                }

                if (close(inputDevice.fd) == -1)
                {
                    Log(Log::Level::ERR) << "Failed to close file descriptor";
                }
            }
        }

        void InputRasp::update()
        {
            fd_set rfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);

            for (const InputDeviceRasp& inputDevice : inputDevices)
            {
                FD_SET(inputDevice.fd, &rfds);
            }

            int retval = select(maxFd + 1, &rfds, NULL, NULL, &tv);

            if (retval == -1)
            {
                Log(Log::Level::ERR) << "Select failed";
                return;
            }
            else if (retval > 0)
            {
                for (const InputDeviceRasp& inputDevice : inputDevices)
                {
                    if (FD_ISSET(inputDevice.fd, &rfds))
                    {
                        ssize_t bytesRead = read(inputDevice.fd, TEMP_BUFFER, sizeof(TEMP_BUFFER));

                        if (bytesRead == -1)
                        {
                            continue;
                        }

                        for (ssize_t i = 0; i < bytesRead - static_cast<ssize_t>(sizeof(input_event)) + 1; i += sizeof(input_event))
                        {
                            input_event* event = reinterpret_cast<input_event*>(TEMP_BUFFER + i);

                            if (inputDevice.deviceClass & InputDeviceRasp::CLASS_KEYBOARD)
                            {
                                if (event->type == EV_KEY)
                                {
                                    if (event->value == 1 || event->value == 2) // press or repeat
                                    {
                                        if (event->value >= 0 && event->value < 256) keyboardKeyDown[event->value] = true;
                                        keyDown(convertKeyCode(event->code), getModifiers());
                                    }
                                    else if (event->value == 0) // release
                                    {
                                        if (event->value >= 0 && event->value < 256) keyboardKeyDown[event->value] = false;
                                        keyUp(convertKeyCode(event->code), getModifiers());
                                    }
                                }
                            }
                            if (inputDevice.deviceClass & InputDeviceRasp::CLASS_MOUSE)
                            {
                                if (event->type == EV_ABS)
                                {
                                    Vector2 absolutePos = cursorPosition;

                                    if (event->code == ABS_X)
                                    {
                                        absolutePos.v[0] = sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0f)).v[0];
                                    }
                                    else if (event->code == ABS_Y)
                                    {
                                        absolutePos.v[1] = sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0f, static_cast<float>(event->value))).v[1];
                                    }

                                    mouseMove(absolutePos, getModifiers());
                                }
                                else if (event->type == EV_REL)
                                {
                                    Vector2 relativePos;

                                    if (event->code == REL_X)
                                    {
                                        relativePos.v[0] = static_cast<float>(event->value);
                                    }
                                    else if (event->code == REL_Y)
                                    {
                                        relativePos.v[1] = static_cast<float>(event->value);
                                    }

                                    mouseRelativeMove(sharedEngine->getWindow()->convertWindowToNormalizedLocationRelative(relativePos), getModifiers());
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
                                        mouseDown(button, cursorPosition, getModifiers());
                                    }
                                    else if (event->value == 0)
                                    {
                                        if (buttonIndex >= 0 && buttonIndex < 3) mouseButtonDown[buttonIndex] = false;
                                        mouseUp(button, cursorPosition, getModifiers());
                                    }
                                }
                            }
                            if (inputDevice.deviceClass & InputDeviceRasp::CLASS_TOUCHPAD)
                            {
                                // TODO: implement
                            }
                            if (inputDevice.deviceClass & InputDeviceRasp::CLASS_GAMEPAD)
                            {
                                // TODO: implement
                            }
                        }
                    }
                }
            }
        }

        uint32_t InputRasp::getModifiers() const
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
