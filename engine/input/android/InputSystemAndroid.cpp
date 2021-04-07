// Ouzel by Elviss Strazdins

#include <array>
#include <stdexcept>
#include <android/input.h>
#include "InputSystemAndroid.hpp"
#include "GamepadDeviceAndroid.hpp"
#include "../../core/android/EngineAndroid.hpp"
#include "../../core/Window.hpp"

namespace ouzel::input::android
{
    InputSystem::InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback):
        input::InputSystem(initCallback),
        keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
        mouseDevice(std::make_unique<MouseDevice>(*this, getNextDeviceId())),
        touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
    {
        const auto engineAndroid = static_cast<core::android::Engine*>(engine);
        javaVm = engineAndroid->getJavaVm();
        void* jniEnvPointer;

        if (const auto result = javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6); result != JNI_OK)
            throw std::system_error(result, core::android::getErrorCategory(), "Failed to get JNI environment");

        const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);

        inputDeviceClass = static_cast<jclass>(jniEnv->NewGlobalRef(jniEnv->FindClass("android/view/InputDevice")));
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

    InputSystem::~InputSystem()
    {
        void* jniEnvPointer;

        if (javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6) == JNI_OK)
        {
            const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);
            if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
        }
    }

    void InputSystem::executeCommand(const Command& command)
    {
        switch (command.type)
        {
        case Command::Type::startDeviceDiscovery:
            break;
        case Command::Type::stopDeviceDiscovery:
            break;
        case Command::Type::setPlayerIndex:
            break;
        case Command::Type::setVibration:
            break;
        case Command::Type::showVirtualKeyboard:
            break;
        case Command::Type::hideVirtualKeyboard:
            break;
        default:
            break;
        }
    }

    jboolean InputSystem::handleTouchEvent(jobject event)
    {
        void* jniEnvPointer;

        if (const auto result = javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6); result != JNI_OK)
            throw std::system_error(result, core::android::getErrorCategory(), "Failed to get JNI environment");

        const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);

        const jint action = jniEnv->CallIntMethod(event, getActionMethod);

        switch (action & AMOTION_EVENT_ACTION_MASK)
        {
            case AMOTION_EVENT_ACTION_DOWN: // first touch
            {
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchBegin(static_cast<std::uint64_t>(pointerId),
                                                     engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     pressure);
                    return true;
                }

                break;
            }
            case AMOTION_EVENT_ACTION_POINTER_DOWN: // touches beyond the first
            {
                const jint pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, pointerIndex);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, pointerIndex);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, pointerIndex);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, pointerIndex);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchBegin(static_cast<std::uint64_t>(pointerId),
                                                     engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     pressure);
                    return true;
                }

                break;
            }
            case AMOTION_EVENT_ACTION_MOVE:
            {
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                {
                    if (!updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y))
                    {
                        const Vector<float, 2> position{
                            static_cast<float>(x),
                            static_cast<float>(y)
                        };

                        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
                    }
                    return true;
                }
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                    toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                    toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchMove(static_cast<std::uint64_t>(pointerId),
                                                    engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                    pressure);
                    return true;
                }

                break;
            }
            case AMOTION_EVENT_ACTION_UP: // first touch
            {
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchEnd(static_cast<std::uint64_t>(pointerId),
                                                   engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                   pressure);
                    return true;
                }

                break;
            }
            case AMOTION_EVENT_ACTION_POINTER_UP: // touches beyond the first
            {
                const jint pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, pointerIndex);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, pointerIndex);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, pointerIndex);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                    return updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y);
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, pointerIndex);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchEnd(static_cast<std::uint64_t>(pointerId),
                                                   engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                   pressure);
                    return true;
                }

                break;
            }
            case AMOTION_EVENT_ACTION_CANCEL:
            {
                const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
                const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
                const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                {
                    if (!updateButtonState(jniEnv->CallIntMethod(event, getButtonStateMethod), x, y))
                    {
                        const Vector<float, 2> position{
                            static_cast<float>(x),
                            static_cast<float>(y)
                        };

                        mouseDevice->handleButtonRelease(Mouse::Button::left, engine->getWindow()->convertWindowToNormalizedLocation(position));
                    }
                    return true;
                }
                else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                            toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                {
                    const jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                    const jfloat pressure = jniEnv->CallFloatMethod(event, getPressureMethod, 0);
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    touchpadDevice->handleTouchCancel(static_cast<std::uint64_t>(pointerId),
                                                      engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                      pressure);
                    return true;
                }

                break;
            }
        }

        return false;
    }

    jboolean InputSystem::handleGenericMotionEvent(jobject event)
    {
        void* jniEnvPointer;

        if (const auto result = javaVm->GetEnv(&jniEnvPointer, JNI_VERSION_1_6); result != JNI_OK)
            throw std::system_error(result, core::android::getErrorCategory(), "Failed to get JNI environment");

        const auto jniEnv = static_cast<JNIEnv*>(jniEnvPointer);

        const jint action = jniEnv->CallIntMethod(event, getActionMethod);
        const jint toolType = jniEnv->CallIntMethod(event, getToolTypeMethod, 0);
        const jfloat x = jniEnv->CallFloatMethod(event, getXMethod, 0);
        const jfloat y = jniEnv->CallFloatMethod(event, getYMethod, 0);

        switch (action & AMOTION_EVENT_ACTION_MASK)
        {
            case AMOTION_EVENT_ACTION_HOVER_MOVE:
            {
                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                {
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
                    return true;
                }
                break;
            }
            case AMOTION_EVENT_ACTION_SCROLL:
            {
                const jfloat scrollX = jniEnv->CallFloatMethod(event, getAxisValueMethod, AMOTION_EVENT_AXIS_HSCROLL);
                const jfloat scrollY = jniEnv->CallFloatMethod(event, getAxisValueMethod, AMOTION_EVENT_AXIS_VSCROLL);

                if (toolType == AMOTION_EVENT_TOOL_TYPE_MOUSE)
                {
                    const Vector<float, 2> position{
                        static_cast<float>(x),
                        static_cast<float>(y)
                    };

                    mouseDevice->handleScroll(Vector<float, 2>{scrollX, scrollY},
                                              engine->getWindow()->convertWindowToNormalizedLocation(position));
                    return true;
                }
                break;
            }
        }

        return false;
    }

    bool InputSystem::updateButtonState(jint newButtonState, jint x, jint y)
    {
        bool result = false;

        constexpr std::array<std::pair<jint, Mouse::Button>, 5> buttons = {{
            {AMOTION_EVENT_BUTTON_PRIMARY, Mouse::Button::left},
            {AMOTION_EVENT_BUTTON_SECONDARY, Mouse::Button::right},
            {AMOTION_EVENT_BUTTON_TERTIARY, Mouse::Button::middle},
            {AMOTION_EVENT_BUTTON_BACK, Mouse::Button::x1},
            {AMOTION_EVENT_BUTTON_FORWARD, Mouse::Button::x2}
        }};

        for (const auto& button : buttons)
        {
            if ((newButtonState & button.first) != (buttonState & button.first))
            {
                const Vector<float, 2> position{
                    static_cast<float>(x),
                    static_cast<float>(y)
                };

                if (newButtonState & button.first)
                    mouseDevice->handleButtonPress(button.second, engine->getWindow()->convertWindowToNormalizedLocation(position));
                else
                    mouseDevice->handleButtonRelease(button.second, engine->getWindow()->convertWindowToNormalizedLocation(position));

                result = true;
            }
        }

        buttonState = newButtonState;
        return result;
    }
}
