// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) == JNI_OK)
            {
                if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
            }
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
                        //mouseButtonPress(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        //touchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseButtonPress(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, pointerIndex);
                        //touchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        //touchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        //touchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        //touchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        //touchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        //mouseButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        //touchCancel(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
            }

            return false;
        }
    } // namespace input
} // namespace ouzel
