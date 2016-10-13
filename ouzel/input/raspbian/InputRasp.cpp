// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

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

using namespace ouzel;

static inline int isBitSet(const unsigned long* array, int bit)
{
    return !!(array[bit / BITS_PER_LONG] & (1LL << (bit % BITS_PER_LONG)));
}

const int KEY_ESC = 1;
const int KEY_1 = 2;
const int KEY_2 = 3;
const int KEY_3 = 4;
const int KEY_4 = 5;
const int KEY_5 = 6;
const int KEY_6 = 7;
const int KEY_7 = 8;
const int KEY_8 = 9;
const int KEY_9 = 10;
const int KEY_0 = 11;
const int KEY_MINUS = 12;
const int KEY_EQUAL = 13;
const int KEY_BACKSPACE = 14;
const int KEY_TAB = 15;
const int KEY_Q = 16;
const int KEY_W = 17;
const int KEY_E = 18;
const int KEY_R = 19;
const int KEY_T = 20;
const int KEY_Y = 21;
const int KEY_U = 22;
const int KEY_I = 23;
const int KEY_O = 24;
const int KEY_P = 25;
const int KEY_LEFTBRACE = 26;
const int KEY_RIGHTBRACE = 27;
const int KEY_ENTER = 28;
const int KEY_LEFTCTRL = 29;
const int KEY_A = 30;
const int KEY_S = 31;
const int KEY_D = 32;
const int KEY_F = 33;
const int KEY_G = 34;
const int KEY_H = 35;
const int KEY_J = 36;
const int KEY_K = 37;
const int KEY_L = 38;
const int KEY_SEMICOLON = 39;
const int KEY_APOSTROPHE = 40;
const int KEY_GRAVE = 41;
const int KEY_LEFTSHIFT = 42;
const int KEY_BACKSLASH = 43;
const int KEY_Z = 44;
const int KEY_X = 45;
const int KEY_C = 46;
const int KEY_V = 47;
const int KEY_B = 48;
const int KEY_N = 49;
const int KEY_M = 50;
const int KEY_COMMA = 51;
const int KEY_DOT = 52;
const int KEY_SLASH = 53;
const int KEY_RIGHTSHIFT = 54;
const int KEY_KPASTERISK = 55;
const int KEY_LEFTALT = 56;
const int KEY_SPACE = 57;
const int KEY_CAPSLOCK = 58;
const int KEY_F1 = 59;
const int KEY_F2 = 60;
const int KEY_F3 = 61;
const int KEY_F4 = 62;
const int KEY_F5 = 63;
const int KEY_F6 = 64;
const int KEY_F7 = 65;
const int KEY_F8 = 66;
const int KEY_F9 = 67;
const int KEY_F10 = 68;
const int KEY_NUMLOCK = 69;
const int KEY_SCROLLLOCK = 70;
const int KEY_KP7 = 71;
const int KEY_KP8 = 72;
const int KEY_KP9 = 73;
const int KEY_KPMINUS = 74;
const int KEY_KP4 = 75;
const int KEY_KP5 = 76;
const int KEY_KP6 = 77;
const int KEY_KPPLUS = 78;
const int KEY_KP1 = 79;
const int KEY_KP2 = 80;
const int KEY_KP3 = 81;
const int KEY_KP0 = 82;
const int KEY_KPDOT = 83;
;
const int KEY_ZENKAKUHANKAKU = 85;
const int KEY_102ND = 86;
const int KEY_F11 = 87;
const int KEY_F12 = 88;
const int KEY_RO = 89;
const int KEY_KATAKANA = 90;
const int KEY_HIRAGANA = 91;
const int KEY_HENKAN = 92;
const int KEY_KATAKANAHIRAGANA = 93;
const int KEY_MUHENKAN = 94;
const int KEY_KPJPCOMMA = 95;
const int KEY_KPENTER = 96;
const int KEY_RIGHTCTRL = 97;
const int KEY_KPSLASH = 98;
const int KEY_SYSRQ = 99;
const int KEY_RIGHTALT = 100;
const int KEY_LINEFEED = 101;
const int KEY_HOME = 102;
const int KEY_UP = 103;
const int KEY_PAGEUP = 104;
const int KEY_LEFT = 105;
const int KEY_RIGHT = 106;
const int KEY_END = 107;
const int KEY_DOWN = 108;
const int KEY_PAGEDOWN = 109;
const int KEY_INSERT = 110;
const int KEY_DELETE = 111;
const int KEY_MACRO = 112;
const int KEY_MUTE = 113;
const int KEY_VOLUMEDOWN = 114;
const int KEY_VOLUMEUP = 115;
const int KEY_POWER = 116; /* SC System Power Down */
const int KEY_KPEQUAL = 117;
const int KEY_KPPLUSMINUS = 118;
const int KEY_PAUSE = 119;
const int KEY_SCALE = 120; /* AL Compiz Scale (Expose) */

const int KEY_KPCOMMA = 121;
const int KEY_HANGEUL = 122;
const int KEY_HANGUEL = KEY_HANGEUL;
const int KEY_HANJA = 123;
const int KEY_YEN = 124;
const int KEY_LEFTMETA = 125;
const int KEY_RIGHTMETA = 126;
const int KEY_COMPOSE = 127;

const int KEY_F13 = 183;
const int KEY_F14 = 184;
const int KEY_F15 = 185;
const int KEY_F16 = 186;
const int KEY_F17 = 187;
const int KEY_F18 = 188;
const int KEY_F19 = 189;
const int KEY_F20 = 190;
const int KEY_F21 = 191;
const int KEY_F22 = 192;
const int KEY_F23 = 193;
const int KEY_F24 = 194;

static input::KeyboardKey convertKeyCode(int keyCode)
{
    switch(keyCode)
    {
        case KEY_ESC: return input::KeyboardKey::ESCAPE;
        case KEY_1: return input::KeyboardKey::KEY_1;
        case KEY_2: return input::KeyboardKey::KEY_2;
        case KEY_3: return input::KeyboardKey::KEY_3;
        case KEY_4: return input::KeyboardKey::KEY_4;
        case KEY_5: return input::KeyboardKey::KEY_5;
        case KEY_6: return input::KeyboardKey::KEY_6;
        case KEY_7: return input::KeyboardKey::KEY_7;
        case KEY_8: return input::KeyboardKey::KEY_8;
        case KEY_9: return input::KeyboardKey::KEY_9;
        case KEY_0: return input::KeyboardKey::KEY_0;
        case KEY_MINUS: return input::KeyboardKey::MINUS;
        case KEY_EQUAL: return input::KeyboardKey::PLUS;
        case KEY_BACKSPACE: return input::KeyboardKey::BACKSPACE;
        case KEY_TAB: return input::KeyboardKey::TAB;
        case KEY_Q: return input::KeyboardKey::KEY_Q;
        case KEY_W: return input::KeyboardKey::KEY_W;
        case KEY_E: return input::KeyboardKey::KEY_E;
        case KEY_R: return input::KeyboardKey::KEY_R;
        case KEY_T: return input::KeyboardKey::KEY_T;
        case KEY_Y: return input::KeyboardKey::KEY_Y;
        case KEY_U: return input::KeyboardKey::KEY_U;
        case KEY_I: return input::KeyboardKey::KEY_I;
        case KEY_O: return input::KeyboardKey::KEY_O;
        case KEY_P: return input::KeyboardKey::KEY_P;
        case KEY_LEFTBRACE: return input::KeyboardKey::BRACKET_LEFT;
        case KEY_RIGHTBRACE: return input::KeyboardKey::BRACKET_RIGHT;
        case KEY_ENTER: return input::KeyboardKey::RETURN;
        case KEY_LEFTCTRL: return input::KeyboardKey::LCONTROL;
        case KEY_A: return input::KeyboardKey::KEY_A;
        case KEY_S: return input::KeyboardKey::KEY_S;
        case KEY_D: return input::KeyboardKey::KEY_D;
        case KEY_F: return input::KeyboardKey::KEY_F;
        case KEY_G: return input::KeyboardKey::KEY_G;
        case KEY_H: return input::KeyboardKey::KEY_H;
        case KEY_J: return input::KeyboardKey::KEY_J;
        case KEY_K: return input::KeyboardKey::KEY_K;
        case KEY_L: return input::KeyboardKey::KEY_L;
        case KEY_SEMICOLON: return input::KeyboardKey::SEMICOLON;
        case KEY_APOSTROPHE: return input::KeyboardKey::QUOTE;
        case KEY_GRAVE: return input::KeyboardKey::GRAVE;
        case KEY_LEFTSHIFT: return input::KeyboardKey::LSHIFT;
        case KEY_BACKSLASH: return input::KeyboardKey::BACKSLASH;
        case KEY_Z: return input::KeyboardKey::KEY_Z;
        case KEY_X: return input::KeyboardKey::KEY_X;
        case KEY_C: return input::KeyboardKey::KEY_C;
        case KEY_V: return input::KeyboardKey::KEY_V;
        case KEY_B: return input::KeyboardKey::KEY_B;
        case KEY_N: return input::KeyboardKey::KEY_N;
        case KEY_M: return input::KeyboardKey::KEY_M;
        case KEY_COMMA: return input::KeyboardKey::COMMA;
        case KEY_DOT: return input::KeyboardKey::PERIOD;
        case KEY_SLASH: return input::KeyboardKey::SLASH;
        case KEY_RIGHTSHIFT: return input::KeyboardKey::RSHIFT;
        case KEY_KPASTERISK: return input::KeyboardKey::MULTIPLY;
        case KEY_LEFTALT: return input::KeyboardKey::LALT;
        case KEY_SPACE: return input::KeyboardKey::SPACE;
        case KEY_CAPSLOCK: return input::KeyboardKey::CAPITAL;
        case KEY_F1: return input::KeyboardKey::F1;
        case KEY_F2: return input::KeyboardKey::F2;
        case KEY_F3: return input::KeyboardKey::F3;
        case KEY_F4: return input::KeyboardKey::F4;
        case KEY_F5: return input::KeyboardKey::F5;
        case KEY_F6: return input::KeyboardKey::F6;
        case KEY_F7: return input::KeyboardKey::F7;
        case KEY_F8: return input::KeyboardKey::F8;
        case KEY_F9: return input::KeyboardKey::F9;
        case KEY_F10: return input::KeyboardKey::F10;
        case KEY_NUMLOCK: return input::KeyboardKey::NUMLOCK;
        case KEY_SCROLLLOCK: return input::KeyboardKey::SCROLL;
        case KEY_KP7: return input::KeyboardKey::NUMPAD7;
        case KEY_KP8: return input::KeyboardKey::NUMPAD8;
        case KEY_KP9: return input::KeyboardKey::NUMPAD9;
        case KEY_KPMINUS: return input::KeyboardKey::SUBTRACT;
        case KEY_KP4: return input::KeyboardKey::NUMPAD4;
        case KEY_KP5: return input::KeyboardKey::NUMPAD5;
        case KEY_KP6: return input::KeyboardKey::NUMPAD6;
        case KEY_KPPLUS: return input::KeyboardKey::ADD;
        case KEY_KP1: return input::KeyboardKey::NUMPAD1;
        case KEY_KP2: return input::KeyboardKey::NUMPAD2;
        case KEY_KP3: return input::KeyboardKey::NUMPAD3;
        case KEY_KP0: return input::KeyboardKey::NUMPAD0;
        case KEY_KPDOT: return input::KeyboardKey::DECIMAL;

        case KEY_ZENKAKUHANKAKU: return input::KeyboardKey::NONE; // ??
        case KEY_102ND: return input::KeyboardKey::LESS;
        case KEY_F11: return input::KeyboardKey::F11;
        case KEY_F12: return input::KeyboardKey::F12;
        case KEY_RO: return input::KeyboardKey::NONE; // ??
        case KEY_KATAKANA: return input::KeyboardKey::NONE; // ??
        case KEY_HIRAGANA: return input::KeyboardKey::NONE; // ??
        case KEY_HENKAN: return input::KeyboardKey::NONE; // ??
        case KEY_KATAKANAHIRAGANA: return input::KeyboardKey::NONE; // ??
        case KEY_MUHENKAN: return input::KeyboardKey::NONE; // ??
        case KEY_KPJPCOMMA: return input::KeyboardKey::NONE; // ??
        case KEY_KPENTER: return input::KeyboardKey::RETURN;
        case KEY_RIGHTCTRL: return input::KeyboardKey::RCONTROL;
        case KEY_KPSLASH: return input::KeyboardKey::DIVIDE;
        case KEY_SYSRQ: return input::KeyboardKey::NONE; // ??
        case KEY_RIGHTALT: return input::KeyboardKey::RALT;
        case KEY_LINEFEED: return input::KeyboardKey::NONE; // ??
        case KEY_HOME: return input::KeyboardKey::HOME;
        case KEY_UP: return input::KeyboardKey::UP;
        case KEY_PAGEUP: return input::KeyboardKey::PRIOR;
        case KEY_LEFT: return input::KeyboardKey::LEFT;
        case KEY_RIGHT: return input::KeyboardKey::RIGHT;
        case KEY_END: return input::KeyboardKey::END;
        case KEY_DOWN: return input::KeyboardKey::DOWN;
        case KEY_PAGEDOWN: return input::KeyboardKey::NEXT;
        case KEY_INSERT: return input::KeyboardKey::INSERT;
        case KEY_DELETE: return input::KeyboardKey::DEL;
        case KEY_MACRO: return input::KeyboardKey::NONE; // ??
        case KEY_MUTE: return input::KeyboardKey::NONE; // ??
        case KEY_VOLUMEDOWN: return input::KeyboardKey::NONE; // ??
        case KEY_VOLUMEUP: return input::KeyboardKey::NONE; // ??
        case KEY_POWER: return input::KeyboardKey::NONE; // ??
        case KEY_KPEQUAL: return input::KeyboardKey::EQUAL;
        case KEY_KPPLUSMINUS: return input::KeyboardKey::NONE; // ??
        case KEY_PAUSE: return input::KeyboardKey::PAUSE;
        case KEY_SCALE: return input::KeyboardKey::NONE; //?

        case KEY_KPCOMMA: return input::KeyboardKey::SEPARATOR;
        case KEY_HANGEUL: return input::KeyboardKey::NONE; // KEY_HANGUEL
        case KEY_HANJA: return input::KeyboardKey::HANJA;
        case KEY_YEN: return input::KeyboardKey::NONE; // ??
        case KEY_LEFTMETA: return input::KeyboardKey::LSUPER;
        case KEY_RIGHTMETA: return input::KeyboardKey::RSUPER;
        case KEY_COMPOSE: return input::KeyboardKey::NONE; // ??

        case KEY_F13: return input::KeyboardKey::F13;
        case KEY_F14: return input::KeyboardKey::F14;
        case KEY_F15: return input::KeyboardKey::F15;
        case KEY_F16: return input::KeyboardKey::F16;
        case KEY_F17: return input::KeyboardKey::F17;
        case KEY_F18: return input::KeyboardKey::F18;
        case KEY_F19: return input::KeyboardKey::F19;
        case KEY_F20: return input::KeyboardKey::F20;
        case KEY_F21: return input::KeyboardKey::F21;
        case KEY_F22: return input::KeyboardKey::F22;
        case KEY_F23: return input::KeyboardKey::F23;
        case KEY_F24: return input::KeyboardKey::F24;
        default: return input::KeyboardKey::NONE;
    }
}

#include <linux/input.h>

static char TEMP_BUFFER[256];

namespace ouzel
{
    namespace input
    {
        InputRasp::InputRasp()
        {
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
                                        keyDown(convertKeyCode(event->code), 0);
                                    }
                                    else if (event->value == 0) // release
                                    {
                                        keyUp(convertKeyCode(event->code), 0);
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
                                        absolutePos.x = sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0f)).x;
                                    }
                                    else if (event->code == ABS_Y)
                                    {
                                        absolutePos.y = sharedEngine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0f, static_cast<float>(event->value))).y;
                                    }

                                    mouseMove(absolutePos, 0);
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

                                    mouseRelativeMove(sharedEngine->getWindow()->convertWindowToNormalizedLocationRelative(relativePos), 0);
                                }
                                else if (event->type == EV_KEY)
                                {
                                    input::MouseButton button;

                                    switch (event->code)
                                    {
                                    case BTN_LEFT:
                                        button = input::MouseButton::LEFT;
                                        break;
                                    case BTN_RIGHT:
                                        button = input::MouseButton::RIGHT;
                                        break;
                                    case BTN_MIDDLE:
                                        button = input::MouseButton::MIDDLE;
                                        break;
                                    default:
                                        button = input::MouseButton::NONE;
                                    }

                                    if (event->value == 1)
                                    {
                                        mouseDown(button, cursorPosition, 0);
                                    }
                                    else if (event->value == 0)
                                    {
                                        mouseUp(button, cursorPosition, 0);
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
    } // namespace input
} // namespace ouzel
