// Ouzel by Elviss Strazdins

#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include "../Setup.h"
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
#include "../../platform/x11/X11ErrorCategory.hpp"
#include "../../events/Event.hpp"
#include "../../graphics/RenderDevice.hpp"
#include "../../input/linux/InputSystemLinux.hpp"
#include "../../utils/Log.hpp"

#if OUZEL_SUPPORTS_X11
namespace
{
    thread_local static int error;

    int errorHandler(Display* display, XErrorEvent* errorEvent)
    {
        error = errorEvent->error_code;
        std::array<char, 256> text;
        if (XGetErrorText(nullptr, errorEvent->error_code, text.data(), text.size()) == 0)
            ouzel::log(ouzel::Log::Level::error) << "X11 error: " << text.data();
        else
            ouzel::log(ouzel::Log::Level::error) << "X11 error";
        return 0;
    }

    ouzel::input::Keyboard::Key convertKeyCode(KeySym keyCode)
    {
        switch (keyCode)
        {
            case XK_BackSpace: return ouzel::input::Keyboard::Key::backspace;
            case XK_Tab: return ouzel::input::Keyboard::Key::tab;
            case XK_ISO_Left_Tab: return ouzel::input::Keyboard::Key::tab;
            case XK_Linefeed: return ouzel::input::Keyboard::Key::none; // ?
            case XK_Clear: return ouzel::input::Keyboard::Key::clear;
            case XK_Return: return ouzel::input::Keyboard::Key::enter;
            case XK_Pause: return ouzel::input::Keyboard::Key::pause;
            case XK_Scroll_Lock: return ouzel::input::Keyboard::Key::scrollLock;
            case XK_Sys_Req: return ouzel::input::Keyboard::Key::none; // ?
            case XK_Escape: return ouzel::input::Keyboard::Key::escape;
            case XK_Insert: return ouzel::input::Keyboard::Key::insert;
            case XK_Delete: return ouzel::input::Keyboard::Key::del;
            case XK_Home: return ouzel::input::Keyboard::Key::home;
            case XK_Left: return ouzel::input::Keyboard::Key::left;
            case XK_Up: return ouzel::input::Keyboard::Key::up;
            case XK_Right: return ouzel::input::Keyboard::Key::right;
            case XK_Down: return ouzel::input::Keyboard::Key::down;
            case XK_Prior: return ouzel::input::Keyboard::Key::pageUp; // also XK_Page_Up
            case XK_Next: return ouzel::input::Keyboard::Key::pageDown; // also XK_Page_Down
            case XK_End: return ouzel::input::Keyboard::Key::end;
            case XK_Begin: return ouzel::input::Keyboard::Key::home;
            case XK_Num_Lock: return ouzel::input::Keyboard::Key::numLock;
            case XK_KP_Space: return ouzel::input::Keyboard::Key::space;
            case XK_KP_Tab: return ouzel::input::Keyboard::Key::tab;
            case XK_KP_Enter: return ouzel::input::Keyboard::Key::numpadEnter;
            case XK_KP_F1: return ouzel::input::Keyboard::Key::f1;
            case XK_KP_F2: return ouzel::input::Keyboard::Key::f2;
            case XK_KP_F3: return ouzel::input::Keyboard::Key::f3;
            case XK_KP_F4: return ouzel::input::Keyboard::Key::f4;
            case XK_KP_Home: return ouzel::input::Keyboard::Key::numpad7;
            case XK_KP_Left: return ouzel::input::Keyboard::Key::numpad4;
            case XK_KP_Up: return ouzel::input::Keyboard::Key::numpad8;
            case XK_KP_Right: return ouzel::input::Keyboard::Key::numpad6;
            case XK_KP_Down: return ouzel::input::Keyboard::Key::numpad2;
            case XK_Print: return ouzel::input::Keyboard::Key::printScreen;
            case XK_KP_Prior: return ouzel::input::Keyboard::Key::numpad9; // also XK_KP_Page_Up
            case XK_KP_Next: return ouzel::input::Keyboard::Key::numpad3; // also XK_KP_Page_Down
            case XK_KP_End: return ouzel::input::Keyboard::Key::numpad1;
            case XK_KP_Begin: return ouzel::input::Keyboard::Key::numpad5;
            case XK_KP_Insert: return ouzel::input::Keyboard::Key::numpad0;
            case XK_KP_Delete: return ouzel::input::Keyboard::Key::numpadDecimal;
            case XK_KP_Equal: return ouzel::input::Keyboard::Key::numpadEqual;
            case XK_KP_Multiply: return ouzel::input::Keyboard::Key::numpadMultiply;
            case XK_KP_Add: return ouzel::input::Keyboard::Key::numpadPlus;
            case XK_KP_Separator: return ouzel::input::Keyboard::Key::numpadSeparator;
            case XK_KP_Subtract: return ouzel::input::Keyboard::Key::numpadMinus;
            case XK_KP_Decimal: return ouzel::input::Keyboard::Key::numpadDecimal;
            case XK_KP_Divide: return ouzel::input::Keyboard::Key::numpadDivide;
            case XK_KP_0: return ouzel::input::Keyboard::Key::numpad0;
            case XK_KP_1: return ouzel::input::Keyboard::Key::numpad1;
            case XK_KP_2: return ouzel::input::Keyboard::Key::numpad2;
            case XK_KP_3: return ouzel::input::Keyboard::Key::numpad3;
            case XK_KP_4: return ouzel::input::Keyboard::Key::numpad4;
            case XK_KP_5: return ouzel::input::Keyboard::Key::numpad5;
            case XK_KP_6: return ouzel::input::Keyboard::Key::numpad6;
            case XK_KP_7: return ouzel::input::Keyboard::Key::numpad7;
            case XK_KP_8: return ouzel::input::Keyboard::Key::numpad8;
            case XK_KP_9: return ouzel::input::Keyboard::Key::numpad9;
            case XK_F1: return ouzel::input::Keyboard::Key::f1;
            case XK_F2: return ouzel::input::Keyboard::Key::f2;
            case XK_F3: return ouzel::input::Keyboard::Key::f3;
            case XK_F4: return ouzel::input::Keyboard::Key::f4;
            case XK_F5: return ouzel::input::Keyboard::Key::f5;
            case XK_F6: return ouzel::input::Keyboard::Key::f6;
            case XK_F7: return ouzel::input::Keyboard::Key::f7;
            case XK_F8: return ouzel::input::Keyboard::Key::f8;
            case XK_F9: return ouzel::input::Keyboard::Key::f9;
            case XK_F10: return ouzel::input::Keyboard::Key::f10;
            case XK_F11: return ouzel::input::Keyboard::Key::f11;
            case XK_F12: return ouzel::input::Keyboard::Key::f12;
            case XK_Shift_L: return ouzel::input::Keyboard::Key::leftShift;
            case XK_Shift_R: return ouzel::input::Keyboard::Key::rightShift;
            case XK_Control_L: return ouzel::input::Keyboard::Key::leftControl;
            case XK_Control_R: return ouzel::input::Keyboard::Key::rightControl;
            case XK_Caps_Lock: return ouzel::input::Keyboard::Key::capsLock;
            case XK_Shift_Lock: return ouzel::input::Keyboard::Key::capsLock;
            case XK_Meta_L: return ouzel::input::Keyboard::Key::leftSuper;
            case XK_Meta_R: return ouzel::input::Keyboard::Key::rightSuper;
            case XK_Hyper_L: return ouzel::input::Keyboard::Key::leftSuper;
            case XK_Hyper_R: return ouzel::input::Keyboard::Key::rightSuper;
            case XK_Alt_L: return ouzel::input::Keyboard::Key::leftAlt;
            case XK_Alt_R: return ouzel::input::Keyboard::Key::rightAlt;
            case XK_ISO_Level3_Shift: return ouzel::input::Keyboard::Key::rightAlt;
            case XK_Menu: return ouzel::input::Keyboard::Key::menu;
            case XK_space: return ouzel::input::Keyboard::Key::space;
            case XK_exclam: return ouzel::input::Keyboard::Key::num1;
            case XK_quotedbl: return ouzel::input::Keyboard::Key::num2;
            case XK_section: return ouzel::input::Keyboard::Key::none; // ?
            case XK_numbersign: return ouzel::input::Keyboard::Key::slash;
            case XK_dollar: return ouzel::input::Keyboard::Key::num4;
            case XK_percent: return ouzel::input::Keyboard::Key::num5;
            case XK_ampersand: return ouzel::input::Keyboard::Key::num7;
            case XK_apostrophe: return ouzel::input::Keyboard::Key::quote;
            case XK_parenleft: return ouzel::input::Keyboard::Key::num9;
            case XK_parenright: return ouzel::input::Keyboard::Key::num0;
            case XK_asterisk: return ouzel::input::Keyboard::Key::num8;
            case XK_plus: return ouzel::input::Keyboard::Key::plus;
            case XK_comma: return ouzel::input::Keyboard::Key::comma;
            case XK_minus: return ouzel::input::Keyboard::Key::minus;
            case XK_period: return ouzel::input::Keyboard::Key::period;
            case XK_slash: return ouzel::input::Keyboard::Key::slash;
            case XK_0: return ouzel::input::Keyboard::Key::num0;
            case XK_1: return ouzel::input::Keyboard::Key::num1;
            case XK_2: return ouzel::input::Keyboard::Key::num2;
            case XK_3: return ouzel::input::Keyboard::Key::num3;
            case XK_4: return ouzel::input::Keyboard::Key::num4;
            case XK_5: return ouzel::input::Keyboard::Key::num5;
            case XK_6: return ouzel::input::Keyboard::Key::num6;
            case XK_7: return ouzel::input::Keyboard::Key::num7;
            case XK_8: return ouzel::input::Keyboard::Key::num8;
            case XK_9: return ouzel::input::Keyboard::Key::num9;
            case XK_colon: return ouzel::input::Keyboard::Key::semicolon;
            case XK_semicolon: return ouzel::input::Keyboard::Key::semicolon;
            case XK_less: return ouzel::input::Keyboard::Key::comma;
            case XK_equal: return ouzel::input::Keyboard::Key::equal;
            case XK_greater: return ouzel::input::Keyboard::Key::period;
            case XK_question: return ouzel::input::Keyboard::Key::slash;
            case XK_at: return ouzel::input::Keyboard::Key::num2; // ?
            case XK_mu: return ouzel::input::Keyboard::Key::none; // ?
            case XK_EuroSign: return ouzel::input::Keyboard::Key::none; // ?
            case XK_A: return ouzel::input::Keyboard::Key::a;
            case XK_B: return ouzel::input::Keyboard::Key::b;
            case XK_C: return ouzel::input::Keyboard::Key::c;
            case XK_D: return ouzel::input::Keyboard::Key::d;
            case XK_E: return ouzel::input::Keyboard::Key::e;
            case XK_F: return ouzel::input::Keyboard::Key::f;
            case XK_G: return ouzel::input::Keyboard::Key::g;
            case XK_H: return ouzel::input::Keyboard::Key::h;
            case XK_I: return ouzel::input::Keyboard::Key::i;
            case XK_J: return ouzel::input::Keyboard::Key::j;
            case XK_K: return ouzel::input::Keyboard::Key::k;
            case XK_L: return ouzel::input::Keyboard::Key::l;
            case XK_M: return ouzel::input::Keyboard::Key::m;
            case XK_N: return ouzel::input::Keyboard::Key::n;
            case XK_O: return ouzel::input::Keyboard::Key::o;
            case XK_P: return ouzel::input::Keyboard::Key::p;
            case XK_Q: return ouzel::input::Keyboard::Key::q;
            case XK_R: return ouzel::input::Keyboard::Key::r;
            case XK_S: return ouzel::input::Keyboard::Key::s;
            case XK_T: return ouzel::input::Keyboard::Key::t;
            case XK_U: return ouzel::input::Keyboard::Key::u;
            case XK_V: return ouzel::input::Keyboard::Key::v;
            case XK_W: return ouzel::input::Keyboard::Key::w;
            case XK_X: return ouzel::input::Keyboard::Key::x;
            case XK_Y: return ouzel::input::Keyboard::Key::y;
            case XK_Z: return ouzel::input::Keyboard::Key::z;
            case XK_bracketleft: return ouzel::input::Keyboard::Key::leftBracket;
            case XK_backslash: return ouzel::input::Keyboard::Key::backslash;
            case XK_bracketright: return ouzel::input::Keyboard::Key::rightBracket;
            case XK_asciicircum: return ouzel::input::Keyboard::Key::backslash;
            case XK_dead_circumflex: return ouzel::input::Keyboard::Key::backslash;
            case XK_degree: return ouzel::input::Keyboard::Key::none; // ?
            case XK_underscore: return ouzel::input::Keyboard::Key::minus;
            case XK_grave: return ouzel::input::Keyboard::Key::grave;
            case XK_dead_grave: return ouzel::input::Keyboard::Key::grave;
            case XK_acute: return ouzel::input::Keyboard::Key::rightBracket;
            case XK_dead_acute: return ouzel::input::Keyboard::Key::rightBracket;
            case XK_a: return ouzel::input::Keyboard::Key::a;
            case XK_b: return ouzel::input::Keyboard::Key::b;
            case XK_c: return ouzel::input::Keyboard::Key::c;
            case XK_d: return ouzel::input::Keyboard::Key::d;
            case XK_e: return ouzel::input::Keyboard::Key::e;
            case XK_f: return ouzel::input::Keyboard::Key::f;
            case XK_g: return ouzel::input::Keyboard::Key::g;
            case XK_h: return ouzel::input::Keyboard::Key::h;
            case XK_i: return ouzel::input::Keyboard::Key::i;
            case XK_j: return ouzel::input::Keyboard::Key::j;
            case XK_k: return ouzel::input::Keyboard::Key::k;
            case XK_l: return ouzel::input::Keyboard::Key::l;
            case XK_m: return ouzel::input::Keyboard::Key::m;
            case XK_n: return ouzel::input::Keyboard::Key::n;
            case XK_o: return ouzel::input::Keyboard::Key::o;
            case XK_p: return ouzel::input::Keyboard::Key::p;
            case XK_q: return ouzel::input::Keyboard::Key::q;
            case XK_r: return ouzel::input::Keyboard::Key::r;
            case XK_s: return ouzel::input::Keyboard::Key::s;
            case XK_t: return ouzel::input::Keyboard::Key::t;
            case XK_u: return ouzel::input::Keyboard::Key::u;
            case XK_v: return ouzel::input::Keyboard::Key::v;
            case XK_w: return ouzel::input::Keyboard::Key::w;
            case XK_x: return ouzel::input::Keyboard::Key::x;
            case XK_y: return ouzel::input::Keyboard::Key::y;
            case XK_z: return ouzel::input::Keyboard::Key::z;
            case XK_ssharp: return ouzel::input::Keyboard::Key::leftBracket;
            case XK_adiaeresis: return ouzel::input::Keyboard::Key::quote;
            case XK_odiaeresis: return ouzel::input::Keyboard::Key::grave;
            case XK_udiaeresis: return ouzel::input::Keyboard::Key::semicolon;
            case XK_Super_L: return ouzel::input::Keyboard::Key::leftSuper;
            case XK_Super_R: return ouzel::input::Keyboard::Key::rightSuper;
            case XK_Mode_switch: return ouzel::input::Keyboard::Key::modeChange;
            case XK_Help: return ouzel::input::Keyboard::Key::help;
            case XK_yen: return ouzel::input::Keyboard::Key::yen;
            case XK_Kanji: return ouzel::input::Keyboard::Key::kanji;
            case XK_Muhenkan: return ouzel::input::Keyboard::Key::muhenkan;
            case XK_Henkan: return ouzel::input::Keyboard::Key::henkan;
            case XK_Romaji: return ouzel::input::Keyboard::Key::ro;
            case XK_Hiragana: return ouzel::input::Keyboard::Key::hiragana;
            case XK_Katakana: return ouzel::input::Keyboard::Key::katakana;
            case XK_Hiragana_Katakana: return ouzel::input::Keyboard::Key::katakanaHiragana;
            default: return ouzel::input::Keyboard::Key::none;
        }
    }

    ouzel::input::Mouse::Button convertButtonCode(unsigned int buttonCode)
    {
        switch (buttonCode)
        {
            case Button1: return ouzel::input::Mouse::Button::left;
            case Button2: return ouzel::input::Mouse::Button::middle;
            case Button3: return ouzel::input::Mouse::Button::right;
            case 8: return ouzel::input::Mouse::Button::x1;
            case 9: return ouzel::input::Mouse::Button::x2;
            default: return ouzel::input::Mouse::Button::none;
        }
    }
}
#endif

namespace ouzel::core::linux
{
    namespace
    {
#if OUZEL_SUPPORTS_X11
        const platform::x11::ErrorCategory errorCategory;
#endif
    }

#if OUZEL_SUPPORTS_X11
    const std::error_category& getErrorCategory() noexcept
    {
        return errorCategory;
    }

    int getLastError() noexcept
    {
        return error;
    }
#endif

    Engine::Engine(const std::vector<std::string>& args):
        core::Engine{args}
    {
#if OUZEL_SUPPORTS_X11
        XSetErrorHandler(errorHandler);

        // open a connection to the X server
        display = XOpenDisplay(nullptr);
        if (!display)
            throw std::system_error{getLastError(), errorCategory, "Failed to open display"};

        executeAtom = XInternAtom(display, "OUZEL_EXECUTE", False);

#elif OUZEL_SUPPORTS_DISPMANX
        bcm_host_init();

        display = vc_dispmanx_display_open(0);
        if (display == DISPMANX_NO_HANDLE)
            throw std::runtime_error{"Failed to open display"};
#endif
    }

    Engine::~Engine()
    {
#if OUZEL_SUPPORTS_X11
        if (display) XCloseDisplay(display);
#elif OUZEL_SUPPORTS_DISPMANX
        if (display != DISPMANX_NO_HANDLE)
            vc_dispmanx_display_close(display);

        bcm_host_deinit();
#endif
    }

    void Engine::run()
    {
        start();

        auto& inputSystemLinux = inputManager.getInputSystem();

#if OUZEL_SUPPORTS_X11
        auto& windowLinux = window.getNativeWindow();

        int xInputOpCode = 0;
        int eventCode;
        int err;
        if (XQueryExtension(display, "XInputExtension", &xInputOpCode, &eventCode, &err) == True)
        {
            int majorVersion = 2;
            int minorVersion = 0;

            if (XIQueryVersion(display, &majorVersion, &minorVersion) != 0)
            {
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

                    XISelectEvents(display, windowLinux.getNativeWindow(), &eventMask, 1);
                }
                else
                    log(Log::Level::warning) << "XInput2 not supported";
            }
            else
                log(Log::Level::warning) << "Failed to query XInput version";
        }
        else
            log(Log::Level::warning) << "XInput not supported";

        while (isActive())
        {
            // XNextEvent will block if there is no event pending, so don't call it if engine is not paused
            if (isPaused() || XPending(display))
            {
                XEvent event;
                XNextEvent(display, &event);

                switch (event.type)
                {
                    case ClientMessage:
                        if (event.xclient.message_type == executeAtom)
                            executeAll();
                        else if (event.xclient.message_type == windowLinux.getProtocolsAtom() &&
                                 static_cast<Atom>(event.xclient.data.l[0]) == windowLinux.getDeleteAtom())
                            exit();
                        break;
                    case FocusIn:
                        if (event.xfocus.window == windowLinux.getNativeWindow())
                            windowLinux.handleFocusIn();
                        break;
                    case FocusOut:
                        if (event.xfocus.window == windowLinux.getNativeWindow())
                            windowLinux.handleFocusOut();
                        break;
                    case KeyPress: // keyboard
                    case KeyRelease:
                        if (event.xkey.window == windowLinux.getNativeWindow())
                        {
                            auto& inputSystemLinux = inputManager.getInputSystem();
                            const auto keyboardDevice = inputSystemLinux.getKeyboardDevice();

                            const auto keySym = XkbKeycodeToKeysym(display,
                                                                   event.xkey.keycode, 0,
                                                                   event.xkey.state & ShiftMask ? 1 : 0);

                            if (event.type == KeyPress)
                                keyboardDevice->handleKeyPress(convertKeyCode(keySym));
                            else
                                keyboardDevice->handleKeyRelease(convertKeyCode(keySym));
                        }
                        break;
                    case ButtonPress: // mouse button
                    case ButtonRelease:
                        if (event.xbutton.window == windowLinux.getNativeWindow())
                        {
                            auto& inputSystemLinux = inputManager.getInputSystem();
                            const auto mouseDevice = inputSystemLinux.getMouseDevice();

                            const math::Vector<float, 2> position{
                                static_cast<float>(event.xbutton.x),
                                static_cast<float>(event.xbutton.y)
                            };

                            if (event.type == ButtonPress)
                                mouseDevice->handleButtonPress(convertButtonCode(event.xbutton.button),
                                                               window.convertWindowToNormalizedLocation(position));
                            else
                                mouseDevice->handleButtonRelease(convertButtonCode(event.xbutton.button),
                                                                 window.convertWindowToNormalizedLocation(position));
                        }
                        break;
                    case MapNotify:
                        if (event.xmap.window == windowLinux.getNativeWindow())
                            windowLinux.handleMap();
                        break;
                    case UnmapNotify:
                        if (event.xmap.window == windowLinux.getNativeWindow())
                            windowLinux.handleUnmap();
                        break;
                    case MotionNotify:
                        if (event.xbutton.window == windowLinux.getNativeWindow())
                        {
                            auto& inputSystemLinux = inputManager.getInputSystem();
                            const auto mouseDevice = inputSystemLinux.getMouseDevice();

                            const math::Vector<float, 2> position{
                                static_cast<float>(event.xmotion.x),
                                static_cast<float>(event.xmotion.y)
                            };

                            mouseDevice->handleMove(window.convertWindowToNormalizedLocation(position));
                        }
                        break;
                    case ConfigureNotify:
                        if (event.xconfigure.window == windowLinux.getNativeWindow())
                        {
                            const math::Size<std::uint32_t, 2> size{
                                static_cast<std::uint32_t>(event.xconfigure.width),
                                static_cast<std::uint32_t>(event.xconfigure.height)
                            };

                            windowLinux.handleResize(size);
                        }
                        break;
                    case Expose:
                    {
                        // need to redraw
                        break;
                    }
                    case GenericEvent:
                        if (event.xcookie.extension == xInputOpCode)
                        {
                            auto& inputSystemLinux = inputManager.getInputSystem();
                            const auto touchpadDevice = inputSystemLinux.getTouchpadDevice();

                            switch (event.xcookie.evtype)
                            {
                                case XI_TouchBegin:
                                {
                                    const auto xievent = static_cast<XIDeviceEvent*>(event.xcookie.data);
                                    const math::Vector<float, 2> position{
                                        static_cast<float>(xievent->event_x),
                                        static_cast<float>(xievent->event_y)
                                    };

                                    touchpadDevice->handleTouchBegin(xievent->detail,
                                                                     window.convertWindowToNormalizedLocation(position));
                                    break;
                                }
                                case XI_TouchEnd:
                                {
                                    const auto xievent = static_cast<XIDeviceEvent*>(event.xcookie.data);
                                    const math::Vector<float, 2> position{
                                        static_cast<float>(xievent->event_x),
                                        static_cast<float>(xievent->event_y)
                                    };

                                    touchpadDevice->handleTouchEnd(xievent->detail,
                                                                   window.convertWindowToNormalizedLocation(position));
                                    break;
                                }
                                case XI_TouchUpdate:
                                {
                                    const auto xievent = static_cast<XIDeviceEvent*>(event.xcookie.data);
                                    const math::Vector<float, 2> position{
                                        static_cast<float>(xievent->event_x),
                                        static_cast<float>(xievent->event_y)
                                    };

                                    touchpadDevice->handleTouchMove(xievent->detail,
                                                                    window.convertWindowToNormalizedLocation(position));
                                    break;
                                }
                            }
                        }
                        break;
                }
            }

            inputSystemLinux.update();
        }
#else
        while (isActive())
        {
            executeAll();

            inputSystemLinux.update();
        }
#endif

        exit();
    }

    void Engine::runOnMainThread(const std::function<void()>& func)
    {
#if OUZEL_SUPPORTS_X11
        const auto& windowLinux = window.getNativeWindow();

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

        std::unique_lock lock{executeMutex};
        executeQueue.push(func);
        lock.unlock();

        if (XSendEvent(display, windowLinux.getNativeWindow(), False, NoEventMask, &event) == 0)
            throw std::system_error{getLastError(), errorCategory, "Failed to send X11 delete message"};

        XFlush(display);
#else
        std::scoped_lock lock{executeMutex};
        executeQueue.push(func);
#endif
    }

    void Engine::openUrl(const std::string& url)
    {
#if OUZEL_SUPPORTS_X11
        ::exit(execl("/usr/bin/xdg-open", "xdg-open", url.c_str(), nullptr));
#endif
    }

    void Engine::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        core::Engine::setScreenSaverEnabled(newScreenSaverEnabled);

#if OUZEL_SUPPORTS_X11
        executeOnMainThread([this, newScreenSaverEnabled]() {
            XScreenSaverSuspend(display, !newScreenSaverEnabled);
        });
#endif
    }

    void Engine::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock lock{executeMutex};

            if (executeQueue.empty()) break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }
}
