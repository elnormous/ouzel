// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include <android/input.h>
#include "InputSystemAndroid.hpp"
#include "GamepadDeviceAndroid.hpp"
#include "core/android/EngineAndroid.hpp"
#include "core/Window.hpp"

namespace ouzel
{
    namespace input
    {
        InputSystemAndroid::InputSystemAndroid(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(std::make_unique<KeyboardDevice>(*this, ++lastDeviceId)),
            mouseDevice(std::make_unique<MouseDevice>(*this, ++lastDeviceId)),
            touchpadDevice(std::make_unique<TouchpadDevice>(*this, ++lastDeviceId, true))
        {
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);
            javaVM = engineAndroid->getJavaVM();
            JNIEnv* jniEnv;

            jint result;
            if ((result = javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6)) != JNI_OK)
                throw std::system_error(result, jniErrorCategory, "Failed to get JNI environment");

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
            getAxisValueMethod = jniEnv->GetMethodID(motionEventClass, "getAxisValue", "(I)F");
            getButtonStateMethod = jniEnv->GetMethodID(motionEventClass, "getButtonState", "()I");
        }

        InputSystemAndroid::~InputSystemAndroid()
        {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) == JNI_OK)
                if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
        }

        void InputSystemAndroid::executeCommand(const Command& command)
        {
            switch (command.type)
            {
            case Command::Type::START_DEVICE_DISCOVERY:
                break;
            case Command::Type::STOP_DEVICE_DISCOVERY:
                break;
            case Command::Type::SET_PLAYER_INDEX:
            {
                break;
            }
            case Command::Type::SET_VIBRATION:
            {
                break;
            }
            case Command::Type::SHOW_VIRTUAL_KEYBOARD:
                break;
            case Command::Type::HIDE_VIRTUAL_KEYBOARD:
                break;
            default:
                break;
            }
        }

        jboolean InputSystemAndroid::handleTouchEvent(jobject event)
        {
            JNIEnv* jniEnv;

            jint result;
            if ((result = javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6)) != JNI_OK)
                throw std::system_error(result, jniErrorCategory, "Failed to get JNI environment");

            jint action = jniEnv->CallIntMethod(event, getActionMethod);

            switch (action & AMOTION_EVENT_ACTION_MASK)
            {
                case AMOTION_EVENT_ACTION_DOWN: // first touch
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                        return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                        touchpadDevice->handleTouchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
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

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                        return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, pointerIndex);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);
                        touchpadDevice->handleTouchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_MOVE:
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        if (!updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y))
                            mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                        touchpadDevice->handleTouchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_UP: // first touch
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                        return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                        touchpadDevice->handleTouchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
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

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                        return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);
                        touchpadDevice->handleTouchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
                        return true;
                    }

                    break;
                }
                case AMOTION_EVENT_ACTION_CANCEL:
                {
                    jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                    jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                    jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        if (!updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y))
                            mouseDevice->handleButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                        touchpadDevice->handleTouchCancel(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)), pressure);
                        return true;
                    }

                    break;
                }
            }

            return false;
        }

        jboolean InputSystemAndroid::handleGenericMotionEvent(jobject event)
        {
            JNIEnv* jniEnv;

            jint result;
            if ((result = javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6)) != JNI_OK)
                throw std::system_error(result, jniErrorCategory, "Failed to get JNI environment");

            jint action = jniEnv->CallIntMethod(event, getActionMethod);
            jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
            jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
            jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

            switch (action & AMOTION_EVENT_ACTION_MASK)
            {
                case AMOTION_EVENT_ACTION_HOVER_MOVE:
                {
                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));
                        return true;
                    }
                    break;
                }
                case AMOTION_EVENT_ACTION_SCROLL:
                {
                    jfloat scrollX = jniEnv->CallFloatMethod(event, getAxisValueMethod, AMOTION_EVENT_AXIS_HSCROLL);
                    jfloat scrollY = jniEnv->CallFloatMethod(event, getAxisValueMethod, AMOTION_EVENT_AXIS_VSCROLL);

                    if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    {
                        mouseDevice->handleScroll(Vector2F(scrollX, scrollY),
                                                  engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));
                        return true;
                    }
                    break;
                }
            }

            return false;
        }

        bool InputSystemAndroid::updateButtonState(jint newButtonState, jint x, jint y)
        {
            bool result = false;

            static const std::vector<std::pair<jint, Mouse::Button>> buttons = {
                {AMOTION_EVENT_BUTTON_PRIMARY, Mouse::Button::LEFT},
                {AMOTION_EVENT_BUTTON_SECONDARY, Mouse::Button::RIGHT},
                {AMOTION_EVENT_BUTTON_TERTIARY, Mouse::Button::MIDDLE},
                {AMOTION_EVENT_BUTTON_BACK, Mouse::Button::X1},
                {AMOTION_EVENT_BUTTON_FORWARD, Mouse::Button::X2}
            };

            for (const auto& button : buttons)
            {
                if ((newButtonState & button.first) != (buttonState & button.first))
                {
                    if (newButtonState & button.first)
                        mouseDevice->handleButtonPress(button.second, engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));
                    else
                        mouseDevice->handleButtonRelease(button.second, engine->getWindow()->convertWindowToNormalizedLocation(Vector2F(x, y)));

                    result = true;
                }
            }

            buttonState = newButtonState;
            return result;
        }
    } // namespace input
} // namespace ouzel
