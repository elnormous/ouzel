// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <linux/joystick.h>
#if OUZEL_SUPPORTS_X11
#  include <X11/cursorfont.h>
#else
#  include <linux/input.h>
#endif
#include "InputManagerLinux.hpp"
#include "NativeCursorLinux.hpp"
#include "events/Event.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
#if OUZEL_SUPPORTS_X11
        static const std::unordered_map<KeySym, KeyboardKey> keyMap = {
            {XK_BackSpace, KeyboardKey::BACKSPACE},
            {XK_Tab, KeyboardKey::TAB},
            {XK_ISO_Left_Tab, KeyboardKey::TAB},
            {XK_Linefeed, KeyboardKey::NONE}, // ?
            {XK_Clear, KeyboardKey::CLEAR},
            {XK_Return, KeyboardKey::RETURN},
            {XK_Pause, KeyboardKey::PAUSE},
            {XK_Scroll_Lock, KeyboardKey::SCROLL},
            {XK_Sys_Req, KeyboardKey::NONE}, // ?
            {XK_Escape, KeyboardKey::ESCAPE},
            {XK_Insert, KeyboardKey::INSERT},
            {XK_Delete, KeyboardKey::DEL},
            {XK_Home, KeyboardKey::HOME},
            {XK_Left, KeyboardKey::LEFT},
            {XK_Up, KeyboardKey::UP},
            {XK_Right, KeyboardKey::RIGHT},
            {XK_Down, KeyboardKey::DOWN},
            {XK_Prior, KeyboardKey::PRIOR}, // also XK_Page_Up
            {XK_Next, KeyboardKey::NEXT}, // also XK_Page_Down
            {XK_End, KeyboardKey::END},
            {XK_Begin, KeyboardKey::HOME},
            {XK_Num_Lock, KeyboardKey::NUMLOCK},
            {XK_KP_Space, KeyboardKey::SPACE},
            {XK_KP_Tab, KeyboardKey::TAB},
            {XK_KP_Enter, KeyboardKey::RETURN},
            {XK_KP_F1, KeyboardKey::F1},
            {XK_KP_F2, KeyboardKey::F2},
            {XK_KP_F3, KeyboardKey::F3},
            {XK_KP_F4, KeyboardKey::F4},
            {XK_KP_Home, KeyboardKey::HOME},
            {XK_KP_Left, KeyboardKey::LEFT},
            {XK_KP_Up, KeyboardKey::UP},
            {XK_KP_Right, KeyboardKey::RIGHT},
            {XK_KP_Down, KeyboardKey::DOWN},
            {XK_Print, KeyboardKey::PRINT},
            {XK_KP_Prior, KeyboardKey::PRIOR}, // alos XK_KP_Page_Up
            {XK_KP_Next, KeyboardKey::NEXT}, // also XK_KP_Page_Down
            {XK_KP_End, KeyboardKey::END},
            {XK_KP_Begin, KeyboardKey::HOME},
            {XK_KP_Insert, KeyboardKey::INSERT},
            {XK_KP_Delete, KeyboardKey::DEL},
            {XK_KP_Equal, KeyboardKey::EQUAL},
            {XK_KP_Multiply, KeyboardKey::MULTIPLY},
            {XK_KP_Add, KeyboardKey::ADD},
            {XK_KP_Separator, KeyboardKey::SEPARATOR},
            {XK_KP_Subtract, KeyboardKey::SUBTRACT},
            {XK_KP_Decimal, KeyboardKey::DECIMAL},
            {XK_KP_Divide, KeyboardKey::DIVIDE},
            {XK_KP_0, KeyboardKey::NUMPAD_0},
            {XK_KP_1, KeyboardKey::NUMPAD_1},
            {XK_KP_2, KeyboardKey::NUMPAD_2},
            {XK_KP_3, KeyboardKey::NUMPAD_3},
            {XK_KP_4, KeyboardKey::NUMPAD_4},
            {XK_KP_5, KeyboardKey::NUMPAD_5},
            {XK_KP_6, KeyboardKey::NUMPAD_6},
            {XK_KP_7, KeyboardKey::NUMPAD_7},
            {XK_KP_8, KeyboardKey::NUMPAD_8},
            {XK_KP_9, KeyboardKey::NUMPAD_9},
            {XK_F1, KeyboardKey::F1},
            {XK_F2, KeyboardKey::F2},
            {XK_F3, KeyboardKey::F3},
            {XK_F4, KeyboardKey::F4},
            {XK_F5, KeyboardKey::F5},
            {XK_F6, KeyboardKey::F6},
            {XK_F7, KeyboardKey::F7},
            {XK_F8, KeyboardKey::F8},
            {XK_F9, KeyboardKey::F9},
            {XK_F10, KeyboardKey::F10},
            {XK_F11, KeyboardKey::F11},
            {XK_F12, KeyboardKey::F12},
            {XK_Shift_L, KeyboardKey::LEFT_SHIFT},
            {XK_Shift_R, KeyboardKey::RIGHT_SHIFT},
            {XK_Control_L, KeyboardKey::LEFT_CONTROL},
            {XK_Control_R, KeyboardKey::RIGHT_CONTROL},
            {XK_Caps_Lock, KeyboardKey::CAPITAL},
            {XK_Shift_Lock, KeyboardKey::CAPITAL},
            {XK_Meta_L, KeyboardKey::LEFT_SUPER},
            {XK_Meta_R, KeyboardKey::RIGHT_SUPER},
            {XK_Alt_L, KeyboardKey::LEFT_ALT},
            {XK_Alt_R, KeyboardKey::RIGHT_ALT},
            {XK_ISO_Level3_Shift, KeyboardKey::RIGHT_SUPER},
            {XK_Menu, KeyboardKey::LEFT_SUPER},
            {XK_space, KeyboardKey::SPACE},
            {XK_exclam, KeyboardKey::NUM_1},
            {XK_quotedbl, KeyboardKey::NUM_2},
            {XK_section, KeyboardKey::NONE}, // ?
            {XK_numbersign, KeyboardKey::SLASH},
            {XK_dollar, KeyboardKey::NUM_4},
            {XK_percent, KeyboardKey::NUM_5},
            {XK_ampersand, KeyboardKey::NUM_7},
            {XK_apostrophe, KeyboardKey::QUOTE},
            {XK_parenleft, KeyboardKey::NUM_9},
            {XK_parenright, KeyboardKey::NUM_0},
            {XK_asterisk, KeyboardKey::NUM_8},
            {XK_plus, KeyboardKey::PLUS},
            {XK_comma, KeyboardKey::COMMA},
            {XK_minus, KeyboardKey::MINUS},
            {XK_period, KeyboardKey::PERIOD},
            {XK_slash, KeyboardKey::SLASH},
            {XK_0, KeyboardKey::NUM_0},
            {XK_1, KeyboardKey::NUM_1},
            {XK_2, KeyboardKey::NUM_2},
            {XK_3, KeyboardKey::NUM_3},
            {XK_4, KeyboardKey::NUM_4},
            {XK_5, KeyboardKey::NUM_5},
            {XK_6, KeyboardKey::NUM_6},
            {XK_7, KeyboardKey::NUM_7},
            {XK_8, KeyboardKey::NUM_8},
            {XK_9, KeyboardKey::NUM_9},
            {XK_colon, KeyboardKey::SEMICOLON},
            {XK_semicolon, KeyboardKey::SEMICOLON},
            {XK_less, KeyboardKey::COMMA},
            {XK_equal, KeyboardKey::PLUS},
            {XK_greater, KeyboardKey::PERIOD},
            {XK_question, KeyboardKey::SLASH},
            {XK_at, KeyboardKey::NUM_2}, // ?
            {XK_mu, KeyboardKey::NONE}, // ?
            {XK_EuroSign, KeyboardKey::NONE}, // ?
            {XK_A, KeyboardKey::A},
            {XK_B, KeyboardKey::B},
            {XK_C, KeyboardKey::C},
            {XK_D, KeyboardKey::D},
            {XK_E, KeyboardKey::E},
            {XK_F, KeyboardKey::F},
            {XK_G, KeyboardKey::G},
            {XK_H, KeyboardKey::H},
            {XK_I, KeyboardKey::I},
            {XK_J, KeyboardKey::J},
            {XK_K, KeyboardKey::K},
            {XK_L, KeyboardKey::L},
            {XK_M, KeyboardKey::M},
            {XK_N, KeyboardKey::N},
            {XK_O, KeyboardKey::O},
            {XK_P, KeyboardKey::P},
            {XK_Q, KeyboardKey::Q},
            {XK_R, KeyboardKey::R},
            {XK_S, KeyboardKey::S},
            {XK_T, KeyboardKey::T},
            {XK_U, KeyboardKey::U},
            {XK_V, KeyboardKey::V},
            {XK_W, KeyboardKey::W},
            {XK_X, KeyboardKey::X},
            {XK_Y, KeyboardKey::Y},
            {XK_Z, KeyboardKey::Z},
            {XK_bracketleft, KeyboardKey::LEFT_BRACKET},
            {XK_backslash, KeyboardKey::BACKSLASH},
            {XK_bracketright, KeyboardKey::RIGHT_BRACKET},
            {XK_asciicircum, KeyboardKey::BACKSLASH},
            {XK_dead_circumflex, KeyboardKey::BACKSLASH},
            {XK_degree, KeyboardKey::NONE}, // ?
            {XK_underscore, KeyboardKey::MINUS},
            {XK_grave, KeyboardKey::GRAVE},
            {XK_dead_grave, KeyboardKey::GRAVE},
            {XK_acute, KeyboardKey::RIGHT_BRACKET},
            {XK_dead_acute, KeyboardKey::RIGHT_BRACKET},
            {XK_a, KeyboardKey::A},
            {XK_b, KeyboardKey::B},
            {XK_c, KeyboardKey::C},
            {XK_d, KeyboardKey::D},
            {XK_e, KeyboardKey::E},
            {XK_f, KeyboardKey::F},
            {XK_g, KeyboardKey::G},
            {XK_h, KeyboardKey::H},
            {XK_i, KeyboardKey::I},
            {XK_j, KeyboardKey::J},
            {XK_k, KeyboardKey::K},
            {XK_l, KeyboardKey::L},
            {XK_m, KeyboardKey::M},
            {XK_n, KeyboardKey::N},
            {XK_o, KeyboardKey::O},
            {XK_p, KeyboardKey::P},
            {XK_q, KeyboardKey::Q},
            {XK_r, KeyboardKey::R},
            {XK_s, KeyboardKey::S},
            {XK_t, KeyboardKey::T},
            {XK_u, KeyboardKey::U},
            {XK_v, KeyboardKey::V},
            {XK_w, KeyboardKey::W},
            {XK_x, KeyboardKey::X},
            {XK_y, KeyboardKey::Y},
            {XK_z, KeyboardKey::Z},
            {XK_ssharp, KeyboardKey::LEFT_BRACKET},
            {XK_adiaeresis, KeyboardKey::QUOTE},
            {XK_odiaeresis, KeyboardKey::GRAVE},
            {XK_udiaeresis, KeyboardKey::SEMICOLON},
            {XK_Super_L, KeyboardKey::LEFT_SUPER},
            {XK_Super_R, KeyboardKey::RIGHT_SUPER}
        };

        KeyboardKey InputManagerLinux::convertKeyCode(KeySym keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        uint32_t InputManagerLinux::getModifiers(unsigned int state)
        {
            uint32_t modifiers = 0;

            if (state & ShiftMask) modifiers |= SHIFT_DOWN;
            if (state & ControlMask) modifiers |= CONTROL_DOWN;
            if (state & Button1Mask) modifiers |= LEFT_MOUSE_DOWN;
            if (state & Button2Mask) modifiers |= RIGHT_MOUSE_DOWN;
            if (state & Button3Mask) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }
#else
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

        KeyboardKey InputManagerLinux::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }
#endif
        uint32_t InputManagerLinux::getModifiers() const
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

        InputManagerLinux::InputManagerLinux()
        {
            std::fill(std::begin(keyboardKeyDown), std::end(keyboardKeyDown), false);
            std::fill(std::begin(mouseButtonDown), std::end(mouseButtonDown), false);

#if OUZEL_SUPPORTS_X11
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            Display* display = engineLinux->getDisplay();

            char data[1] = {0};

            Pixmap pixmap = XCreateBitmapFromData(display, DefaultRootWindow(display), data, 1, 1);
            if (pixmap)
            {
                XColor color;
                color.red = color.green = color.blue = 0;

                emptyCursor = XCreatePixmapCursor(display, pixmap, pixmap, &color, &color, 0, 0);
                XFreePixmap(display, pixmap);
            }
#endif

            DIR* dir = opendir("/dev/input");

            if (!dir)
                throw SystemError("Failed to open directory");

            dirent ent;
            dirent* p;

            while (readdir_r(dir, &ent, &p) == 0 && p)
            {
                if (strncmp("event", ent.d_name, 5) == 0)
                {
                    try
                    {
                        std::string filename = std::string("/dev/input/") + ent.d_name;
                        EventDevice inputDevice(filename);

                        if (inputDevice.getDeviceClass() != EventDevice::CLASS_NONE)
                        {
                            if (inputDevice.getFd() > maxFd) maxFd = inputDevice.getFd();

                            inputDevices.push_back(std::move(inputDevice));
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }

            closedir(dir);
        }

        InputManagerLinux::~InputManagerLinux()
        {
#if OUZEL_SUPPORTS_X11
            if (engine)
            {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                if (emptyCursor != None) XFreeCursor(engineLinux->getDisplay(), emptyCursor);
            }
#endif
        }

        void InputManagerLinux::update()
        {
            fd_set rfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);

            for (const EventDevice& inputDevice : inputDevices)
                FD_SET(inputDevice.getFd(), &rfds);

            int retval = select(maxFd + 1, &rfds, nullptr, nullptr, &tv);

            if (retval == -1)
                throw SystemError("Select failed");
            else if (retval > 0)
            {
                static char TEMP_BUFFER[256];

                for (const EventDevice& inputDevice : inputDevices)
                {
                    if (FD_ISSET(inputDevice.getFd(), &rfds))
                    {
                        // TODO: buffer data
                        // TODO: move this code to EventDevice
                        ssize_t bytesRead = read(inputDevice.getFd(), TEMP_BUFFER, sizeof(TEMP_BUFFER));

                        if (bytesRead == -1)
                            throw SystemError("Failed to read from " + inputDevice.getFilename()); // TODO: disconnect the device

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
                                        absolutePos.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0F)).x;
                                    else if (event->code == ABS_Y)
                                        absolutePos.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event->value))).y;

                                    mouseMove(absolutePos, getModifiers());
                                }
                                else if (event->type == EV_REL)
                                {
                                    Vector2 relativePos;

                                    if (event->code == REL_X)
                                        relativePos.x = static_cast<float>(event->value);
                                    else if (event->code == REL_Y)
                                        relativePos.y = static_cast<float>(event->value);

                                    mouseRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos), getModifiers());
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

        void InputManagerLinux::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

#if OUZEL_SUPPORTS_X11
            if (engine)
            {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                NativeCursorLinux* cursorLinux = static_cast<NativeCursorLinux*>(resource);

                if (cursorLinux)
                {
                    currentCursor = cursorLinux->getNativeCursor();
                    if (!currentCursor) currentCursor = emptyCursor;
                }
                else
                    currentCursor = None;

                if (cursorVisible)
                {
                    if (currentCursor)
                        XDefineCursor(display, window, currentCursor);
                    else
                        XUndefineCursor(display, window);
                }
                else
                    XDefineCursor(display, window, emptyCursor);
            }
#endif
        }

        NativeCursor* InputManagerLinux::createNativeCursor()
        {
            std::unique_lock<std::mutex> lock(resourceMutex);

            std::unique_ptr<NativeCursorLinux> cursorResource(new NativeCursorLinux(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerLinux::setCursorVisible(bool visible)
        {
            if (visible != cursorVisible)
            {
                cursorVisible = visible;

#if OUZEL_SUPPORTS_X11
                engine->executeOnMainThread([visible, this] {
                    EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                    NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                    Display* display = engineLinux->getDisplay();
                    ::Window window = windowLinux->getNativeWindow();

                    if (visible)
                    {
                        if (currentCursor)
                            XDefineCursor(display, window, currentCursor);
                        else
                            XUndefineCursor(display, window);
                    }
                    else
                        XDefineCursor(display, window, emptyCursor);

                    XFlush(display);
                });
#endif
            }
        }

        bool InputManagerLinux::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerLinux::setCursorLocked(bool locked)
        {
#if OUZEL_SUPPORTS_X11
            engine->executeOnMainThread([locked] {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                if (locked)
                {
                    if (XGrabPointer(display, window, False,
                                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask,
                                     GrabModeAsync, GrabModeAsync,
                                     None, None, CurrentTime) != GrabSuccess)
                        throw SystemError("Failed to grab pointer");
                }
                else
                    XUngrabPointer(display, CurrentTime);

                XSync(display, False);
            });
#endif
            cursorLocked = locked;
        }

        bool InputManagerLinux::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputManagerLinux::setCursorPosition(const Vector2& position)
        {
            InputManager::setCursorPosition(position);

#if OUZEL_SUPPORTS_X11
            engine->executeOnMainThread([position] {
                EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
                NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
                Display* display = engineLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                XWindowAttributes attributes;
                XGetWindowAttributes(display, window, &attributes);

                Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

                XWarpPointer(display, None, window, 0, 0, 0, 0,
                             attributes.x + static_cast<int>(windowLocation.x),
                             attributes.y + static_cast<int>(windowLocation.y));
                XSync(display, False);
            });
#endif
        }

        void InputManagerLinux::startDeviceDiscovery()
        {
            DIR* dir = opendir("/dev/input");

            if (!dir)
                throw SystemError("Failed to open directory");

            dirent ent;
            dirent* p;

            while (readdir_r(dir, &ent, &p) == 0 && p)
            {
                if (strncmp("event", ent.d_name, 5) == 0)
                {
                    try
                    {
                        std::string filename = std::string("/dev/input/") + ent.d_name;

                        bool found = false;
                        for (const EventDevice& inputDevice : inputDevices)
                        {
                            if (inputDevice.getFilename() == filename)
                            {
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                        {
                            EventDevice inputDevice(filename);

                            if (inputDevice.getDeviceClass() != EventDevice::CLASS_NONE)
                            {
                                if (inputDevice.getFd() > maxFd) maxFd = inputDevice.getFd();

                                inputDevices.push_back(std::move(inputDevice));
                            }
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }

            closedir(dir);
        }
    } // namespace input
} // namespace ouzel
