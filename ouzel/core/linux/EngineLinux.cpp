// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
static ouzel::input::Keyboard::Key convertKeyCode(KeySym keyCode)
{
    switch (keyCode)
    {
        case XK_BackSpace: return ouzel::input::Keyboard::Key::BACKSPACE;
        case XK_Tab: return ouzel::input::Keyboard::Key::TAB;
        case XK_ISO_Left_Tab: return ouzel::input::Keyboard::Key::TAB;
        case XK_Linefeed: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_Clear: return ouzel::input::Keyboard::Key::CLEAR;
        case XK_Return: return ouzel::input::Keyboard::Key::ENTER;
        case XK_Pause: return ouzel::input::Keyboard::Key::PAUSE;
        case XK_Scroll_Lock: return ouzel::input::Keyboard::Key::SCROLL_LOCK;
        case XK_Sys_Req: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_Escape: return ouzel::input::Keyboard::Key::ESCAPE;
        case XK_Insert: return ouzel::input::Keyboard::Key::INSERT;
        case XK_Delete: return ouzel::input::Keyboard::Key::DEL;
        case XK_Home: return ouzel::input::Keyboard::Key::HOME;
        case XK_Left: return ouzel::input::Keyboard::Key::LEFT;
        case XK_Up: return ouzel::input::Keyboard::Key::UP;
        case XK_Right: return ouzel::input::Keyboard::Key::RIGHT;
        case XK_Down: return ouzel::input::Keyboard::Key::DOWN;
        case XK_Prior: return ouzel::input::Keyboard::Key::PAGE_UP; // also XK_Page_Up
        case XK_Next: return ouzel::input::Keyboard::Key::PAGE_DOWN; // also XK_Page_Down
        case XK_End: return ouzel::input::Keyboard::Key::END;
        case XK_Begin: return ouzel::input::Keyboard::Key::HOME;
        case XK_Num_Lock: return ouzel::input::Keyboard::Key::NUM_LOCK;
        case XK_KP_Space: return ouzel::input::Keyboard::Key::SPACE;
        case XK_KP_Tab: return ouzel::input::Keyboard::Key::TAB;
        case XK_KP_Enter: return ouzel::input::Keyboard::Key::NUMPAD_ENTER;
        case XK_KP_F1: return ouzel::input::Keyboard::Key::F1;
        case XK_KP_F2: return ouzel::input::Keyboard::Key::F2;
        case XK_KP_F3: return ouzel::input::Keyboard::Key::F3;
        case XK_KP_F4: return ouzel::input::Keyboard::Key::F4;
        case XK_KP_Home: return ouzel::input::Keyboard::Key::NUMPAD_7;
        case XK_KP_Left: return ouzel::input::Keyboard::Key::NUMPAD_4;
        case XK_KP_Up: return ouzel::input::Keyboard::Key::NUMPAD_8;
        case XK_KP_Right: return ouzel::input::Keyboard::Key::NUMPAD_6;
        case XK_KP_Down: return ouzel::input::Keyboard::Key::NUMPAD_2;
        case XK_Print: return ouzel::input::Keyboard::Key::PRINT_SCREEN;
        case XK_KP_Prior: return ouzel::input::Keyboard::Key::NUMPAD_9; // also XK_KP_Page_Up
        case XK_KP_Next: return ouzel::input::Keyboard::Key::NUMPAD_3; // also XK_KP_Page_Down
        case XK_KP_End: return ouzel::input::Keyboard::Key::NUMPAD_1;
        case XK_KP_Begin: return ouzel::input::Keyboard::Key::NUMPAD_5;
        case XK_KP_Insert: return ouzel::input::Keyboard::Key::NUMPAD_0;
        case XK_KP_Delete: return ouzel::input::Keyboard::Key::NUMPAD_DECIMAL;
        case XK_KP_Equal: return ouzel::input::Keyboard::Key::NUMPAD_EQUAL;
        case XK_KP_Multiply: return ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY;
        case XK_KP_Add: return ouzel::input::Keyboard::Key::NUMPAD_PLUS;
        case XK_KP_Separator: return ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR;
        case XK_KP_Subtract: return ouzel::input::Keyboard::Key::NUMPAD_MINUS;
        case XK_KP_Decimal: return ouzel::input::Keyboard::Key::NUMPAD_DECIMAL;
        case XK_KP_Divide: return ouzel::input::Keyboard::Key::NUMPAD_DIVIDE;
        case XK_KP_0: return ouzel::input::Keyboard::Key::NUMPAD_0;
        case XK_KP_1: return ouzel::input::Keyboard::Key::NUMPAD_1;
        case XK_KP_2: return ouzel::input::Keyboard::Key::NUMPAD_2;
        case XK_KP_3: return ouzel::input::Keyboard::Key::NUMPAD_3;
        case XK_KP_4: return ouzel::input::Keyboard::Key::NUMPAD_4;
        case XK_KP_5: return ouzel::input::Keyboard::Key::NUMPAD_5;
        case XK_KP_6: return ouzel::input::Keyboard::Key::NUMPAD_6;
        case XK_KP_7: return ouzel::input::Keyboard::Key::NUMPAD_7;
        case XK_KP_8: return ouzel::input::Keyboard::Key::NUMPAD_8;
        case XK_KP_9: return ouzel::input::Keyboard::Key::NUMPAD_9;
        case XK_F1: return ouzel::input::Keyboard::Key::F1;
        case XK_F2: return ouzel::input::Keyboard::Key::F2;
        case XK_F3: return ouzel::input::Keyboard::Key::F3;
        case XK_F4: return ouzel::input::Keyboard::Key::F4;
        case XK_F5: return ouzel::input::Keyboard::Key::F5;
        case XK_F6: return ouzel::input::Keyboard::Key::F6;
        case XK_F7: return ouzel::input::Keyboard::Key::F7;
        case XK_F8: return ouzel::input::Keyboard::Key::F8;
        case XK_F9: return ouzel::input::Keyboard::Key::F9;
        case XK_F10: return ouzel::input::Keyboard::Key::F10;
        case XK_F11: return ouzel::input::Keyboard::Key::F11;
        case XK_F12: return ouzel::input::Keyboard::Key::F12;
        case XK_Shift_L: return ouzel::input::Keyboard::Key::LEFT_SHIFT;
        case XK_Shift_R: return ouzel::input::Keyboard::Key::RIGHT_SHIFT;
        case XK_Control_L: return ouzel::input::Keyboard::Key::LEFT_CONTROL;
        case XK_Control_R: return ouzel::input::Keyboard::Key::RIGHT_CONTROL;
        case XK_Caps_Lock: return ouzel::input::Keyboard::Key::CAPS_LOCK;
        case XK_Shift_Lock: return ouzel::input::Keyboard::Key::CAPS_LOCK;
        case XK_Meta_L: return ouzel::input::Keyboard::Key::LEFT_SUPER;
        case XK_Meta_R: return ouzel::input::Keyboard::Key::RIGHT_SUPER;
        case XK_Hyper_L: return ouzel::input::Keyboard::Key::LEFT_SUPER;
        case XK_Hyper_R: return ouzel::input::Keyboard::Key::RIGHT_SUPER;
        case XK_Alt_L: return ouzel::input::Keyboard::Key::LEFT_ALT;
        case XK_Alt_R: return ouzel::input::Keyboard::Key::RIGHT_ALT;
        case XK_ISO_Level3_Shift: return ouzel::input::Keyboard::Key::RIGHT_ALT;
        case XK_Menu: return ouzel::input::Keyboard::Key::MENU;
        case XK_space: return ouzel::input::Keyboard::Key::SPACE;
        case XK_exclam: return ouzel::input::Keyboard::Key::NUM_1;
        case XK_quotedbl: return ouzel::input::Keyboard::Key::NUM_2;
        case XK_section: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_numbersign: return ouzel::input::Keyboard::Key::SLASH;
        case XK_dollar: return ouzel::input::Keyboard::Key::NUM_4;
        case XK_percent: return ouzel::input::Keyboard::Key::NUM_5;
        case XK_ampersand: return ouzel::input::Keyboard::Key::NUM_7;
        case XK_apostrophe: return ouzel::input::Keyboard::Key::QUOTE;
        case XK_parenleft: return ouzel::input::Keyboard::Key::NUM_9;
        case XK_parenright: return ouzel::input::Keyboard::Key::NUM_0;
        case XK_asterisk: return ouzel::input::Keyboard::Key::NUM_8;
        case XK_plus: return ouzel::input::Keyboard::Key::PLUS;
        case XK_comma: return ouzel::input::Keyboard::Key::COMMA;
        case XK_minus: return ouzel::input::Keyboard::Key::MINUS;
        case XK_period: return ouzel::input::Keyboard::Key::PERIOD;
        case XK_slash: return ouzel::input::Keyboard::Key::SLASH;
        case XK_0: return ouzel::input::Keyboard::Key::NUM_0;
        case XK_1: return ouzel::input::Keyboard::Key::NUM_1;
        case XK_2: return ouzel::input::Keyboard::Key::NUM_2;
        case XK_3: return ouzel::input::Keyboard::Key::NUM_3;
        case XK_4: return ouzel::input::Keyboard::Key::NUM_4;
        case XK_5: return ouzel::input::Keyboard::Key::NUM_5;
        case XK_6: return ouzel::input::Keyboard::Key::NUM_6;
        case XK_7: return ouzel::input::Keyboard::Key::NUM_7;
        case XK_8: return ouzel::input::Keyboard::Key::NUM_8;
        case XK_9: return ouzel::input::Keyboard::Key::NUM_9;
        case XK_colon: return ouzel::input::Keyboard::Key::SEMICOLON;
        case XK_semicolon: return ouzel::input::Keyboard::Key::SEMICOLON;
        case XK_less: return ouzel::input::Keyboard::Key::COMMA;
        case XK_equal: return ouzel::input::Keyboard::Key::EQUAL;
        case XK_greater: return ouzel::input::Keyboard::Key::PERIOD;
        case XK_question: return ouzel::input::Keyboard::Key::SLASH;
        case XK_at: return ouzel::input::Keyboard::Key::NUM_2; // ?
        case XK_mu: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_EuroSign: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_A: return ouzel::input::Keyboard::Key::A;
        case XK_B: return ouzel::input::Keyboard::Key::B;
        case XK_C: return ouzel::input::Keyboard::Key::C;
        case XK_D: return ouzel::input::Keyboard::Key::D;
        case XK_E: return ouzel::input::Keyboard::Key::E;
        case XK_F: return ouzel::input::Keyboard::Key::F;
        case XK_G: return ouzel::input::Keyboard::Key::G;
        case XK_H: return ouzel::input::Keyboard::Key::H;
        case XK_I: return ouzel::input::Keyboard::Key::I;
        case XK_J: return ouzel::input::Keyboard::Key::J;
        case XK_K: return ouzel::input::Keyboard::Key::K;
        case XK_L: return ouzel::input::Keyboard::Key::L;
        case XK_M: return ouzel::input::Keyboard::Key::M;
        case XK_N: return ouzel::input::Keyboard::Key::N;
        case XK_O: return ouzel::input::Keyboard::Key::O;
        case XK_P: return ouzel::input::Keyboard::Key::P;
        case XK_Q: return ouzel::input::Keyboard::Key::Q;
        case XK_R: return ouzel::input::Keyboard::Key::R;
        case XK_S: return ouzel::input::Keyboard::Key::S;
        case XK_T: return ouzel::input::Keyboard::Key::T;
        case XK_U: return ouzel::input::Keyboard::Key::U;
        case XK_V: return ouzel::input::Keyboard::Key::V;
        case XK_W: return ouzel::input::Keyboard::Key::W;
        case XK_X: return ouzel::input::Keyboard::Key::X;
        case XK_Y: return ouzel::input::Keyboard::Key::Y;
        case XK_Z: return ouzel::input::Keyboard::Key::Z;
        case XK_bracketleft: return ouzel::input::Keyboard::Key::LEFT_BRACKET;
        case XK_backslash: return ouzel::input::Keyboard::Key::BACKSLASH;
        case XK_bracketright: return ouzel::input::Keyboard::Key::RIGHT_BRACKET;
        case XK_asciicircum: return ouzel::input::Keyboard::Key::BACKSLASH;
        case XK_dead_circumflex: return ouzel::input::Keyboard::Key::BACKSLASH;
        case XK_degree: return ouzel::input::Keyboard::Key::NONE; // ?
        case XK_underscore: return ouzel::input::Keyboard::Key::MINUS;
        case XK_grave: return ouzel::input::Keyboard::Key::GRAVE;
        case XK_dead_grave: return ouzel::input::Keyboard::Key::GRAVE;
        case XK_acute: return ouzel::input::Keyboard::Key::RIGHT_BRACKET;
        case XK_dead_acute: return ouzel::input::Keyboard::Key::RIGHT_BRACKET;
        case XK_a: return ouzel::input::Keyboard::Key::A;
        case XK_b: return ouzel::input::Keyboard::Key::B;
        case XK_c: return ouzel::input::Keyboard::Key::C;
        case XK_d: return ouzel::input::Keyboard::Key::D;
        case XK_e: return ouzel::input::Keyboard::Key::E;
        case XK_f: return ouzel::input::Keyboard::Key::F;
        case XK_g: return ouzel::input::Keyboard::Key::G;
        case XK_h: return ouzel::input::Keyboard::Key::H;
        case XK_i: return ouzel::input::Keyboard::Key::I;
        case XK_j: return ouzel::input::Keyboard::Key::J;
        case XK_k: return ouzel::input::Keyboard::Key::K;
        case XK_l: return ouzel::input::Keyboard::Key::L;
        case XK_m: return ouzel::input::Keyboard::Key::M;
        case XK_n: return ouzel::input::Keyboard::Key::N;
        case XK_o: return ouzel::input::Keyboard::Key::O;
        case XK_p: return ouzel::input::Keyboard::Key::P;
        case XK_q: return ouzel::input::Keyboard::Key::Q;
        case XK_r: return ouzel::input::Keyboard::Key::R;
        case XK_s: return ouzel::input::Keyboard::Key::S;
        case XK_t: return ouzel::input::Keyboard::Key::T;
        case XK_u: return ouzel::input::Keyboard::Key::U;
        case XK_v: return ouzel::input::Keyboard::Key::V;
        case XK_w: return ouzel::input::Keyboard::Key::W;
        case XK_x: return ouzel::input::Keyboard::Key::X;
        case XK_y: return ouzel::input::Keyboard::Key::Y;
        case XK_z: return ouzel::input::Keyboard::Key::Z;
        case XK_ssharp: return ouzel::input::Keyboard::Key::LEFT_BRACKET;
        case XK_adiaeresis: return ouzel::input::Keyboard::Key::QUOTE;
        case XK_odiaeresis: return ouzel::input::Keyboard::Key::GRAVE;
        case XK_udiaeresis: return ouzel::input::Keyboard::Key::SEMICOLON;
        case XK_Super_L: return ouzel::input::Keyboard::Key::LEFT_SUPER;
        case XK_Super_R: return ouzel::input::Keyboard::Key::RIGHT_SUPER;
        case XK_Mode_switch: return ouzel::input::Keyboard::Key::MODE_CHANGE;
        case XK_Help: return ouzel::input::Keyboard::Key::HELP;
        case XK_yen: return ouzel::input::Keyboard::Key::YEN;
        case XK_Kanji: return ouzel::input::Keyboard::Key::KANJI;
        case XK_Muhenkan: return ouzel::input::Keyboard::Key::MUHENKAN;
        case XK_Henkan: return ouzel::input::Keyboard::Key::HENKAN;
        case XK_Romaji: return ouzel::input::Keyboard::Key::RO;
        case XK_Hiragana: return ouzel::input::Keyboard::Key::HIRAGANA;
        case XK_Katakana: return ouzel::input::Keyboard::Key::KATAKANA;
        case XK_Hiragana_Katakana: return ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA;
        default: return ouzel::input::Keyboard::Key::NONE;
    }
}

static ouzel::input::Mouse::Button convertButtonCode(unsigned int buttonCode)
{
    switch (buttonCode)
    {
        case Button1: return ouzel::input::Mouse::Button::LEFT;
        case Button2: return ouzel::input::Mouse::Button::MIDDLE;
        case Button3: return ouzel::input::Mouse::Button::RIGHT;
        case 8: return ouzel::input::Mouse::Button::X1;
        case 9: return ouzel::input::Mouse::Button::X2;
        default: return ouzel::input::Mouse::Button::NONE;
    }
}
#endif

namespace ouzel
{
    EngineLinux::EngineLinux(int initArgc, char* initArgv[])
    {
        for (int i = 0; i < initArgc; ++i)
            args.push_back(initArgv[i]);

#if OUZEL_SUPPORTS_X11
        if (!XInitThreads())
            throw std::runtime_error("Failed to initialize thread support");

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
            throw std::runtime_error("Failed to open display");
#elif OUZEL_SUPPORTS_DISPMANX
        bcm_host_init();

        display = vc_dispmanx_display_open(0);
        if (display == DISPMANX_NO_HANDLE)
            throw std::runtime_error("Failed to open display");

#endif
    }

    EngineLinux::~EngineLinux()
    {
#if OUZEL_SUPPORTS_X11
        if (display) XCloseDisplay(display);
#elif OUZEL_SUPPORTS_DISPMANX
        if (display != DISPMANX_NO_HANDLE)
            vc_dispmanx_display_close(display);

        bcm_host_deinit();
#endif
    }

    void EngineLinux::run()
    {
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

        executeAtom = XInternAtom(display, "OUZEL_EXECUTE", False);

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
                        if (event.xclient.message_type == executeAtom)
                            executeAll();
                        else if (event.xclient.message_type == windowLinux->getProtocolsAtom() && static_cast<Atom>(event.xclient.data.l[0]) == windowLinux->getDeleteAtom())
                            exit();
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

                        Vector2F pos(static_cast<float>(event.xbutton.x),
                                     static_cast<float>(event.xbutton.y));

                        if (event.type == ButtonPress)
                            mouseDevice->handleButtonPress(convertButtonCode(event.xbutton.button),
                                                           window->convertWindowToNormalizedLocation(pos));
                        else
                            mouseDevice->handleButtonRelease(convertButtonCode(event.xbutton.button),
                                                             window->convertWindowToNormalizedLocation(pos));
                        break;
                    }
                    case MapNotify:
                    {
                        windowLinux->handleMap();
                        break;
                    }
                    case UnmapNotify:
                    {
                        windowLinux->handleUnmap();
                        break;
                    }
                    case MotionNotify:
                    {
                        ouzel::input::InputSystemLinux* inputSystemLinux = static_cast<ouzel::input::InputSystemLinux*>(inputManager->getInputSystem());
                        ouzel::input::MouseDeviceLinux* mouseDevice = inputSystemLinux->getMouseDevice();

                        Vector2F pos(static_cast<float>(event.xmotion.x),
                                     static_cast<float>(event.xmotion.y));

                        mouseDevice->handleMove(window->convertWindowToNormalizedLocation(pos));

                        break;
                    }
                    case ConfigureNotify:
                    {
                        windowLinux->handleResize(Size2U(static_cast<uint32_t>(event.xconfigure.width),
                                                         static_cast<uint32_t>(event.xconfigure.height)));
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
                                                                     window->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(xievent->event_x),
                                                                                                                        static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchEnd:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    touchpadDevice->handleTouchEnd(xievent->detail,
                                                                   window->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(xievent->event_x),
                                                                                                                      static_cast<float>(xievent->event_y))));
                                    break;
                                }
                                case XI_TouchUpdate:
                                {
                                    XIDeviceEvent* xievent = reinterpret_cast<XIDeviceEvent*>(cookie->data);
                                    touchpadDevice->handleTouchMove(xievent->detail,
                                                                    window->convertWindowToNormalizedLocation(Vector2F(static_cast<float>(xievent->event_x),
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

    void EngineLinux::runOnMainThread(const std::function<void()>& func)
    {
#if OUZEL_SUPPORTS_X11
        NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = 0;
        event.xclient.message_type = executeAtom;
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
