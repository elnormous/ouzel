// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include "Engine.h"
#include "Utils.h"

using namespace ouzel;

static uint32_t getModifiers(unsigned int state)
{
    uint32_t modifiers = 0;

    if (state & ShiftMask) modifiers |= Event::SHIFT_DOWN;
    if (state & ControlMask) modifiers |= Event::CONTROL_DOWN;
    if (state & Button1) modifiers |= Event::LEFT_MOUSE_DOWN;
    if (state & Button2) modifiers |= Event::RIGHT_MOUSE_DOWN;
    if (state & Button3) modifiers |= Event::MIDDLE_MOUSE_DOWN;

    return modifiers;
}

static KeyboardKey convertKeyCode(KeySym keyCode)
{
    switch(keyCode)
    {
        case XK_BackSpace: return KeyboardKey::BACKSPACE;
        case XK_Tab: return KeyboardKey::TAB;
        case XK_ISO_Left_Tab: return KeyboardKey::TAB;
        case XK_Linefeed: return KeyboardKey::NONE; // ???
        case XK_Clear: return KeyboardKey::CLEAR;
        case XK_Return: return KeyboardKey::RETURN;
        case XK_Pause: return KeyboardKey::PAUSE;
        case XK_Scroll_Lock: return KeyboardKey::SCROLL;
        case XK_Sys_Req: return KeyboardKey::NONE; // ???
        case XK_Escape: return KeyboardKey::ESCAPE;
        case XK_Insert: return KeyboardKey::INSERT;
        case XK_Delete: return KeyboardKey::DEL;
        case XK_Home: return KeyboardKey::HOME;
        case XK_Left: return KeyboardKey::LEFT;
        case XK_Up: return KeyboardKey::UP;
        case XK_Right: return KeyboardKey::RIGHT;
        case XK_Down: return KeyboardKey::DOWN;
        case XK_Prior: return KeyboardKey::PRIOR; // also XK_Page_Up
        case XK_Next: return KeyboardKey::NEXT; // also XK_Page_Down
        case XK_End: return KeyboardKey::END;
        case XK_Begin: return KeyboardKey::HOME;
        case XK_Num_Lock: return KeyboardKey::NUMLOCK;
        case XK_KP_Space: return KeyboardKey::SPACE;
        case XK_KP_Tab: return KeyboardKey::TAB;
        case XK_KP_Enter: return KeyboardKey::RETURN;
        case XK_KP_F1: return KeyboardKey::F1;
        case XK_KP_F2: return KeyboardKey::F2;
        case XK_KP_F3: return KeyboardKey::F3;
        case XK_KP_F4: return KeyboardKey::F4;
        case XK_KP_Home: return KeyboardKey::HOME;
        case XK_KP_Left: return KeyboardKey::LEFT;
        case XK_KP_Up: return KeyboardKey::UP;
        case XK_KP_Right: return KeyboardKey::RIGHT;
        case XK_KP_Down: return KeyboardKey::DOWN;
        case XK_Print: return KeyboardKey::PRINT;
        case XK_KP_Prior: return KeyboardKey::PRIOR; // alos XK_KP_Page_Up
        case XK_KP_Next: return KeyboardKey::NEXT; // also XK_KP_Page_Down
        case XK_KP_End: return KeyboardKey::END;
        case XK_KP_Begin: return KeyboardKey::HOME;
        case XK_KP_Insert: return KeyboardKey::INSERT;
        case XK_KP_Delete: return KeyboardKey::DEL;
        case XK_KP_Equal: return KeyboardKey::NONE; // ???
        case XK_KP_Multiply: return KeyboardKey::MULTIPLY;
        case XK_KP_Add: return KeyboardKey::ADD;
        case XK_KP_Separator: return KeyboardKey::SEPARATOR;
        case XK_KP_Subtract: return KeyboardKey::SUBTRACT;
        case XK_KP_Decimal: return KeyboardKey::DECIMAL;
        case XK_KP_Divide: return KeyboardKey::DIVIDE;
        case XK_KP_0: return KeyboardKey::NUMPAD0;
        case XK_KP_1: return KeyboardKey::NUMPAD1;
        case XK_KP_2: return KeyboardKey::NUMPAD2;
        case XK_KP_3: return KeyboardKey::NUMPAD3;
        case XK_KP_4: return KeyboardKey::NUMPAD4;
        case XK_KP_5: return KeyboardKey::NUMPAD5;
        case XK_KP_6: return KeyboardKey::NUMPAD6;
        case XK_KP_7: return KeyboardKey::NUMPAD7;
        case XK_KP_8: return KeyboardKey::NUMPAD8;
        case XK_KP_9: return KeyboardKey::NUMPAD9;
        case XK_F1: return KeyboardKey::F1;
        case XK_F2: return KeyboardKey::F2;
        case XK_F3: return KeyboardKey::F3;
        case XK_F4: return KeyboardKey::F4;
        case XK_F5: return KeyboardKey::F5;
        case XK_F6: return KeyboardKey::F6;
        case XK_F7: return KeyboardKey::F7;
        case XK_F8: return KeyboardKey::F8;
        case XK_F9: return KeyboardKey::F9;
        case XK_F10: return KeyboardKey::F10;
        case XK_F11: return KeyboardKey::F11;
        case XK_F12: return KeyboardKey::F12;
        case XK_Shift_L: return KeyboardKey::LSHIFT;
        case XK_Shift_R: return KeyboardKey::RSHIFT;
        case XK_Control_L: return KeyboardKey::LCONTROL;
        case XK_Control_R: return KeyboardKey::RCONTROL;
        case XK_Caps_Lock: return KeyboardKey::CAPITAL;
        case XK_Shift_Lock: return KeyboardKey::CAPITAL;
        case XK_Meta_L: return KeyboardKey::LWIN;
        case XK_Meta_R: return KeyboardKey::RWIN;
        case XK_Alt_L: return KeyboardKey::LMENU;
        case XK_Alt_R: return KeyboardKey::RMENU;
        case XK_ISO_Level3_Shift: return KeyboardKey::RMENU;
        case XK_Menu: return KeyboardKey::MENU;
        case XK_space: return KeyboardKey::SPACE;
        case XK_exclam: return KeyboardKey::NONE; //?
        case XK_quotedbl: return KeyboardKey::NONE; //?
        case XK_section: return KeyboardKey::NONE; //?
        case XK_numbersign: return KeyboardKey::OEM_2;
        case XK_dollar: return KeyboardKey::NONE; //?
        case XK_percent: return KeyboardKey::NONE; //?
        case XK_ampersand: return KeyboardKey::NONE; //?
        case XK_apostrophe: return KeyboardKey::OEM_7;
        case XK_parenleft: return KeyboardKey::NONE; //?
        case XK_parenright: return KeyboardKey::NONE; //?
        case XK_asterisk: return KeyboardKey::NONE; //?
        case XK_plus: return KeyboardKey::PLUS; //?
        case XK_comma: return KeyboardKey::COMMA; //?
        case XK_minus: return KeyboardKey::MINUS; //?
        case XK_period: return KeyboardKey::PERIOD; //?
        case XK_slash: return KeyboardKey::OEM_2; //?
        case XK_0: return KeyboardKey::KEY_0;
        case XK_1: return KeyboardKey::KEY_1;
        case XK_2: return KeyboardKey::KEY_2;
        case XK_3: return KeyboardKey::KEY_3;
        case XK_4: return KeyboardKey::KEY_4;
        case XK_5: return KeyboardKey::KEY_5;
        case XK_6: return KeyboardKey::KEY_6;
        case XK_7: return KeyboardKey::KEY_7;
        case XK_8: return KeyboardKey::KEY_8;
        case XK_9: return KeyboardKey::KEY_9;
        case XK_colon: return KeyboardKey::NONE; //?
        case XK_semicolon: return KeyboardKey::OEM_1;
        case XK_less: return KeyboardKey::OEM_102;
        case XK_equal: return KeyboardKey::PLUS;
        case XK_greater: return KeyboardKey::NONE; //?
        case XK_question: return KeyboardKey::NONE; //?
        case XK_at: return KeyboardKey::KEY_2; //?
        case XK_mu: return KeyboardKey::NONE; //?
        case XK_EuroSign: return KeyboardKey::NONE; //?
        case XK_A: return KeyboardKey::KEY_A;
        case XK_B: return KeyboardKey::KEY_B;
        case XK_C: return KeyboardKey::KEY_C;
        case XK_D: return KeyboardKey::KEY_D;
        case XK_E: return KeyboardKey::KEY_E;
        case XK_F: return KeyboardKey::KEY_F;
        case XK_G: return KeyboardKey::KEY_G;
        case XK_H: return KeyboardKey::KEY_H;
        case XK_I: return KeyboardKey::KEY_I;
        case XK_J: return KeyboardKey::KEY_J;
        case XK_K: return KeyboardKey::KEY_K;
        case XK_L: return KeyboardKey::KEY_L;
        case XK_M: return KeyboardKey::KEY_M;
        case XK_N: return KeyboardKey::KEY_N;
        case XK_O: return KeyboardKey::KEY_O;
        case XK_P: return KeyboardKey::KEY_P;
        case XK_Q: return KeyboardKey::KEY_Q;
        case XK_R: return KeyboardKey::KEY_R;
        case XK_S: return KeyboardKey::KEY_S;
        case XK_T: return KeyboardKey::KEY_T;
        case XK_U: return KeyboardKey::KEY_U;
        case XK_V: return KeyboardKey::KEY_V;
        case XK_W: return KeyboardKey::KEY_W;
        case XK_X: return KeyboardKey::KEY_X;
        case XK_Y: return KeyboardKey::KEY_Y;
        case XK_Z: return KeyboardKey::KEY_Z;
        case XK_bracketleft: return KeyboardKey::OEM_4;
        case XK_backslash: return KeyboardKey::OEM_5;
        case XK_bracketright: return KeyboardKey::OEM_6;
        case XK_asciicircum: return KeyboardKey::OEM_5;
        case XK_dead_circumflex: return KeyboardKey::OEM_5;
        case XK_degree: return KeyboardKey::NONE; //?
        case XK_underscore: return KeyboardKey::MINUS; //?
        case XK_grave: return KeyboardKey::OEM_3;
        case XK_dead_grave: return KeyboardKey::OEM_3;
        case XK_acute: return KeyboardKey::OEM_6;
        case XK_dead_acute: return KeyboardKey::OEM_6;
        case XK_a: return KeyboardKey::KEY_A;
        case XK_b: return KeyboardKey::KEY_B;
        case XK_c: return KeyboardKey::KEY_C;
        case XK_d: return KeyboardKey::KEY_D;
        case XK_e: return KeyboardKey::KEY_E;
        case XK_f: return KeyboardKey::KEY_F;
        case XK_g: return KeyboardKey::KEY_G;
        case XK_h: return KeyboardKey::KEY_H;
        case XK_i: return KeyboardKey::KEY_I;
        case XK_j: return KeyboardKey::KEY_J;
        case XK_k: return KeyboardKey::KEY_K;
        case XK_l: return KeyboardKey::KEY_L;
        case XK_m: return KeyboardKey::KEY_M;
        case XK_n: return KeyboardKey::KEY_N;
        case XK_o: return KeyboardKey::KEY_O;
        case XK_p: return KeyboardKey::KEY_P;
        case XK_q: return KeyboardKey::KEY_Q;
        case XK_r: return KeyboardKey::KEY_R;
        case XK_s: return KeyboardKey::KEY_S;
        case XK_t: return KeyboardKey::KEY_T;
        case XK_u: return KeyboardKey::KEY_U;
        case XK_v: return KeyboardKey::KEY_V;
        case XK_w: return KeyboardKey::KEY_W;
        case XK_x: return KeyboardKey::KEY_X;
        case XK_y: return KeyboardKey::KEY_Y;
        case XK_z: return KeyboardKey::KEY_Z;
        case XK_ssharp: return KeyboardKey::OEM_4;
        case XK_adiaeresis: return KeyboardKey::OEM_7;
        case XK_odiaeresis: return KeyboardKey::OEM_3;
        case XK_udiaeresis: return KeyboardKey::OEM_1;
        case XK_Super_L: return KeyboardKey::LWIN;
        case XK_Super_R: return KeyboardKey::RWIN;
        default: return KeyboardKey::NONE;
    }
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;

    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    ouzel::Engine::getInstance()->setArgs(args);

    // open a connection to the X server
    Display* display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        ouzel::log("Failed to open display");
        return 1;
    }

    // make sure OpenGL's GLX extension supported
    int dummy;
    if (!glXQueryExtension(display, &dummy, &dummy))
    {
        ouzel::log("X server has no OpenGL GLX extension");
        return 1;
    }

    // find an OpenGL-capable RGB visual with depth buffer
    static int doubleBuffer[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

    XVisualInfo* vi = glXChooseVisual(display, DefaultScreen(display), doubleBuffer);
    if (vi == NULL)
    {
        ouzel::log("No RGB visual with depth buffer");
        return 1;
    }
    if (vi->c_class != TrueColor)
    {
        ouzel::log("TrueColor visual required for this program");
        return 1;
    }

    // create an OpenGL rendering context
    GLXContext context = glXCreateContext(display, vi, /* no shared dlists */ None,
                                          /* direct rendering if possible */ GL_TRUE);
    if (context == NULL)
    {
        ouzel::log("Failed to create rendering context");
        return 1;
    }

    // create an X colormap since probably not using default visual
    Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;
    Window window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                                  300, 300, 0, vi->depth, InputOutput, vi->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &swa);
    XSetStandardProperties(display, window, "ouzel", "ouzel", None, argv, argc, NULL);

    // bind the rendering context to the window
    glXMakeCurrent(display, window, context);

    // request the X window to be displayed on the screen
    XMapWindow(display, window);

    ouzel::Engine::getInstance()->init();
    ouzel::Engine::getInstance()->begin();

    XEvent event;

    while (true)
    {
        do
        {
            XNextEvent(display, &event);
            switch (event.type)
            {
                case KeyPress: // keyboard
                case KeyRelease:
                {
                    KeySym keySym = XKeycodeToKeysym(display, event.xkey.keycode, 0);

                    if (event.type == KeyPress)
                    {
                        Engine::getInstance()->getInput()->keyDown(convertKeyCode(keySym), getModifiers(event.xkey.state));
                    }
                    else
                    {
                        Engine::getInstance()->getInput()->keyUp(convertKeyCode(keySym), getModifiers(event.xkey.state));
                    }
                    break;
                }
                case ButtonPress: // mouse button
                case ButtonRelease:
                {
                    Vector2 pos(static_cast<float>(event.xbutton.x),
                                static_cast<float>(event.xbutton.y));

                    MouseButton button;

                    switch (event.xbutton.button)
                    {
                    case 1:
                        button = MouseButton::LEFT;
                        break;
                    case 2:
                        button = MouseButton::RIGHT;
                        break;
                    case 3:
                        button = MouseButton::MIDDLE;
                        break;
                    }

                    if (event.type == ButtonPress)
                    {
                        Engine::getInstance()->getInput()->mouseDown(button,
                                                                     Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                     getModifiers(event.xbutton.state));
                    }
                    else
                    {
                        Engine::getInstance()->getInput()->mouseUp(button,
                                                                   Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                   getModifiers(event.xbutton.state));
                    }
                    break;
                }
                case MotionNotify:
                {
                    Vector2 pos(static_cast<float>(event.xmotion.x),
                                static_cast<float>(event.xmotion.y));

                    Engine::getInstance()->getInput()->mouseMove(Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                                 getModifiers(event.xmotion.state));

                    break;
                }
                case ConfigureNotify:
                {
                    ouzel::Engine::getInstance()->getRenderer()->resize(ouzel::Size2(event.xconfigure.width, event.xconfigure.height));
                    //needRedraw = true;
                    break;
                }
                case Expose:
                {
                    // needRedraw = true;
                    break;
                }
          }
        }
        while (XPending(display)); /* loop to compress events */

        if (!ouzel::Engine::getInstance()->run())
        {
            break;
        }
        glXSwapBuffers(display, window);
    }

    if (display)
    {
        if (context)
        {
            glXDestroyContext(display, context);
        }

        if (window)
        {
            XDestroyWindow(display, window);
        }

	    XCloseDisplay(display);
	}

    ouzel::Engine::getInstance()->end();

    return 0;
}
