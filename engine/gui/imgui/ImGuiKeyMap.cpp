#include "ImGuiKeyMap.hpp"

namespace fluxion::gui::imgui
{
    ImGuiKey toImGuiKey(input::Keyboard::Key key) noexcept
    {
        using K = input::Keyboard::Key;
        switch (key)
        {
            case K::tab:           return ImGuiKey_Tab;
            case K::left:          return ImGuiKey_LeftArrow;
            case K::right:         return ImGuiKey_RightArrow;
            case K::up:            return ImGuiKey_UpArrow;
            case K::down:          return ImGuiKey_DownArrow;
            case K::pageUp:        return ImGuiKey_PageUp;
            case K::pageDown:      return ImGuiKey_PageDown;
            case K::home:          return ImGuiKey_Home;
            case K::end:           return ImGuiKey_End;
            case K::insert:        return ImGuiKey_Insert;
            case K::del:           return ImGuiKey_Delete;
            case K::backspace:     return ImGuiKey_Backspace;
            case K::space:         return ImGuiKey_Space;
            case K::enter:         return ImGuiKey_Enter;
            case K::escape:        return ImGuiKey_Escape;
            case K::quote:         return ImGuiKey_Apostrophe;
            case K::comma:         return ImGuiKey_Comma;
            case K::minus:         return ImGuiKey_Minus;
            case K::period:        return ImGuiKey_Period;
            case K::slash:         return ImGuiKey_Slash;
            case K::semicolon:     return ImGuiKey_Semicolon;
            case K::equal:         return ImGuiKey_Equal;
            case K::leftBracket:   return ImGuiKey_LeftBracket;
            case K::backslash:     return ImGuiKey_Backslash;
            case K::rightBracket:  return ImGuiKey_RightBracket;
            case K::grave:         return ImGuiKey_GraveAccent;
            case K::capsLock:      return ImGuiKey_CapsLock;
            case K::scrollLock:    return ImGuiKey_ScrollLock;
            case K::numLock:       return ImGuiKey_NumLock;
            case K::printScreen:   return ImGuiKey_PrintScreen;
            case K::pause:         return ImGuiKey_Pause;
            case K::leftControl:   return ImGuiKey_LeftCtrl;
            case K::leftShift:     return ImGuiKey_LeftShift;
            case K::leftAlt:       return ImGuiKey_LeftAlt;
            case K::leftSuper:     return ImGuiKey_LeftSuper;
            case K::rightControl:  return ImGuiKey_RightCtrl;
            case K::rightShift:    return ImGuiKey_RightShift;
            case K::rightAlt:      return ImGuiKey_RightAlt;
            case K::rightSuper:    return ImGuiKey_RightSuper;
            case K::menu:          return ImGuiKey_Menu;

            case K::num0: return ImGuiKey_0;
            case K::num1: return ImGuiKey_1;
            case K::num2: return ImGuiKey_2;
            case K::num3: return ImGuiKey_3;
            case K::num4: return ImGuiKey_4;
            case K::num5: return ImGuiKey_5;
            case K::num6: return ImGuiKey_6;
            case K::num7: return ImGuiKey_7;
            case K::num8: return ImGuiKey_8;
            case K::num9: return ImGuiKey_9;

            case K::a: return ImGuiKey_A;
            case K::b: return ImGuiKey_B;
            case K::c: return ImGuiKey_C;
            case K::d: return ImGuiKey_D;
            case K::e: return ImGuiKey_E;
            case K::f: return ImGuiKey_F;
            case K::g: return ImGuiKey_G;
            case K::h: return ImGuiKey_H;
            case K::i: return ImGuiKey_I;
            case K::j: return ImGuiKey_J;
            case K::k: return ImGuiKey_K;
            case K::l: return ImGuiKey_L;
            case K::m: return ImGuiKey_M;
            case K::n: return ImGuiKey_N;
            case K::o: return ImGuiKey_O;
            case K::p: return ImGuiKey_P;
            case K::q: return ImGuiKey_Q;
            case K::r: return ImGuiKey_R;
            case K::s: return ImGuiKey_S;
            case K::t: return ImGuiKey_T;
            case K::u: return ImGuiKey_U;
            case K::v: return ImGuiKey_V;
            case K::w: return ImGuiKey_W;
            case K::x: return ImGuiKey_X;
            case K::y: return ImGuiKey_Y;
            case K::z: return ImGuiKey_Z;

            case K::f1:  return ImGuiKey_F1;
            case K::f2:  return ImGuiKey_F2;
            case K::f3:  return ImGuiKey_F3;
            case K::f4:  return ImGuiKey_F4;
            case K::f5:  return ImGuiKey_F5;
            case K::f6:  return ImGuiKey_F6;
            case K::f7:  return ImGuiKey_F7;
            case K::f8:  return ImGuiKey_F8;
            case K::f9:  return ImGuiKey_F9;
            case K::f10: return ImGuiKey_F10;
            case K::f11: return ImGuiKey_F11;
            case K::f12: return ImGuiKey_F12;

            case K::numpad0:        return ImGuiKey_Keypad0;
            case K::numpad1:        return ImGuiKey_Keypad1;
            case K::numpad2:        return ImGuiKey_Keypad2;
            case K::numpad3:        return ImGuiKey_Keypad3;
            case K::numpad4:        return ImGuiKey_Keypad4;
            case K::numpad5:        return ImGuiKey_Keypad5;
            case K::numpad6:        return ImGuiKey_Keypad6;
            case K::numpad7:        return ImGuiKey_Keypad7;
            case K::numpad8:        return ImGuiKey_Keypad8;
            case K::numpad9:        return ImGuiKey_Keypad9;
            case K::numpadDecimal:  return ImGuiKey_KeypadDecimal;
            case K::numpadDivide:   return ImGuiKey_KeypadDivide;
            case K::numpadMultiply: return ImGuiKey_KeypadMultiply;
            case K::numpadMinus:    return ImGuiKey_KeypadSubtract;
            case K::numpadPlus:     return ImGuiKey_KeypadAdd;
            case K::numpadEnter:    return ImGuiKey_KeypadEnter;
            case K::numpadEqual:    return ImGuiKey_KeypadEqual;

            default: return ImGuiKey_None;
        }
    }

    int toImGuiMouseButton(input::Mouse::Button button) noexcept
    {
        switch (button)
        {
            case input::Mouse::Button::left:   return 0;
            case input::Mouse::Button::right:  return 1;
            case input::Mouse::Button::middle: return 2;
            case input::Mouse::Button::x1:     return 3;
            case input::Mouse::Button::x2:     return 4;
            default: return -1;
        }
    }
}
