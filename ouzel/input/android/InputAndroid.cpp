// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <map>
#include <android/keycodes.h>
#include <android/input.h>
#include "InputAndroid.hpp"
#include "core/android/EngineAndroid.hpp"
#include "utils/Log.hpp"

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

            {AKEYCODE_DPAD_UP, KeyboardKey::UP},
            {AKEYCODE_DPAD_DOWN, KeyboardKey::DOWN},
            {AKEYCODE_DPAD_LEFT, KeyboardKey::LEFT},
            {AKEYCODE_DPAD_RIGHT, KeyboardKey::RIGHT},

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
            {AKEYCODE_LEFT_BRACKET, KeyboardKey::LEFT_BRACKET},
            {AKEYCODE_BACKSLASH, KeyboardKey::BACKSLASH},
            {AKEYCODE_RIGHT_BRACKET, KeyboardKey::RIGHT_BRACKET},
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

            {AKEYCODE_SHIFT_LEFT, KeyboardKey::LEFT_SHIFT},
            {AKEYCODE_SHIFT_RIGHT, KeyboardKey::RIGHT_SHIFT},
            {AKEYCODE_CTRL_LEFT, KeyboardKey::LEFT_CONTROL},
            {AKEYCODE_CTRL_RIGHT, KeyboardKey::RIGHT_CONTROL},
            {AKEYCODE_ALT_LEFT, KeyboardKey::LEFT_ALT},
            {AKEYCODE_ALT_RIGHT, KeyboardKey::RIGHT_ALT},
            {AKEYCODE_META_LEFT, KeyboardKey::LEFT_SUPER},
            {AKEYCODE_META_RIGHT, KeyboardKey::RIGHT_SUPER}
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
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(sharedEngine);
            javaVM = engineAndroid->getJavaVM();
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            {
                Log(Log::Level::ERR) << "Failed to get JNI environment";
                return;
            }

            inputDeviceClass = jniEnv->FindClass("android/view/InputDevice");
            inputDeviceClass = static_cast<jclass>(jniEnv->NewGlobalRef(inputDeviceClass));
            getDeviceIdsMethod = jniEnv->GetStaticMethodID(inputDeviceClass, "getDeviceIds", "()[I");
        }

        InputAndroid::~InputAndroid()
        {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
            {
                Log(Log::Level::ERR) << "Failed to get JNI environment";
                return;
            }

            if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
        }
    } // namespace input
} // namespace ouzel
