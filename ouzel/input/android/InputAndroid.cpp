// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <map>
#include <android/keycodes.h>
#include "InputAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::map<jint, KeyboardKey> keyMap = {
            {AKEYCODE_0, KeyboardKey::NUM_0},
            {AKEYCODE_1, KeyboardKey::NUM_1},
            {AKEYCODE_2, KeyboardKey::NUM_2},
            {AKEYCODE_3, KeyboardKey::NUM_3},
            {AKEYCODE_4, KeyboardKey::NUM_4},
            {AKEYCODE_5, KeyboardKey::NUM_5},
            {AKEYCODE_6, KeyboardKey::NUM_6},
            {AKEYCODE_7, KeyboardKey::NUM_7},
            {AKEYCODE_8, KeyboardKey::NUM_8},
            {AKEYCODE_9, KeyboardKey::NUM_9},

            {AKEYCODE_A, KeyboardKey::A},
            {AKEYCODE_B, KeyboardKey::B},
            {AKEYCODE_C, KeyboardKey::C},
            {AKEYCODE_D, KeyboardKey::D},
            {AKEYCODE_E, KeyboardKey::E},
            {AKEYCODE_F, KeyboardKey::F},
            {AKEYCODE_G, KeyboardKey::G},
            {AKEYCODE_H, KeyboardKey::H},
            {AKEYCODE_I, KeyboardKey::I},
            {AKEYCODE_J, KeyboardKey::J},
            {AKEYCODE_K, KeyboardKey::K},
            {AKEYCODE_L, KeyboardKey::L},
            {AKEYCODE_M, KeyboardKey::M},
            {AKEYCODE_N, KeyboardKey::N},
            {AKEYCODE_O, KeyboardKey::O},
            {AKEYCODE_P, KeyboardKey::P},
            {AKEYCODE_Q, KeyboardKey::Q},
            {AKEYCODE_R, KeyboardKey::R},
            {AKEYCODE_S, KeyboardKey::S},
            {AKEYCODE_T, KeyboardKey::T},
            {AKEYCODE_U, KeyboardKey::U},
            {AKEYCODE_V, KeyboardKey::V},
            {AKEYCODE_W, KeyboardKey::W},
            {AKEYCODE_X, KeyboardKey::X},
            {AKEYCODE_Y, KeyboardKey::Y},
            {AKEYCODE_Z, KeyboardKey::Z},

            {AKEYCODE_SEMICOLON, KeyboardKey::SEMICOLON},
            {AKEYCODE_EQUALS, KeyboardKey::PLUS},
            {AKEYCODE_COMMA, KeyboardKey::COMMA},
            {AKEYCODE_PERIOD, KeyboardKey::PERIOD},
            {AKEYCODE_SLASH, KeyboardKey::SLASH},
            {AKEYCODE_GRAVE, KeyboardKey::GRAVE},
            {AKEYCODE_LEFT_BRACKET, KeyboardKey::BRACKET_LEFT},
            {AKEYCODE_BACKSLASH, KeyboardKey::BACKSLASH},
            {AKEYCODE_RIGHT_BRACKET, KeyboardKey::BRACKET_RIGHT},
            {AKEYCODE_APOSTROPHE, KeyboardKey::QUOTE},
            {AKEYCODE_SPACE, KeyboardKey::SPACE},

            {AKEYCODE_DEL, KeyboardKey::BACKSPACE},
            {AKEYCODE_TAB, KeyboardKey::TAB},
            {AKEYCODE_ENTER, KeyboardKey::RETURN},
            {AKEYCODE_BREAK, KeyboardKey::PAUSE},

            {AKEYCODE_BACK, KeyboardKey::ESCAPE},
            {AKEYCODE_PAGE_UP, KeyboardKey::PRIOR},
            {AKEYCODE_PAGE_DOWN, KeyboardKey::NEXT},

            {AKEYCODE_FORWARD_DEL, KeyboardKey::DEL},

            {AKEYCODE_STAR, KeyboardKey::MULTIPLY},
            {AKEYCODE_PLUS, KeyboardKey::PLUS},
            {AKEYCODE_MINUS, KeyboardKey::MINUS},

            {AKEYCODE_F1, KeyboardKey::F1},
            {AKEYCODE_F2, KeyboardKey::F2},
            {AKEYCODE_F3, KeyboardKey::F3},
            {AKEYCODE_F4, KeyboardKey::F4},
            {AKEYCODE_F5, KeyboardKey::F5},
            {AKEYCODE_F6, KeyboardKey::F6},
            {AKEYCODE_F7, KeyboardKey::F7},
            {AKEYCODE_F8, KeyboardKey::F8},
            {AKEYCODE_F9, KeyboardKey::F9},
            {AKEYCODE_F10, KeyboardKey::F10},
            {AKEYCODE_F11, KeyboardKey::F11},
            {AKEYCODE_F12, KeyboardKey::F12},

            {AKEYCODE_SHIFT_LEFT, KeyboardKey::LSHIFT},
            {AKEYCODE_SHIFT_RIGHT, KeyboardKey::RSHIFT},
            {AKEYCODE_CTRL_LEFT, KeyboardKey::LCONTROL},
            {AKEYCODE_CTRL_RIGHT, KeyboardKey::RCONTROL},
            {AKEYCODE_ALT_LEFT, KeyboardKey::LALT},
            {AKEYCODE_ALT_RIGHT, KeyboardKey::RALT},
            {AKEYCODE_META_LEFT, KeyboardKey::LSUPER},
            {AKEYCODE_META_RIGHT, KeyboardKey::RSUPER}
        };

        KeyboardKey InputAndroid::convertKeyCode(jint keyCode)
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

        InputAndroid::InputAndroid()
        {
        }

        InputAndroid::~InputAndroid()
        {
        }
    } // namespace input
} // namespace ouzel
