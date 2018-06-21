// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <unordered_map>
#include <android/keycodes.h>
#include <android/input.h>
#include "InputManagerAndroid.hpp"
#include "core/android/EngineAndroid.hpp"
#include "core/Window.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<jint, KeyboardKey> keyMap = {
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

        KeyboardKey InputManagerAndroid::convertKeyCode(jint keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        InputManagerAndroid::InputManagerAndroid()
        {
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
            javaVM = engineAndroid->getJavaVM();
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw SystemError("Failed to get JNI environment");

            inputDeviceClass = jniEnv->FindClass("android/view/InputDevice");
            inputDeviceClass = static_cast<jclass>(jniEnv->NewGlobalRef(inputDeviceClass));
            getDeviceIdsMethod = jniEnv->GetStaticMethodID(inputDeviceClass, "getDeviceIds", "()[I");

            jclass motionEventClass = jniEnv->FindClass("android/view/MotionEvent");
            getActionMethod = jniEnv->GetMethodID(motionEventClass, "getAction", "()I");
            getPointerIdMethod = jniEnv->GetMethodID(motionEventClass, "getPointerId", "(I)I");
            getToolTypeMethod = jniEnv->GetMethodID(motionEventClass, "getToolType", "(I)I");
            getXMethod = jniEnv->GetMethodID(motionEventClass, "getX", "(I)F");
            getYMethod = jniEnv->GetMethodID(motionEventClass, "getY", "(I)F");
            getPressureMethod = jniEnv->GetMethodID(motionEventClass, "getPressure", "(I)F");
        }

        InputManagerAndroid::~InputManagerAndroid()
        {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw SystemError("Failed to get JNI environment");

            if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
        }

        jboolean InputManagerAndroid::handleTouchEvent(jobject event)
        {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) != JNI_OK)
                throw SystemError("Failed to get JNI environment");

            jint action = jniEnv->CallIntMethod(event, getActionMethod);

            switch (action & AMOTION_EVENT_ACTION_MASK)
            {
                case AMOTION_EVENT_ACTION_DOWN: // first touch
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseButtonPress(MouseButton::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_POINTER_DOWN: // touches beyond the first
                {
                    jint pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, pointerIndex);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, pointerIndex);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, pointerIndex);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseButtonPress(MouseButton::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, pointerIndex);
                        touchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_UP: // first touch
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseButtonRelease(MouseButton::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_POINTER_UP: // touches beyond the first
                {
                    jint pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, pointerIndex);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, pointerIndex);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, pointerIndex);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseButtonRelease(MouseButton::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_CANCEL:
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);
                    jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseButtonRelease(MouseButton::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        touchCancel(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
            }

            return false;
        }
    } // namespace input
} // namespace ouzel
