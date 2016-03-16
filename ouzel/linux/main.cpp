// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include "Engine.h"
#include "Utils.h"

void ouzelMain(const std::vector<std::string>& args);

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

static input::KeyboardKey convertKeyCode(KeySym keyCode)
{
    switch(keyCode)
    {
        case XK_BackSpace: return input::KeyboardKey::BACKSPACE;
        case XK_Tab: return input::KeyboardKey::TAB;
        case XK_ISO_Left_Tab: return input::KeyboardKey::TAB;
        case XK_Linefeed: return input::KeyboardKey::NONE; // ???
        case XK_Clear: return input::KeyboardKey::CLEAR;
        case XK_Return: return input::KeyboardKey::RETURN;
        case XK_Pause: return input::KeyboardKey::PAUSE;
        case XK_Scroll_Lock: return input::KeyboardKey::SCROLL;
        case XK_Sys_Req: return input::KeyboardKey::NONE; // ???
        case XK_Escape: return input::KeyboardKey::ESCAPE;
        case XK_Insert: return input::KeyboardKey::INSERT;
        case XK_Delete: return input::KeyboardKey::DEL;
        case XK_Home: return input::KeyboardKey::HOME;
        case XK_Left: return input::KeyboardKey::LEFT;
        case XK_Up: return input::KeyboardKey::UP;
        case XK_Right: return input::KeyboardKey::RIGHT;
        case XK_Down: return input::KeyboardKey::DOWN;
        case XK_Prior: return input::KeyboardKey::PRIOR; // also XK_Page_Up
        case XK_Next: return input::KeyboardKey::NEXT; // also XK_Page_Down
        case XK_End: return input::KeyboardKey::END;
        case XK_Begin: return input::KeyboardKey::HOME;
        case XK_Num_Lock: return input::KeyboardKey::NUMLOCK;
        case XK_KP_Space: return input::KeyboardKey::SPACE;
        case XK_KP_Tab: return input::KeyboardKey::TAB;
        case XK_KP_Enter: return input::KeyboardKey::RETURN;
        case XK_KP_F1: return input::KeyboardKey::F1;
        case XK_KP_F2: return input::KeyboardKey::F2;
        case XK_KP_F3: return input::KeyboardKey::F3;
        case XK_KP_F4: return input::KeyboardKey::F4;
        case XK_KP_Home: return input::KeyboardKey::HOME;
        case XK_KP_Left: return input::KeyboardKey::LEFT;
        case XK_KP_Up: return input::KeyboardKey::UP;
        case XK_KP_Right: return input::KeyboardKey::RIGHT;
        case XK_KP_Down: return input::KeyboardKey::DOWN;
        case XK_Print: return input::KeyboardKey::PRINT;
        case XK_KP_Prior: return input::KeyboardKey::PRIOR; // alos XK_KP_Page_Up
        case XK_KP_Next: return input::KeyboardKey::NEXT; // also XK_KP_Page_Down
        case XK_KP_End: return input::KeyboardKey::END;
        case XK_KP_Begin: return input::KeyboardKey::HOME;
        case XK_KP_Insert: return input::KeyboardKey::INSERT;
        case XK_KP_Delete: return input::KeyboardKey::DEL;
        case XK_KP_Equal: return input::KeyboardKey::NONE; // ???
        case XK_KP_Multiply: return input::KeyboardKey::MULTIPLY;
        case XK_KP_Add: return input::KeyboardKey::ADD;
        case XK_KP_Separator: return input::KeyboardKey::SEPARATOR;
        case XK_KP_Subtract: return input::KeyboardKey::SUBTRACT;
        case XK_KP_Decimal: return input::KeyboardKey::DECIMAL;
        case XK_KP_Divide: return input::KeyboardKey::DIVIDE;
        case XK_KP_0: return input::KeyboardKey::NUMPAD0;
        case XK_KP_1: return input::KeyboardKey::NUMPAD1;
        case XK_KP_2: return input::KeyboardKey::NUMPAD2;
        case XK_KP_3: return input::KeyboardKey::NUMPAD3;
        case XK_KP_4: return input::KeyboardKey::NUMPAD4;
        case XK_KP_5: return input::KeyboardKey::NUMPAD5;
        case XK_KP_6: return input::KeyboardKey::NUMPAD6;
        case XK_KP_7: return input::KeyboardKey::NUMPAD7;
        case XK_KP_8: return input::KeyboardKey::NUMPAD8;
        case XK_KP_9: return input::KeyboardKey::NUMPAD9;
        case XK_F1: return input::KeyboardKey::F1;
        case XK_F2: return input::KeyboardKey::F2;
        case XK_F3: return input::KeyboardKey::F3;
        case XK_F4: return input::KeyboardKey::F4;
        case XK_F5: return input::KeyboardKey::F5;
        case XK_F6: return input::KeyboardKey::F6;
        case XK_F7: return input::KeyboardKey::F7;
        case XK_F8: return input::KeyboardKey::F8;
        case XK_F9: return input::KeyboardKey::F9;
        case XK_F10: return input::KeyboardKey::F10;
        case XK_F11: return input::KeyboardKey::F11;
        case XK_F12: return input::KeyboardKey::F12;
        case XK_Shift_L: return input::KeyboardKey::LSHIFT;
        case XK_Shift_R: return input::KeyboardKey::RSHIFT;
        case XK_Control_L: return input::KeyboardKey::LCONTROL;
        case XK_Control_R: return input::KeyboardKey::RCONTROL;
        case XK_Caps_Lock: return input::KeyboardKey::CAPITAL;
        case XK_Shift_Lock: return input::KeyboardKey::CAPITAL;
        case XK_Meta_L: return input::KeyboardKey::LWIN;
        case XK_Meta_R: return input::KeyboardKey::RWIN;
        case XK_Alt_L: return input::KeyboardKey::LMENU;
        case XK_Alt_R: return input::KeyboardKey::RMENU;
        case XK_ISO_Level3_Shift: return input::KeyboardKey::RMENU;
        case XK_Menu: return input::KeyboardKey::MENU;
        case XK_space: return input::KeyboardKey::SPACE;
        case XK_exclam: return input::KeyboardKey::NONE; //?
        case XK_quotedbl: return input::KeyboardKey::NONE; //?
        case XK_section: return input::KeyboardKey::NONE; //?
        case XK_numbersign: return input::KeyboardKey::OEM_2;
        case XK_dollar: return input::KeyboardKey::NONE; //?
        case XK_percent: return input::KeyboardKey::NONE; //?
        case XK_ampersand: return input::KeyboardKey::NONE; //?
        case XK_apostrophe: return input::KeyboardKey::OEM_7;
        case XK_parenleft: return input::KeyboardKey::NONE; //?
        case XK_parenright: return input::KeyboardKey::NONE; //?
        case XK_asterisk: return input::KeyboardKey::NONE; //?
        case XK_plus: return input::KeyboardKey::PLUS; //?
        case XK_comma: return input::KeyboardKey::COMMA; //?
        case XK_minus: return input::KeyboardKey::MINUS; //?
        case XK_period: return input::KeyboardKey::PERIOD; //?
        case XK_slash: return input::KeyboardKey::OEM_2; //?
        case XK_0: return input::KeyboardKey::KEY_0;
        case XK_1: return input::KeyboardKey::KEY_1;
        case XK_2: return input::KeyboardKey::KEY_2;
        case XK_3: return input::KeyboardKey::KEY_3;
        case XK_4: return input::KeyboardKey::KEY_4;
        case XK_5: return input::KeyboardKey::KEY_5;
        case XK_6: return input::KeyboardKey::KEY_6;
        case XK_7: return input::KeyboardKey::KEY_7;
        case XK_8: return input::KeyboardKey::KEY_8;
        case XK_9: return input::KeyboardKey::KEY_9;
        case XK_colon: return input::KeyboardKey::NONE; //?
        case XK_semicolon: return input::KeyboardKey::OEM_1;
        case XK_less: return input::KeyboardKey::OEM_102;
        case XK_equal: return input::KeyboardKey::PLUS;
        case XK_greater: return input::KeyboardKey::NONE; //?
        case XK_question: return input::KeyboardKey::NONE; //?
        case XK_at: return input::KeyboardKey::KEY_2; //?
        case XK_mu: return input::KeyboardKey::NONE; //?
        case XK_EuroSign: return input::KeyboardKey::NONE; //?
        case XK_A: return input::KeyboardKey::KEY_A;
        case XK_B: return input::KeyboardKey::KEY_B;
        case XK_C: return input::KeyboardKey::KEY_C;
        case XK_D: return input::KeyboardKey::KEY_D;
        case XK_E: return input::KeyboardKey::KEY_E;
        case XK_F: return input::KeyboardKey::KEY_F;
        case XK_G: return input::KeyboardKey::KEY_G;
        case XK_H: return input::KeyboardKey::KEY_H;
        case XK_I: return input::KeyboardKey::KEY_I;
        case XK_J: return input::KeyboardKey::KEY_J;
        case XK_K: return input::KeyboardKey::KEY_K;
        case XK_L: return input::KeyboardKey::KEY_L;
        case XK_M: return input::KeyboardKey::KEY_M;
        case XK_N: return input::KeyboardKey::KEY_N;
        case XK_O: return input::KeyboardKey::KEY_O;
        case XK_P: return input::KeyboardKey::KEY_P;
        case XK_Q: return input::KeyboardKey::KEY_Q;
        case XK_R: return input::KeyboardKey::KEY_R;
        case XK_S: return input::KeyboardKey::KEY_S;
        case XK_T: return input::KeyboardKey::KEY_T;
        case XK_U: return input::KeyboardKey::KEY_U;
        case XK_V: return input::KeyboardKey::KEY_V;
        case XK_W: return input::KeyboardKey::KEY_W;
        case XK_X: return input::KeyboardKey::KEY_X;
        case XK_Y: return input::KeyboardKey::KEY_Y;
        case XK_Z: return input::KeyboardKey::KEY_Z;
        case XK_bracketleft: return input::KeyboardKey::OEM_4;
        case XK_backslash: return input::KeyboardKey::OEM_5;
        case XK_bracketright: return input::KeyboardKey::OEM_6;
        case XK_asciicircum: return input::KeyboardKey::OEM_5;
        case XK_dead_circumflex: return input::KeyboardKey::OEM_5;
        case XK_degree: return input::KeyboardKey::NONE; //?
        case XK_underscore: return input::KeyboardKey::MINUS; //?
        case XK_grave: return input::KeyboardKey::OEM_3;
        case XK_dead_grave: return input::KeyboardKey::OEM_3;
        case XK_acute: return input::KeyboardKey::OEM_6;
        case XK_dead_acute: return input::KeyboardKey::OEM_6;
        case XK_a: return input::KeyboardKey::KEY_A;
        case XK_b: return input::KeyboardKey::KEY_B;
        case XK_c: return input::KeyboardKey::KEY_C;
        case XK_d: return input::KeyboardKey::KEY_D;
        case XK_e: return input::KeyboardKey::KEY_E;
        case XK_f: return input::KeyboardKey::KEY_F;
        case XK_g: return input::KeyboardKey::KEY_G;
        case XK_h: return input::KeyboardKey::KEY_H;
        case XK_i: return input::KeyboardKey::KEY_I;
        case XK_j: return input::KeyboardKey::KEY_J;
        case XK_k: return input::KeyboardKey::KEY_K;
        case XK_l: return input::KeyboardKey::KEY_L;
        case XK_m: return input::KeyboardKey::KEY_M;
        case XK_n: return input::KeyboardKey::KEY_N;
        case XK_o: return input::KeyboardKey::KEY_O;
        case XK_p: return input::KeyboardKey::KEY_P;
        case XK_q: return input::KeyboardKey::KEY_Q;
        case XK_r: return input::KeyboardKey::KEY_R;
        case XK_s: return input::KeyboardKey::KEY_S;
        case XK_t: return input::KeyboardKey::KEY_T;
        case XK_u: return input::KeyboardKey::KEY_U;
        case XK_v: return input::KeyboardKey::KEY_V;
        case XK_w: return input::KeyboardKey::KEY_W;
        case XK_x: return input::KeyboardKey::KEY_X;
        case XK_y: return input::KeyboardKey::KEY_Y;
        case XK_z: return input::KeyboardKey::KEY_Z;
        case XK_ssharp: return input::KeyboardKey::OEM_4;
        case XK_adiaeresis: return input::KeyboardKey::OEM_7;
        case XK_odiaeresis: return input::KeyboardKey::OEM_3;
        case XK_udiaeresis: return input::KeyboardKey::OEM_1;
        case XK_Super_L: return input::KeyboardKey::LWIN;
        case XK_Super_R: return input::KeyboardKey::RWIN;
        default: return input::KeyboardKey::NONE;
    }
}

int main(int argc, char **argv)
{
    std::vector<std::string> args;

    for (int32_t i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    ouzel::setArgs(args);

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

    ouzelMain(getArgs());
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

                    input::MouseButton button;

                    switch (event.xbutton.button)
                    {
                    case 1:
                        button = input::MouseButton::LEFT;
                        break;
                    case 2:
                        button = input::MouseButton::RIGHT;
                        break;
                    case 3:
                        button = input::MouseButton::MIDDLE;
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
