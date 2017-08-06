// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <X11/cursorfont.h>
#include "InputLinux.hpp"
#include "CursorInterfaceLinux.hpp"
#include "events/Event.hpp"
#include "core/Engine.hpp"
#include "core/linux/WindowLinux.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::map<KeySym, KeyboardKey> keyMap = {
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
            {XK_Shift_L, KeyboardKey::LSHIFT},
            {XK_Shift_R, KeyboardKey::RSHIFT},
            {XK_Control_L, KeyboardKey::LCONTROL},
            {XK_Control_R, KeyboardKey::RCONTROL},
            {XK_Caps_Lock, KeyboardKey::CAPITAL},
            {XK_Shift_Lock, KeyboardKey::CAPITAL},
            {XK_Meta_L, KeyboardKey::LSUPER},
            {XK_Meta_R, KeyboardKey::RSUPER},
            {XK_Alt_L, KeyboardKey::LALT},
            {XK_Alt_R, KeyboardKey::RALT},
            {XK_ISO_Level3_Shift, KeyboardKey::RSUPER},
            {XK_Menu, KeyboardKey::LSUPER},
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
            {XK_bracketleft, KeyboardKey::BRACKET_LEFT},
            {XK_backslash, KeyboardKey::BACKSLASH},
            {XK_bracketright, KeyboardKey::BRACKET_RIGHT},
            {XK_asciicircum, KeyboardKey::BACKSLASH},
            {XK_dead_circumflex, KeyboardKey::BACKSLASH},
            {XK_degree, KeyboardKey::NONE}, // ?
            {XK_underscore, KeyboardKey::MINUS},
            {XK_grave, KeyboardKey::GRAVE},
            {XK_dead_grave, KeyboardKey::GRAVE},
            {XK_acute, KeyboardKey::BRACKET_RIGHT},
            {XK_dead_acute, KeyboardKey::BRACKET_RIGHT},
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
            {XK_ssharp, KeyboardKey::BRACKET_LEFT},
            {XK_adiaeresis, KeyboardKey::QUOTE},
            {XK_odiaeresis, KeyboardKey::GRAVE},
            {XK_udiaeresis, KeyboardKey::SEMICOLON},
            {XK_Super_L, KeyboardKey::LSUPER},
            {XK_Super_R, KeyboardKey::RSUPER}
        };

        KeyboardKey InputLinux::convertKeyCode(KeySym keyCode)
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

        uint32_t InputLinux::getModifiers(unsigned int state)
        {
            uint32_t modifiers = 0;

            if (state & ShiftMask) modifiers |= SHIFT_DOWN;
            if (state & ControlMask) modifiers |= CONTROL_DOWN;
            if (state & Button1Mask) modifiers |= LEFT_MOUSE_DOWN;
            if (state & Button2Mask) modifiers |= RIGHT_MOUSE_DOWN;
            if (state & Button3Mask) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }

        InputLinux::InputLinux()
        {
        }

        bool InputLinux::init()
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
            Display* display = windowLinux->getDisplay();

            char data[1] = {0};

            Pixmap pixmap = XCreateBitmapFromData(display, DefaultRootWindow(display), data, 1, 1);
            if (pixmap)
            {
                XColor color;
                color.red = color.green = color.blue = 0;

                emptyCursor = XCreatePixmapCursor(display, pixmap, pixmap, &color, &color, 0, 0);
                XFreePixmap(display, pixmap);
            }

            return true;
        }

        InputLinux::~InputLinux()
        {
            if (sharedEngine)
            {
                WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                Display* display = windowLinux->getDisplay();
                if (emptyCursor != None) XFreeCursor(display, emptyCursor);
            }
        }

        void InputLinux::activateCursorResource(CursorInterface* resource)
        {
            Input::activateCursorResource(resource);

            if (sharedEngine)
            {
                WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                Display* display = windowLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                CursorInterfaceLinux* cursorLinux = static_cast<CursorInterfaceLinux*>(resource);

                if (cursorLinux)
                {
                    currentCursor = cursorLinux->getNativeCursor();
                    if (!currentCursor) currentCursor = emptyCursor;
                }
                else
                {
                    currentCursor = None;
                }

                if (cursorVisible)
                {
                    if (currentCursor)
                    {
                        XDefineCursor(display, window, currentCursor);
                    }
                    else
                    {
                        XUndefineCursor(display, window);
                    }
                }
                else
                {
                    XDefineCursor(display, window, emptyCursor);
                }
            }
        }

        CursorInterface* InputLinux::createCursorResource()
        {
            std::lock_guard<std::mutex> lock(resourceMutex);

            std::unique_ptr<CursorInterfaceLinux> cursorResource(new CursorInterfaceLinux());
            CursorInterface* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputLinux::setCursorVisible(bool visible)
        {
            if (visible != cursorVisible)
            {
                cursorVisible = visible;

                sharedEngine->executeOnMainThread([visible, this] {
                    WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                    Display* display = windowLinux->getDisplay();
                    ::Window window = windowLinux->getNativeWindow();

                    if (visible)
                    {
                        if (currentCursor)
                        {
                            XDefineCursor(display, window, currentCursor);
                        }
                        else
                        {
                            XUndefineCursor(display, window);
                        }
                    }
                    else
                    {
                        XDefineCursor(display, window, emptyCursor);
                    }

                    XFlush(display);
                });
            }
        }

        bool InputLinux::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputLinux::setCursorLocked(bool locked)
        {
            sharedEngine->executeOnMainThread([locked] {
                WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                Display* display = windowLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                if (locked)
                {
                    if (XGrabPointer(display, window, False,
                                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask,
                                     GrabModeAsync, GrabModeAsync,
                                     None, None, CurrentTime) != GrabSuccess)
                    {
                        Log(Log::Level::ERR) << "Failed to grab pointer";
                    }
                }
                else
                {
                    XUngrabPointer(display, CurrentTime);
                }

                XSync(display, False);
            });
            cursorLocked = locked;
        }

        bool InputLinux::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputLinux::setCursorPosition(const Vector2& position)
        {
            Input::setCursorPosition(position);

            sharedEngine->executeOnMainThread([position] {
                WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                Display* display = windowLinux->getDisplay();
                ::Window window = windowLinux->getNativeWindow();

                XWindowAttributes attributes;
                XGetWindowAttributes(display, window, &attributes);

                ouzel::Vector2 windowLocation = ouzel::sharedEngine->getWindow()->convertNormalizedToWindowLocation(position);

                XWarpPointer(display, None, window, 0, 0, 0, 0,
                             attributes.x + static_cast<int>(windowLocation.v[0]),
                             attributes.y + static_cast<int>(windowLocation.v[1]));
                XSync(display, False);
            });
        }
    } // namespace input
} // namespace ouzel
