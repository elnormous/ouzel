// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include "GL/gl.h"
#  include "GL/glext.h"
#  include <X11/XKBlib.h>
#  include <X11/keysym.h>
#  include <X11/X.h>
#  include <X11/extensions/scrnsaver.h>
#  include <X11/extensions/XInput2.h>
#endif
#include "EngineLinux.hpp"
#include "NativeWindowLinux.hpp"
#include "events/Event.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/linux/InputSystemLinux.hpp"
#include "utils/Log.hpp"

#if OUZEL_SUPPORTS_X11
static const std::unordered_map<KeySym, ouzel::input::Keyboard::Key> keyMap = {
    {XK_BackSpace, ouzel::input::Keyboard::Key::BACKSPACE},
    {XK_Tab, ouzel::input::Keyboard::Key::TAB},
    {XK_ISO_Left_Tab, ouzel::input::Keyboard::Key::TAB},
    {XK_Linefeed, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_Clear, ouzel::input::Keyboard::Key::CLEAR},
    {XK_Return, ouzel::input::Keyboard::Key::ENTER},
    {XK_Pause, ouzel::input::Keyboard::Key::PAUSE},
    {XK_Scroll_Lock, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {XK_Sys_Req, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_Escape, ouzel::input::Keyboard::Key::ESCAPE},
    {XK_Insert, ouzel::input::Keyboard::Key::INSERT},
    {XK_Delete, ouzel::input::Keyboard::Key::DEL},
    {XK_Home, ouzel::input::Keyboard::Key::HOME},
    {XK_Left, ouzel::input::Keyboard::Key::LEFT},
    {XK_Up, ouzel::input::Keyboard::Key::UP},
    {XK_Right, ouzel::input::Keyboard::Key::RIGHT},
    {XK_Down, ouzel::input::Keyboard::Key::DOWN},
    {XK_Prior, ouzel::input::Keyboard::Key::PAGE_UP}, // also XK_Page_Up
    {XK_Next, ouzel::input::Keyboard::Key::PAGE_DOWN}, // also XK_Page_Down
    {XK_End, ouzel::input::Keyboard::Key::END},
    {XK_Begin, ouzel::input::Keyboard::Key::HOME},
    {XK_Num_Lock, ouzel::input::Keyboard::Key::NUM_LOCK},
    {XK_KP_Space, ouzel::input::Keyboard::Key::SPACE},
    {XK_KP_Tab, ouzel::input::Keyboard::Key::TAB},
    {XK_KP_Enter, ouzel::input::Keyboard::Key::NUMPAD_ENTER},
    {XK_KP_F1, ouzel::input::Keyboard::Key::F1},
    {XK_KP_F2, ouzel::input::Keyboard::Key::F2},
    {XK_KP_F3, ouzel::input::Keyboard::Key::F3},
    {XK_KP_F4, ouzel::input::Keyboard::Key::F4},
    {XK_KP_Home, ouzel::input::Keyboard::Key::NUMPAD_7},
    {XK_KP_Left, ouzel::input::Keyboard::Key::NUMPAD_4},
    {XK_KP_Up, ouzel::input::Keyboard::Key::NUMPAD_8},
    {XK_KP_Right, ouzel::input::Keyboard::Key::NUMPAD_6},
    {XK_KP_Down, ouzel::input::Keyboard::Key::NUMPAD_2},
    {XK_Print, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {XK_KP_Prior, ouzel::input::Keyboard::Key::NUMPAD_9}, // also XK_KP_Page_Up
    {XK_KP_Next, ouzel::input::Keyboard::Key::NUMPAD_3}, // also XK_KP_Page_Down
    {XK_KP_End, ouzel::input::Keyboard::Key::NUMPAD_1},
    {XK_KP_Begin, ouzel::input::Keyboard::Key::NUMPAD_5},
    {XK_KP_Insert, ouzel::input::Keyboard::Key::NUMPAD_0},
    {XK_KP_Delete, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {XK_KP_Equal, ouzel::input::Keyboard::Key::NUMPAD_EQUAL},
    {XK_KP_Multiply, ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {XK_KP_Add, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {XK_KP_Separator, ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR},
    {XK_KP_Subtract, ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {XK_KP_Decimal, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {XK_KP_Divide, ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},
    {XK_KP_0, ouzel::input::Keyboard::Key::NUMPAD_0},
    {XK_KP_1, ouzel::input::Keyboard::Key::NUMPAD_1},
    {XK_KP_2, ouzel::input::Keyboard::Key::NUMPAD_2},
    {XK_KP_3, ouzel::input::Keyboard::Key::NUMPAD_3},
    {XK_KP_4, ouzel::input::Keyboard::Key::NUMPAD_4},
    {XK_KP_5, ouzel::input::Keyboard::Key::NUMPAD_5},
    {XK_KP_6, ouzel::input::Keyboard::Key::NUMPAD_6},
    {XK_KP_7, ouzel::input::Keyboard::Key::NUMPAD_7},
    {XK_KP_8, ouzel::input::Keyboard::Key::NUMPAD_8},
    {XK_KP_9, ouzel::input::Keyboard::Key::NUMPAD_9},
    {XK_F1, ouzel::input::Keyboard::Key::F1},
    {XK_F2, ouzel::input::Keyboard::Key::F2},
    {XK_F3, ouzel::input::Keyboard::Key::F3},
    {XK_F4, ouzel::input::Keyboard::Key::F4},
    {XK_F5, ouzel::input::Keyboard::Key::F5},
    {XK_F6, ouzel::input::Keyboard::Key::F6},
    {XK_F7, ouzel::input::Keyboard::Key::F7},
    {XK_F8, ouzel::input::Keyboard::Key::F8},
    {XK_F9, ouzel::input::Keyboard::Key::F9},
    {XK_F10, ouzel::input::Keyboard::Key::F10},
    {XK_F11, ouzel::input::Keyboard::Key::F11},
    {XK_F12, ouzel::input::Keyboard::Key::F12},
    {XK_Shift_L, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {XK_Shift_R, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {XK_Control_L, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {XK_Control_R, ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {XK_Caps_Lock, ouzel::input::Keyboard::Key::CAPS_LOCK},
    {XK_Shift_Lock, ouzel::input::Keyboard::Key::CAPS_LOCK},
    {XK_Meta_L, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {XK_Meta_R, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {XK_Hyper_L, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {XK_Hyper_R, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {XK_Alt_L, ouzel::input::Keyboard::Key::LEFT_ALT},
    {XK_Alt_R, ouzel::input::Keyboard::Key::RIGHT_ALT},
    {XK_ISO_Level3_Shift, ouzel::input::Keyboard::Key::RIGHT_ALT},
    {XK_Menu, ouzel::input::Keyboard::Key::MENU},
    {XK_space, ouzel::input::Keyboard::Key::SPACE},
    {XK_exclam, ouzel::input::Keyboard::Key::NUM_1},
    {XK_quotedbl, ouzel::input::Keyboard::Key::NUM_2},
    {XK_section, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_numbersign, ouzel::input::Keyboard::Key::SLASH},
    {XK_dollar, ouzel::input::Keyboard::Key::NUM_4},
    {XK_percent, ouzel::input::Keyboard::Key::NUM_5},
    {XK_ampersand, ouzel::input::Keyboard::Key::NUM_7},
    {XK_apostrophe, ouzel::input::Keyboard::Key::QUOTE},
    {XK_parenleft, ouzel::input::Keyboard::Key::NUM_9},
    {XK_parenright, ouzel::input::Keyboard::Key::NUM_0},
    {XK_asterisk, ouzel::input::Keyboard::Key::NUM_8},
    {XK_plus, ouzel::input::Keyboard::Key::PLUS},
    {XK_comma, ouzel::input::Keyboard::Key::COMMA},
    {XK_minus, ouzel::input::Keyboard::Key::MINUS},
    {XK_period, ouzel::input::Keyboard::Key::PERIOD},
    {XK_slash, ouzel::input::Keyboard::Key::SLASH},
    {XK_0, ouzel::input::Keyboard::Key::NUM_0},
    {XK_1, ouzel::input::Keyboard::Key::NUM_1},
    {XK_2, ouzel::input::Keyboard::Key::NUM_2},
    {XK_3, ouzel::input::Keyboard::Key::NUM_3},
    {XK_4, ouzel::input::Keyboard::Key::NUM_4},
    {XK_5, ouzel::input::Keyboard::Key::NUM_5},
    {XK_6, ouzel::input::Keyboard::Key::NUM_6},
    {XK_7, ouzel::input::Keyboard::Key::NUM_7},
    {XK_8, ouzel::input::Keyboard::Key::NUM_8},
    {XK_9, ouzel::input::Keyboard::Key::NUM_9},
    {XK_colon, ouzel::input::Keyboard::Key::SEMICOLON},
    {XK_semicolon, ouzel::input::Keyboard::Key::SEMICOLON},
    {XK_less, ouzel::input::Keyboard::Key::COMMA},
    {XK_equal, ouzel::input::Keyboard::Key::EQUAL},
    {XK_greater, ouzel::input::Keyboard::Key::PERIOD},
    {XK_question, ouzel::input::Keyboard::Key::SLASH},
    {XK_at, ouzel::input::Keyboard::Key::NUM_2}, // ?
    {XK_mu, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_EuroSign, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_A, ouzel::input::Keyboard::Key::A},
    {XK_B, ouzel::input::Keyboard::Key::B},
    {XK_C, ouzel::input::Keyboard::Key::C},
    {XK_D, ouzel::input::Keyboard::Key::D},
    {XK_E, ouzel::input::Keyboard::Key::E},
    {XK_F, ouzel::input::Keyboard::Key::F},
    {XK_G, ouzel::input::Keyboard::Key::G},
    {XK_H, ouzel::input::Keyboard::Key::H},
    {XK_I, ouzel::input::Keyboard::Key::I},
    {XK_J, ouzel::input::Keyboard::Key::J},
    {XK_K, ouzel::input::Keyboard::Key::K},
    {XK_L, ouzel::input::Keyboard::Key::L},
    {XK_M, ouzel::input::Keyboard::Key::M},
    {XK_N, ouzel::input::Keyboard::Key::N},
    {XK_O, ouzel::input::Keyboard::Key::O},
    {XK_P, ouzel::input::Keyboard::Key::P},
    {XK_Q, ouzel::input::Keyboard::Key::Q},
    {XK_R, ouzel::input::Keyboard::Key::R},
    {XK_S, ouzel::input::Keyboard::Key::S},
    {XK_T, ouzel::input::Keyboard::Key::T},
    {XK_U, ouzel::input::Keyboard::Key::U},
    {XK_V, ouzel::input::Keyboard::Key::V},
    {XK_W, ouzel::input::Keyboard::Key::W},
    {XK_X, ouzel::input::Keyboard::Key::X},
    {XK_Y, ouzel::input::Keyboard::Key::Y},
    {XK_Z, ouzel::input::Keyboard::Key::Z},
    {XK_bracketleft, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {XK_backslash, ouzel::input::Keyboard::Key::BACKSLASH},
    {XK_bracketright, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {XK_asciicircum, ouzel::input::Keyboard::Key::BACKSLASH},
    {XK_dead_circumflex, ouzel::input::Keyboard::Key::BACKSLASH},
    {XK_degree, ouzel::input::Keyboard::Key::NONE}, // ?
    {XK_underscore, ouzel::input::Keyboard::Key::MINUS},
    {XK_grave, ouzel::input::Keyboard::Key::GRAVE},
    {XK_dead_grave, ouzel::input::Keyboard::Key::GRAVE},
    {XK_acute, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {XK_dead_acute, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {XK_a, ouzel::input::Keyboard::Key::A},
    {XK_b, ouzel::input::Keyboard::Key::B},
    {XK_c, ouzel::input::Keyboard::Key::C},
    {XK_d, ouzel::input::Keyboard::Key::D},
    {XK_e, ouzel::input::Keyboard::Key::E},
    {XK_f, ouzel::input::Keyboard::Key::F},
    {XK_g, ouzel::input::Keyboard::Key::G},
    {XK_h, ouzel::input::Keyboard::Key::H},
    {XK_i, ouzel::input::Keyboard::Key::I},
    {XK_j, ouzel::input::Keyboard::Key::J},
    {XK_k, ouzel::input::Keyboard::Key::K},
    {XK_l, ouzel::input::Keyboard::Key::L},
    {XK_m, ouzel::input::Keyboard::Key::M},
    {XK_n, ouzel::input::Keyboard::Key::N},
    {XK_o, ouzel::input::Keyboard::Key::O},
    {XK_p, ouzel::input::Keyboard::Key::P},
    {XK_q, ouzel::input::Keyboard::Key::Q},
    {XK_r, ouzel::input::Keyboard::Key::R},
    {XK_s, ouzel::input::Keyboard::Key::S},
    {XK_t, ouzel::input::Keyboard::Key::T},
    {XK_u, ouzel::input::Keyboard::Key::U},
    {XK_v, ouzel::input::Keyboard::Key::V},
    {XK_w, ouzel::input::Keyboard::Key::W},
    {XK_x, ouzel::input::Keyboard::Key::X},
    {XK_y, ouzel::input::Keyboard::Key::Y},
    {XK_z, ouzel::input::Keyboard::Key::Z},
    {XK_ssharp, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {XK_adiaeresis, ouzel::input::Keyboard::Key::QUOTE},
    {XK_odiaeresis, ouzel::input::Keyboard::Key::GRAVE},
    {XK_udiaeresis, ouzel::input::Keyboard::Key::SEMICOLON},
    {XK_Super_L, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {XK_Super_R, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {XK_Mode_switch, ouzel::input::Keyboard::Key::MODE_CHANGE},
    {XK_Help, ouzel::input::Keyboard::Key::HELP},
    {XK_yen, ouzel::input::Keyboard::Key::YEN},
    {XK_Kanji, ouzel::input::Keyboard::Key::KANJI},
    {XK_Muhenkan, ouzel::input::Keyboard::Key::MUHENKAN},
    {XK_Henkan, ouzel::input::Keyboard::Key::HENKAN},
    {XK_Romaji, ouzel::input::Keyboard::Key::RO},
    {XK_Hiragana, ouzel::input::Keyboard::Key::HIRAGANA},
    {XK_Katakana, ouzel::input::Keyboard::Key::KATAKANA},
    {XK_Hiragana_Katakana, ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA}
};

static const std::unordered_map<unsigned int, ouzel::input::Mouse::Button> buttonMap = {
    {Button1, ouzel::input::Mouse::Button::LEFT},
    {Button2, ouzel::input::Mouse::Button::MIDDLE},
    {Button3, ouzel::input::Mouse::Button::RIGHT},
    {8, ouzel::input::Mouse::Button::X1},
    {9, ouzel::input::Mouse::Button::X2}
};

static ouzel::input::Keyboard::Key convertKeyCode(KeySym keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

static ouzel::input::Mouse::Button convertButtonCode(unsigned int buttonCode)
{
    auto i = buttonMap.find(buttonCode);

    if (i != buttonMap.end())
        return i->second;
    else
        return ouzel::input::Mouse::Button::NONE;
}
#endif

namespace ouzel
{
    EngineLinux::EngineLinux(int initArgc, char* initArgv[])
    {
        for (int i = 0; i < initArgc; ++i)
            args.push_back(initArgv[i]);
    }

    EngineLinux::~EngineLinux()
    {
#if OUZEL_SUPPORTS_X11
        if (display) XCloseDisplay(display);
#else
        if (display != DISPMANX_NO_HANDLE)
            vc_dispmanx_display_close(display);

        bcm_host_deinit();
#endif
    }

    void EngineLinux::run()
    {
#if OUZEL_SUPPORTS_X11
        if (!XInitThreads())
            throw std::runtime_error("Failed to initialize thread support");

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
            throw std::runtime_error("Failed to open display");
#else
        bcm_host_init();

        display = vc_dispmanx_display_open(0);
        if (display == DISPMANX_NO_HANDLE)
            throw std::runtime_error("Failed to open display");

#endif

        init();
        start();

        input::InputSystemLinux* inputLinux = static_cast<input::InputSystemLinux*>(inputManager->getInputSystem());

#if OUZEL_SUPPORTS_X11
        NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

        int xInputOpCode = 0;
        int eventCode;
        int err;
        if (XQueryExtension(display, "XInputExtension", &xInputOpCode, &eventCode, &err))
        {
            int majorVersion = 2;
            int minorVersion = 0;

            XIQueryVersion(display, &majorVersion, &minorVersion);

            if (majorVersion >= 2)
            {
                unsigned char mask[] = {0, 0 ,0};

                XIEventMask eventMask;
                eventMask.deviceid = XIAllMasterDevices;
                eventMask.mask_len = sizeof(mask);
                eventMask.mask = mask;

                XISetMask(mask, XI_TouchBegin);
                XISetMask(mask, XI_TouchEnd);
                XISetMask(mask, XI_TouchUpdate);

                XISelectEvents(display, windowLinux->getNativeWindow(), &eventMask, 1);
            }
            else
                log(Log::Level::WARN) << "XInput2 not supported";
        }
        else
            log(Log::Level::WARN) << "XInput not supported";

        XEvent event;

        while (active)
        {
            // XNextEvent will block if there is no event pending, so don't call it if engine is not paused
            if (paused || XPending(display))
            {
                XNextEvent(display, &event);

                switch (event.type)
                {
                    case ClientMessage:
                    {
                        if (event.xclient.message_type == windowLinux->getProtocolsAtom() && static_cast<Atom>(event.xclient.data.l[0]) == windowLinux->getDeleteAtom())
                            exit();
                        else if (event.xclient.message_type == windowLinux->getExecuteAtom())
                            executeAll();
                        break;
                    }
                    case FocusIn:
                        windowLinux->handleFocusIn();
                        break;
                    case FocusOut:
                        windowLinux->handleFocusOut();
                        break;
                    case KeyPress: // keyboard
                    case KeyRelease:
                    {
                        ouzel::input::InputSystemLinux* inputSystemLinux = static_cast<ouzel::input::InputSystemLinux*>(inputManager->getInputSystem());
                        ouzel::input::KeyboardDevice* keyboardDevice = inputSystemLinux->getKeyboardDevice();

                        KeySym keySym = XkbKeycodeToKeysym(display,
                                                           event.xkey.keycode, 0,
                                                           event.xkey.state & ShiftMask ? 1 : 0);

                        if (event.type == KeyPress)
                            keyboardDevice->handleKeyPress(convertKeyCode(keySym));
                        else
                            keyboardDevice->handleKeyRelease(convertKeyCode(keySym));
                        break;
                    }
                    case ButtonPress: // mouse button
                    case ButtonRelease:
                    {
                        ouzel::input::InputSystemLinux* inputSystemLinux = static_cast<ouzel::input::InputSystemLinux*>(inputManager->getInputSystem());
                        ouzel::input::MouseDeviceLinux* mouseDevice = inputSystemLinux->getMouseDevice();

                        Vector2 pos(static_cast<float>(event.xbutton.x),
                                    static_cast<float>(event.xbutton.y));

                        if (event.type == ButtonPress)
                            mouseDevice->handleButtonPress(convertButtonCode(event.xbutton.button),
                                                           window->convertWindowToNormalizedLocation(pos));
                        else
                            mouseDevice->handleButtonRelease(convertButtonCode(event.xbutton.button),
                                                             window->convertWindowToNormalizedLocation(pos));
                        break;
                    }
                    case MotionNotify:
                    {
                        ouzel::input::InputSystemLinux* inputSystemLinux = static_cast<ouzel::input::InputSystemLinux*>(inputManager->getInputSystem());
                        ouzel::input::MouseDeviceLinux* mouseDevice = inputSystemLinux->getMouseDevice();

                        Vector2 pos(static_cast<float>(event.xmotion.x),
                                    static_cast<float>(event.xmotion.y));

                        mouseDevice->handleMove(window->convertWindowToNormalizedLocation(pos));

                        break;
                    }
                    case ConfigureNotify:
                    {
                        windowLinux->handleResize(Size2(static_cast<float>(event.xconfigure.width),
                                                        static_cast<float>(event.xconfigure.height)));
                        break;
                    }
                    case Expose:
                    {
                        // need to redraw
                        break;
                    }
                    case GenericEvent:
                    {
                        XGenericEventCookie* cookie = &event.xcookie;
                        if (cookie->extension == xInputOpCode)
                        {
                            ouzel::input::InputSystemLinux* inputSystemLinux = static_cast<ouzel::input::InputSystemLinux*>(inputManager->getInputSystem());
                            ouzel::input::TouchpadDevice* touchpadDevice = inputSystemLinux->getTouchpadDevice();

                            switch (cookie->evtype)
                            {
                                case XI_TouchBegin:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    touchpadDevice->handleTouchBegin(xievent->detail,
                                                                     window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                       static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchEnd:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    touchpadDevice->handleTouchEnd(xievent->detail,
                                                                   window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                     static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchUpdate:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    touchpadDevice->handleTouchMove(xievent->detail,
                                                                    window->convertWindowToNormalizedLocation(Vector2(static_cast<float>(xievent->event_x),
                                                                                                                      static_cast<float>(xievent->event_y))));
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            inputLinux->update();
        }
#else
        while (active)
        {
            executeAll();

            inputLinux->update();
        }
#endif

        exit();
    }

    void EngineLinux::executeOnMainThread(const std::function<void()>& func)
    {
#if OUZEL_SUPPORTS_X11
        NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = windowLinux->getNativeWindow();
        event.xclient.message_type = windowLinux->getExecuteAtom();
        event.xclient.format = 32; // data is set as 32-bit integers
        event.xclient.data.l[0] = 0;
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0; // unused
        event.xclient.data.l[3] = 0; // unused
        event.xclient.data.l[4] = 0; // unused

        std::unique_lock<std::mutex> lock(executeMutex);
        executeQueue.push(func);
        lock.unlock();

        if (!XSendEvent(display, windowLinux->getNativeWindow(), False, NoEventMask, &event))
            throw std::runtime_error("Failed to send X11 delete message");

        XFlush(display);
#else
        std::unique_lock<std::mutex> lock(executeMutex);
        executeQueue.push(func);
#endif
    }

    void EngineLinux::openURL(const std::string& url)
    {
#if OUZEL_SUPPORTS_X11
		::exit(execl("/usr/bin/xdg-open", "xdg-open", url.c_str(), nullptr));
#endif
	}

	void EngineLinux::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        Engine::setScreenSaverEnabled(newScreenSaverEnabled);

#if OUZEL_SUPPORTS_X11
        executeOnMainThread([this, newScreenSaverEnabled]() {
            XScreenSaverSuspend(display, !newScreenSaverEnabled);
        });
#endif
    }

    void EngineLinux::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(executeMutex);

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
