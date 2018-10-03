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
#include "InputSystemLinux.hpp"
#include "events/Event.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
#if OUZEL_SUPPORTS_X11
        static const std::unordered_map<KeySym, Keyboard::Key> keyMap = {
            {XK_BackSpace, Keyboard::Key::BACKSPACE},
            {XK_Tab, Keyboard::Key::TAB},
            {XK_ISO_Left_Tab, Keyboard::Key::TAB},
            {XK_Linefeed, Keyboard::Key::NONE}, // ?
            {XK_Clear, Keyboard::Key::CLEAR},
            {XK_Return, Keyboard::Key::RETURN},
            {XK_Pause, Keyboard::Key::PAUSE},
            {XK_Scroll_Lock, Keyboard::Key::SCROLL},
            {XK_Sys_Req, Keyboard::Key::NONE}, // ?
            {XK_Escape, Keyboard::Key::ESCAPE},
            {XK_Insert, Keyboard::Key::INSERT},
            {XK_Delete, Keyboard::Key::DEL},
            {XK_Home, Keyboard::Key::HOME},
            {XK_Left, Keyboard::Key::LEFT},
            {XK_Up, Keyboard::Key::UP},
            {XK_Right, Keyboard::Key::RIGHT},
            {XK_Down, Keyboard::Key::DOWN},
            {XK_Prior, Keyboard::Key::PRIOR}, // also XK_Page_Up
            {XK_Next, Keyboard::Key::NEXT}, // also XK_Page_Down
            {XK_End, Keyboard::Key::END},
            {XK_Begin, Keyboard::Key::HOME},
            {XK_Num_Lock, Keyboard::Key::NUMLOCK},
            {XK_KP_Space, Keyboard::Key::SPACE},
            {XK_KP_Tab, Keyboard::Key::TAB},
            {XK_KP_Enter, Keyboard::Key::RETURN},
            {XK_KP_F1, Keyboard::Key::F1},
            {XK_KP_F2, Keyboard::Key::F2},
            {XK_KP_F3, Keyboard::Key::F3},
            {XK_KP_F4, Keyboard::Key::F4},
            {XK_KP_Home, Keyboard::Key::HOME},
            {XK_KP_Left, Keyboard::Key::LEFT},
            {XK_KP_Up, Keyboard::Key::UP},
            {XK_KP_Right, Keyboard::Key::RIGHT},
            {XK_KP_Down, Keyboard::Key::DOWN},
            {XK_Print, Keyboard::Key::PRINT},
            {XK_KP_Prior, Keyboard::Key::PRIOR}, // alos XK_KP_Page_Up
            {XK_KP_Next, Keyboard::Key::NEXT}, // also XK_KP_Page_Down
            {XK_KP_End, Keyboard::Key::END},
            {XK_KP_Begin, Keyboard::Key::HOME},
            {XK_KP_Insert, Keyboard::Key::INSERT},
            {XK_KP_Delete, Keyboard::Key::DEL},
            {XK_KP_Equal, Keyboard::Key::EQUAL},
            {XK_KP_Multiply, Keyboard::Key::MULTIPLY},
            {XK_KP_Add, Keyboard::Key::ADD},
            {XK_KP_Separator, Keyboard::Key::SEPARATOR},
            {XK_KP_Subtract, Keyboard::Key::SUBTRACT},
            {XK_KP_Decimal, Keyboard::Key::DECIMAL},
            {XK_KP_Divide, Keyboard::Key::DIVIDE},
            {XK_KP_0, Keyboard::Key::NUMPAD_0},
            {XK_KP_1, Keyboard::Key::NUMPAD_1},
            {XK_KP_2, Keyboard::Key::NUMPAD_2},
            {XK_KP_3, Keyboard::Key::NUMPAD_3},
            {XK_KP_4, Keyboard::Key::NUMPAD_4},
            {XK_KP_5, Keyboard::Key::NUMPAD_5},
            {XK_KP_6, Keyboard::Key::NUMPAD_6},
            {XK_KP_7, Keyboard::Key::NUMPAD_7},
            {XK_KP_8, Keyboard::Key::NUMPAD_8},
            {XK_KP_9, Keyboard::Key::NUMPAD_9},
            {XK_F1, Keyboard::Key::F1},
            {XK_F2, Keyboard::Key::F2},
            {XK_F3, Keyboard::Key::F3},
            {XK_F4, Keyboard::Key::F4},
            {XK_F5, Keyboard::Key::F5},
            {XK_F6, Keyboard::Key::F6},
            {XK_F7, Keyboard::Key::F7},
            {XK_F8, Keyboard::Key::F8},
            {XK_F9, Keyboard::Key::F9},
            {XK_F10, Keyboard::Key::F10},
            {XK_F11, Keyboard::Key::F11},
            {XK_F12, Keyboard::Key::F12},
            {XK_Shift_L, Keyboard::Key::LEFT_SHIFT},
            {XK_Shift_R, Keyboard::Key::RIGHT_SHIFT},
            {XK_Control_L, Keyboard::Key::LEFT_CONTROL},
            {XK_Control_R, Keyboard::Key::RIGHT_CONTROL},
            {XK_Caps_Lock, Keyboard::Key::CAPITAL},
            {XK_Shift_Lock, Keyboard::Key::CAPITAL},
            {XK_Meta_L, Keyboard::Key::LEFT_SUPER},
            {XK_Meta_R, Keyboard::Key::RIGHT_SUPER},
            {XK_Alt_L, Keyboard::Key::LEFT_ALT},
            {XK_Alt_R, Keyboard::Key::RIGHT_ALT},
            {XK_ISO_Level3_Shift, Keyboard::Key::RIGHT_SUPER},
            {XK_Menu, Keyboard::Key::LEFT_SUPER},
            {XK_space, Keyboard::Key::SPACE},
            {XK_exclam, Keyboard::Key::NUM_1},
            {XK_quotedbl, Keyboard::Key::NUM_2},
            {XK_section, Keyboard::Key::NONE}, // ?
            {XK_numbersign, Keyboard::Key::SLASH},
            {XK_dollar, Keyboard::Key::NUM_4},
            {XK_percent, Keyboard::Key::NUM_5},
            {XK_ampersand, Keyboard::Key::NUM_7},
            {XK_apostrophe, Keyboard::Key::QUOTE},
            {XK_parenleft, Keyboard::Key::NUM_9},
            {XK_parenright, Keyboard::Key::NUM_0},
            {XK_asterisk, Keyboard::Key::NUM_8},
            {XK_plus, Keyboard::Key::PLUS},
            {XK_comma, Keyboard::Key::COMMA},
            {XK_minus, Keyboard::Key::MINUS},
            {XK_period, Keyboard::Key::PERIOD},
            {XK_slash, Keyboard::Key::SLASH},
            {XK_0, Keyboard::Key::NUM_0},
            {XK_1, Keyboard::Key::NUM_1},
            {XK_2, Keyboard::Key::NUM_2},
            {XK_3, Keyboard::Key::NUM_3},
            {XK_4, Keyboard::Key::NUM_4},
            {XK_5, Keyboard::Key::NUM_5},
            {XK_6, Keyboard::Key::NUM_6},
            {XK_7, Keyboard::Key::NUM_7},
            {XK_8, Keyboard::Key::NUM_8},
            {XK_9, Keyboard::Key::NUM_9},
            {XK_colon, Keyboard::Key::SEMICOLON},
            {XK_semicolon, Keyboard::Key::SEMICOLON},
            {XK_less, Keyboard::Key::COMMA},
            {XK_equal, Keyboard::Key::PLUS},
            {XK_greater, Keyboard::Key::PERIOD},
            {XK_question, Keyboard::Key::SLASH},
            {XK_at, Keyboard::Key::NUM_2}, // ?
            {XK_mu, Keyboard::Key::NONE}, // ?
            {XK_EuroSign, Keyboard::Key::NONE}, // ?
            {XK_A, Keyboard::Key::A},
            {XK_B, Keyboard::Key::B},
            {XK_C, Keyboard::Key::C},
            {XK_D, Keyboard::Key::D},
            {XK_E, Keyboard::Key::E},
            {XK_F, Keyboard::Key::F},
            {XK_G, Keyboard::Key::G},
            {XK_H, Keyboard::Key::H},
            {XK_I, Keyboard::Key::I},
            {XK_J, Keyboard::Key::J},
            {XK_K, Keyboard::Key::K},
            {XK_L, Keyboard::Key::L},
            {XK_M, Keyboard::Key::M},
            {XK_N, Keyboard::Key::N},
            {XK_O, Keyboard::Key::O},
            {XK_P, Keyboard::Key::P},
            {XK_Q, Keyboard::Key::Q},
            {XK_R, Keyboard::Key::R},
            {XK_S, Keyboard::Key::S},
            {XK_T, Keyboard::Key::T},
            {XK_U, Keyboard::Key::U},
            {XK_V, Keyboard::Key::V},
            {XK_W, Keyboard::Key::W},
            {XK_X, Keyboard::Key::X},
            {XK_Y, Keyboard::Key::Y},
            {XK_Z, Keyboard::Key::Z},
            {XK_bracketleft, Keyboard::Key::LEFT_BRACKET},
            {XK_backslash, Keyboard::Key::BACKSLASH},
            {XK_bracketright, Keyboard::Key::RIGHT_BRACKET},
            {XK_asciicircum, Keyboard::Key::BACKSLASH},
            {XK_dead_circumflex, Keyboard::Key::BACKSLASH},
            {XK_degree, Keyboard::Key::NONE}, // ?
            {XK_underscore, Keyboard::Key::MINUS},
            {XK_grave, Keyboard::Key::GRAVE},
            {XK_dead_grave, Keyboard::Key::GRAVE},
            {XK_acute, Keyboard::Key::RIGHT_BRACKET},
            {XK_dead_acute, Keyboard::Key::RIGHT_BRACKET},
            {XK_a, Keyboard::Key::A},
            {XK_b, Keyboard::Key::B},
            {XK_c, Keyboard::Key::C},
            {XK_d, Keyboard::Key::D},
            {XK_e, Keyboard::Key::E},
            {XK_f, Keyboard::Key::F},
            {XK_g, Keyboard::Key::G},
            {XK_h, Keyboard::Key::H},
            {XK_i, Keyboard::Key::I},
            {XK_j, Keyboard::Key::J},
            {XK_k, Keyboard::Key::K},
            {XK_l, Keyboard::Key::L},
            {XK_m, Keyboard::Key::M},
            {XK_n, Keyboard::Key::N},
            {XK_o, Keyboard::Key::O},
            {XK_p, Keyboard::Key::P},
            {XK_q, Keyboard::Key::Q},
            {XK_r, Keyboard::Key::R},
            {XK_s, Keyboard::Key::S},
            {XK_t, Keyboard::Key::T},
            {XK_u, Keyboard::Key::U},
            {XK_v, Keyboard::Key::V},
            {XK_w, Keyboard::Key::W},
            {XK_x, Keyboard::Key::X},
            {XK_y, Keyboard::Key::Y},
            {XK_z, Keyboard::Key::Z},
            {XK_ssharp, Keyboard::Key::LEFT_BRACKET},
            {XK_adiaeresis, Keyboard::Key::QUOTE},
            {XK_odiaeresis, Keyboard::Key::GRAVE},
            {XK_udiaeresis, Keyboard::Key::SEMICOLON},
            {XK_Super_L, Keyboard::Key::LEFT_SUPER},
            {XK_Super_R, Keyboard::Key::RIGHT_SUPER}
        };

        Keyboard::Key InputSystemLinux::convertKeyCode(KeySym keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        uint32_t InputSystemLinux::getModifiers(unsigned int state)
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
        static const std::unordered_map<uint16_t, Keyboard::Key> keyMap = {
            {KEY_ESC, Keyboard::Key::ESCAPE},
            {KEY_1, Keyboard::Key::NUM_1},
            {KEY_2, Keyboard::Key::NUM_2},
            {KEY_3, Keyboard::Key::NUM_3},
            {KEY_4, Keyboard::Key::NUM_4},
            {KEY_5, Keyboard::Key::NUM_5},
            {KEY_6, Keyboard::Key::NUM_6},
            {KEY_7, Keyboard::Key::NUM_7},
            {KEY_8, Keyboard::Key::NUM_8},
            {KEY_9, Keyboard::Key::NUM_9},
            {KEY_0, Keyboard::Key::NUM_0},
            {KEY_MINUS, Keyboard::Key::MINUS},
            {KEY_EQUAL, Keyboard::Key::PLUS},
            {KEY_BACKSPACE, Keyboard::Key::BACKSPACE},
            {KEY_TAB, Keyboard::Key::TAB},
            {KEY_Q, Keyboard::Key::Q},
            {KEY_W, Keyboard::Key::W},
            {KEY_E, Keyboard::Key::E},
            {KEY_R, Keyboard::Key::R},
            {KEY_T, Keyboard::Key::T},
            {KEY_Y, Keyboard::Key::Y},
            {KEY_U, Keyboard::Key::U},
            {KEY_I, Keyboard::Key::I},
            {KEY_O, Keyboard::Key::O},
            {KEY_P, Keyboard::Key::P},
            {KEY_LEFTBRACE, Keyboard::Key::LEFT_BRACKET},
            {KEY_RIGHTBRACE, Keyboard::Key::RIGHT_BRACKET},
            {KEY_ENTER, Keyboard::Key::RETURN},
            {KEY_LEFTCTRL, Keyboard::Key::LEFT_CONTROL},
            {KEY_A, Keyboard::Key::A},
            {KEY_S, Keyboard::Key::S},
            {KEY_D, Keyboard::Key::D},
            {KEY_F, Keyboard::Key::F},
            {KEY_G, Keyboard::Key::G},
            {KEY_H, Keyboard::Key::H},
            {KEY_J, Keyboard::Key::J},
            {KEY_K, Keyboard::Key::K},
            {KEY_L, Keyboard::Key::L},
            {KEY_SEMICOLON, Keyboard::Key::SEMICOLON},
            {KEY_APOSTROPHE, Keyboard::Key::QUOTE},
            {KEY_GRAVE, Keyboard::Key::GRAVE},
            {KEY_LEFTSHIFT, Keyboard::Key::LEFT_SHIFT},
            {KEY_BACKSLASH, Keyboard::Key::BACKSLASH},
            {KEY_Z, Keyboard::Key::Z},
            {KEY_X, Keyboard::Key::X},
            {KEY_C, Keyboard::Key::C},
            {KEY_V, Keyboard::Key::V},
            {KEY_B, Keyboard::Key::B},
            {KEY_N, Keyboard::Key::N},
            {KEY_M, Keyboard::Key::M},
            {KEY_COMMA, Keyboard::Key::COMMA},
            {KEY_DOT, Keyboard::Key::PERIOD},
            {KEY_SLASH, Keyboard::Key::SLASH},
            {KEY_RIGHTSHIFT, Keyboard::Key::RIGHT_SHIFT},
            {KEY_KPASTERISK, Keyboard::Key::MULTIPLY},
            {KEY_LEFTALT, Keyboard::Key::LEFT_ALT},
            {KEY_SPACE, Keyboard::Key::SPACE},
            {KEY_CAPSLOCK, Keyboard::Key::CAPITAL},
            {KEY_F1, Keyboard::Key::F1},
            {KEY_F2, Keyboard::Key::F2},
            {KEY_F3, Keyboard::Key::F3},
            {KEY_F4, Keyboard::Key::F4},
            {KEY_F5, Keyboard::Key::F5},
            {KEY_F6, Keyboard::Key::F6},
            {KEY_F7, Keyboard::Key::F7},
            {KEY_F8, Keyboard::Key::F8},
            {KEY_F9, Keyboard::Key::F9},
            {KEY_F10, Keyboard::Key::F10},
            {KEY_NUMLOCK, Keyboard::Key::NUMLOCK},
            {KEY_SCROLLLOCK, Keyboard::Key::SCROLL},
            {KEY_KP7, Keyboard::Key::NUMPAD_7},
            {KEY_KP8, Keyboard::Key::NUMPAD_8},
            {KEY_KP9, Keyboard::Key::NUMPAD_9},
            {KEY_KPMINUS, Keyboard::Key::SUBTRACT},
            {KEY_KP4, Keyboard::Key::NUMPAD_4},
            {KEY_KP5, Keyboard::Key::NUMPAD_5},
            {KEY_KP6, Keyboard::Key::NUMPAD_6},
            {KEY_KPPLUS, Keyboard::Key::ADD},
            {KEY_KP1, Keyboard::Key::NUMPAD_1},
            {KEY_KP2, Keyboard::Key::NUMPAD_2},
            {KEY_KP3, Keyboard::Key::NUMPAD_3},
            {KEY_KP0, Keyboard::Key::NUMPAD_0},
            {KEY_KPDOT, Keyboard::Key::DECIMAL},

            {KEY_ZENKAKUHANKAKU, Keyboard::Key::NONE}, // ??
            {KEY_102ND, Keyboard::Key::LESS},
            {KEY_F11, Keyboard::Key::F11},
            {KEY_F12, Keyboard::Key::F12},
            {KEY_RO, Keyboard::Key::NONE}, // ??
            {KEY_KATAKANA, Keyboard::Key::NONE}, // ??
            {KEY_HIRAGANA, Keyboard::Key::NONE}, // ??
            {KEY_HENKAN, Keyboard::Key::NONE}, // ??
            {KEY_KATAKANAHIRAGANA, Keyboard::Key::NONE}, // ??
            {KEY_MUHENKAN, Keyboard::Key::NONE}, // ??
            {KEY_KPJPCOMMA, Keyboard::Key::NONE}, // ??
            {KEY_KPENTER, Keyboard::Key::RETURN},
            {KEY_RIGHTCTRL, Keyboard::Key::RIGHT_CONTROL},
            {KEY_KPSLASH, Keyboard::Key::DIVIDE},
            {KEY_SYSRQ, Keyboard::Key::NONE}, // ??
            {KEY_RIGHTALT, Keyboard::Key::RIGHT_ALT},
            {KEY_LINEFEED, Keyboard::Key::NONE}, // ??
            {KEY_HOME, Keyboard::Key::HOME},
            {KEY_UP, Keyboard::Key::UP},
            {KEY_PAGEUP, Keyboard::Key::PRIOR},
            {KEY_LEFT, Keyboard::Key::LEFT},
            {KEY_RIGHT, Keyboard::Key::RIGHT},
            {KEY_END, Keyboard::Key::END},
            {KEY_DOWN, Keyboard::Key::DOWN},
            {KEY_PAGEDOWN, Keyboard::Key::NEXT},
            {KEY_INSERT, Keyboard::Key::INSERT},
            {KEY_DELETE, Keyboard::Key::DEL},
            {KEY_MACRO, Keyboard::Key::NONE}, // ??
            {KEY_MUTE, Keyboard::Key::NONE}, // ??
            {KEY_VOLUMEDOWN, Keyboard::Key::NONE}, // ??
            {KEY_VOLUMEUP, Keyboard::Key::NONE}, // ??
            {KEY_POWER, Keyboard::Key::NONE}, // ??
            {KEY_KPEQUAL, Keyboard::Key::EQUAL},
            {KEY_KPPLUSMINUS, Keyboard::Key::NONE}, // ??
            {KEY_PAUSE, Keyboard::Key::PAUSE},
            {KEY_SCALE, Keyboard::Key::NONE}, //?

            {KEY_KPCOMMA, Keyboard::Key::SEPARATOR},
            {KEY_HANGEUL, Keyboard::Key::NONE}, // KEY_HANGUEL
            {KEY_HANJA, Keyboard::Key::HANJA},
            {KEY_YEN, Keyboard::Key::NONE}, // ??
            {KEY_LEFTMETA, Keyboard::Key::LEFT_SUPER},
            {KEY_RIGHTMETA, Keyboard::Key::RIGHT_SUPER},
            {KEY_COMPOSE, Keyboard::Key::NONE}, // ??

            {KEY_F13, Keyboard::Key::F13},
            {KEY_F14, Keyboard::Key::F14},
            {KEY_F15, Keyboard::Key::F15},
            {KEY_F16, Keyboard::Key::F16},
            {KEY_F17, Keyboard::Key::F17},
            {KEY_F18, Keyboard::Key::F18},
            {KEY_F19, Keyboard::Key::F19},
            {KEY_F20, Keyboard::Key::F20},
            {KEY_F21, Keyboard::Key::F21},
            {KEY_F22, Keyboard::Key::F22},
            {KEY_F23, Keyboard::Key::F23},
            {KEY_F24, Keyboard::Key::F24}
        };

        Keyboard::Key InputSystemLinux::convertKeyCode(uint16_t keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }
#endif

        uint32_t InputSystemLinux::getModifiers() const
        {
            uint32_t modifiers = 0;

            /*if (keyboardKeyDown[KEY_LEFTSHIFT] || keyboardKeyDown[KEY_RIGHTSHIFT]) modifiers |= SHIFT_DOWN;
            if (keyboardKeyDown[KEY_LEFTALT] || keyboardKeyDown[KEY_RIGHTALT]) modifiers |= ALT_DOWN;
            if (keyboardKeyDown[KEY_LEFTCTRL] || keyboardKeyDown[KEY_RIGHTCTRL]) modifiers |= CONTROL_DOWN;
            if (keyboardKeyDown[KEY_LEFTMETA] || keyboardKeyDown[KEY_RIGHTMETA]) modifiers |= SUPER_DOWN;

            if (mouseButtonDown[0]) modifiers |= LEFT_MOUSE_DOWN;
            if (mouseButtonDown[1]) modifiers |= RIGHT_MOUSE_DOWN;
            if (mouseButtonDown[2]) modifiers |= MIDDLE_MOUSE_DOWN;*/

            return modifiers;
        }

        InputSystemLinux::InputSystemLinux()
        {
#if OUZEL_SUPPORTS_X11
            Event keyboardConnectEvent;
            keyboardConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<KeyboardDevice> keyboard(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardConnectEvent.deviceId = keyboard->getId();
            keyboardConnectEvent.deviceType = Controller::Type::KEYBOARD;
            keyboardDevice = keyboard.get();
            inputDevices.insert(std::make_pair(keyboard->getId(), std::move(keyboard)));
            addEvent(keyboardConnectEvent);

            Event mouseConnectEvent;
            mouseConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<MouseDeviceLinux> mouse(new MouseDeviceLinux(*this, ++lastDeviceId));
            mouseConnectEvent.deviceId = mouse->getId();
            mouseConnectEvent.deviceType = Controller::Type::MOUSE;
            mouseDevice = mouse.get();
            inputDevices.insert(std::make_pair(mouse->getId(), std::move(mouse)));
            addEvent(mouseConnectEvent);

            Event touchpadConnectEvent;
            touchpadConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<TouchpadDevice> touchpad(new TouchpadDevice(*this, ++lastDeviceId));
            touchpadConnectEvent.deviceId = touchpad->getId();
            touchpadConnectEvent.deviceType = Controller::Type::TOUCHPAD;
            touchpadDevice = touchpad.get();
            inputDevices.insert(std::make_pair(touchpad->getId(), std::move(touchpad)));
            addEvent(touchpadConnectEvent);
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

                            eventDevices.push_back(std::move(inputDevice));
                        }
                    }
                    catch (...)
                    {
                    }
                }
            }

            closedir(dir);
        }

        InputSystemLinux::~InputSystemLinux()
        {
        }

        void InputSystemLinux::executeCommand(Command command)
        {
            switch (command.type)
            {
            case Command::Type::START_DEVICE_DISCOVERY:
                break;
            case Command::Type::STOP_DEVICE_DISCOVERY:
                break;
            case Command::Type::SET_ABSOLUTE_DPAD_VALUES:
            {
                break;
            }
            case Command::Type::SET_PLAYER_INDEX:
            {
                break;
            }
            case Command::Type::SET_VIBRATION:
            {
                break;
            }
            case Command::Type::SET_POSITON:
            {
                break;
            }
            case Command::Type::SET_CURSOR:
            {
                break;
            }
            case Command::Type::SET_CURSOR_VISIBLE:
            {
                break;
            }
            case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                break;
            case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                break;
            }
        }

        void InputSystemLinux::update()
        {
            fd_set rfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;

            FD_ZERO(&rfds);

            for (const EventDevice& inputDevice : eventDevices)
                FD_SET(inputDevice.getFd(), &rfds);

            int retval = select(maxFd + 1, &rfds, nullptr, nullptr, &tv);

            if (retval == -1)
                throw SystemError("Select failed");
            else if (retval > 0)
            {
                static char TEMP_BUFFER[256];

                for (const EventDevice& inputDevice : eventDevices)
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
                                        //keyPress(convertKeyCode(event->code), getModifiers());
                                    }
                                    else if (event->value == 0) // release
                                    {
                                        //keyRelease(convertKeyCode(event->code), getModifiers());
                                    }
                                }
                            }
                            if (inputDevice.getDeviceClass() & EventDevice::CLASS_MOUSE)
                            {
                                if (event->type == EV_ABS)
                                {
                                    /*Vector2 absolutePos = cursorPosition;

                                    if (event->code == ABS_X)
                                        absolutePos.x = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(static_cast<float>(event->value), 0.0F)).x;
                                    else if (event->code == ABS_Y)
                                        absolutePos.y = engine->getWindow()->convertWindowToNormalizedLocation(Vector2(0.0F, static_cast<float>(event->value))).y;

                                    mouseMove(absolutePos, getModifiers());*/
                                }
                                else if (event->type == EV_REL)
                                {
                                    Vector2 relativePos;

                                    if (event->code == REL_X)
                                        relativePos.x = static_cast<float>(event->value);
                                    else if (event->code == REL_Y)
                                        relativePos.y = static_cast<float>(event->value);

                                    //mouseRelativeMove(engine->getWindow()->convertWindowToNormalizedLocation(relativePos), getModifiers());
                                }
                                else if (event->type == EV_KEY)
                                {
                                    Mouse::Button button;
                                    int buttonIndex = -1;

                                    switch (event->code)
                                    {
                                    case BTN_LEFT:
                                        button = Mouse::Button::LEFT;
                                        buttonIndex = 0;
                                        break;
                                    case BTN_RIGHT:
                                        button = Mouse::Button::RIGHT;
                                        buttonIndex =  1;
                                        break;
                                    case BTN_MIDDLE:
                                        button = Mouse::Button::MIDDLE;
                                        buttonIndex = 2;
                                        break;
                                    default:
                                        button = Mouse::Button::NONE;
                                    }

                                    if (event->value == 1)
                                    {
                                        //mouseButtonPress(button, cursorPosition, getModifiers());
                                    }
                                    else if (event->value == 0)
                                    {
                                        //mouseButtonRelease(button, cursorPosition, getModifiers());
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
    } // namespace input
} // namespace ouzel
