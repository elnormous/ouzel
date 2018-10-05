// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <android/keycodes.h>
#include <android/input.h>
#include "InputSystemAndroid.hpp"
#include "GamepadDeviceAndroid.hpp"
#include "core/android/EngineAndroid.hpp"
#include "core/Window.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<jint, Keyboard::Key> keyMap = {
            {AKEYCODE_0, Keyboard::Key::NUM_0},
            {AKEYCODE_1, Keyboard::Key::NUM_1},
            {AKEYCODE_2, Keyboard::Key::NUM_2},
            {AKEYCODE_3, Keyboard::Key::NUM_3},
            {AKEYCODE_4, Keyboard::Key::NUM_4},
            {AKEYCODE_5, Keyboard::Key::NUM_5},
            {AKEYCODE_6, Keyboard::Key::NUM_6},
            {AKEYCODE_7, Keyboard::Key::NUM_7},
            {AKEYCODE_8, Keyboard::Key::NUM_8},
            {AKEYCODE_9, Keyboard::Key::NUM_9},

            {AKEYCODE_DPAD_UP, Keyboard::Key::UP},
            {AKEYCODE_DPAD_DOWN, Keyboard::Key::DOWN},
            {AKEYCODE_DPAD_LEFT, Keyboard::Key::LEFT},
            {AKEYCODE_DPAD_RIGHT, Keyboard::Key::RIGHT},

            {AKEYCODE_A, Keyboard::Key::A},
            {AKEYCODE_B, Keyboard::Key::B},
            {AKEYCODE_C, Keyboard::Key::C},
            {AKEYCODE_D, Keyboard::Key::D},
            {AKEYCODE_E, Keyboard::Key::E},
            {AKEYCODE_F, Keyboard::Key::F},
            {AKEYCODE_G, Keyboard::Key::G},
            {AKEYCODE_H, Keyboard::Key::H},
            {AKEYCODE_I, Keyboard::Key::I},
            {AKEYCODE_J, Keyboard::Key::J},
            {AKEYCODE_K, Keyboard::Key::K},
            {AKEYCODE_L, Keyboard::Key::L},
            {AKEYCODE_M, Keyboard::Key::M},
            {AKEYCODE_N, Keyboard::Key::N},
            {AKEYCODE_O, Keyboard::Key::O},
            {AKEYCODE_P, Keyboard::Key::P},
            {AKEYCODE_Q, Keyboard::Key::Q},
            {AKEYCODE_R, Keyboard::Key::R},
            {AKEYCODE_S, Keyboard::Key::S},
            {AKEYCODE_T, Keyboard::Key::T},
            {AKEYCODE_U, Keyboard::Key::U},
            {AKEYCODE_V, Keyboard::Key::V},
            {AKEYCODE_W, Keyboard::Key::W},
            {AKEYCODE_X, Keyboard::Key::X},
            {AKEYCODE_Y, Keyboard::Key::Y},
            {AKEYCODE_Z, Keyboard::Key::Z},

            {AKEYCODE_SEMICOLON, Keyboard::Key::SEMICOLON},
            {AKEYCODE_EQUALS, Keyboard::Key::PLUS},
            {AKEYCODE_COMMA, Keyboard::Key::COMMA},
            {AKEYCODE_PERIOD, Keyboard::Key::PERIOD},
            {AKEYCODE_SLASH, Keyboard::Key::SLASH},
            {AKEYCODE_GRAVE, Keyboard::Key::GRAVE},
            {AKEYCODE_LEFT_BRACKET, Keyboard::Key::LEFT_BRACKET},
            {AKEYCODE_BACKSLASH, Keyboard::Key::BACKSLASH},
            {AKEYCODE_RIGHT_BRACKET, Keyboard::Key::RIGHT_BRACKET},
            {AKEYCODE_APOSTROPHE, Keyboard::Key::QUOTE},
            {AKEYCODE_SPACE, Keyboard::Key::SPACE},

            {AKEYCODE_DEL, Keyboard::Key::BACKSPACE},
            {AKEYCODE_TAB, Keyboard::Key::TAB},
            {AKEYCODE_ENTER, Keyboard::Key::RETURN},
            {AKEYCODE_BREAK, Keyboard::Key::PAUSE},

            {AKEYCODE_BACK, Keyboard::Key::ESCAPE},
            {AKEYCODE_PAGE_UP, Keyboard::Key::PRIOR},
            {AKEYCODE_PAGE_DOWN, Keyboard::Key::NEXT},

            {AKEYCODE_FORWARD_DEL, Keyboard::Key::DEL},

            {AKEYCODE_STAR, Keyboard::Key::MULTIPLY},
            {AKEYCODE_PLUS, Keyboard::Key::PLUS},
            {AKEYCODE_MINUS, Keyboard::Key::MINUS},

            {AKEYCODE_F1, Keyboard::Key::F1},
            {AKEYCODE_F2, Keyboard::Key::F2},
            {AKEYCODE_F3, Keyboard::Key::F3},
            {AKEYCODE_F4, Keyboard::Key::F4},
            {AKEYCODE_F5, Keyboard::Key::F5},
            {AKEYCODE_F6, Keyboard::Key::F6},
            {AKEYCODE_F7, Keyboard::Key::F7},
            {AKEYCODE_F8, Keyboard::Key::F8},
            {AKEYCODE_F9, Keyboard::Key::F9},
            {AKEYCODE_F10, Keyboard::Key::F10},
            {AKEYCODE_F11, Keyboard::Key::F11},
            {AKEYCODE_F12, Keyboard::Key::F12},

            {AKEYCODE_SHIFT_LEFT, Keyboard::Key::LEFT_SHIFT},
            {AKEYCODE_SHIFT_RIGHT, Keyboard::Key::RIGHT_SHIFT},
            {AKEYCODE_CTRL_LEFT, Keyboard::Key::LEFT_CONTROL},
            {AKEYCODE_CTRL_RIGHT, Keyboard::Key::RIGHT_CONTROL},
            {AKEYCODE_ALT_LEFT, Keyboard::Key::LEFT_ALT},
            {AKEYCODE_ALT_RIGHT, Keyboard::Key::RIGHT_ALT},
            {AKEYCODE_META_LEFT, Keyboard::Key::LEFT_SUPER},
            {AKEYCODE_META_RIGHT, Keyboard::Key::RIGHT_SUPER}
        };

        Keyboard::Key InputSystemAndroid::convertKeyCode(jint keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        InputSystemAndroid::InputSystemAndroid()
        {
            Event keyboardConnectEvent;
            keyboardConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<KeyboardDevice> keyboard(new KeyboardDevice(*this, ++lastDeviceId));
            keyboardConnectEvent.deviceId = keyboard->getId();
            keyboardConnectEvent.deviceType = Controller::Type::KEYBOARD;
            keyboardDevice = keyboard.get();
            inputDevices.insert(std::make_pair(keyboard->getId(), std::move(keyboard)));
            addEvent(keyboardConnectEvent);

            Event mouseConnectEvent;
            mouseConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<MouseDeviceAndroid> mouse(new MouseDeviceAndroid(*this, ++lastDeviceId));
            mouseConnectEvent.deviceId = mouse->getId();
            mouseConnectEvent.deviceType = Controller::Type::MOUSE;
            mouseDevice = mouse.get();
            inputDevices.insert(std::make_pair(mouse->getId(), std::move(mouse)));
            addEvent(mouseConnectEvent);

            Event touchpadConnectEvent;
            touchpadConnectEvent.type = Event::Type::DEVICE_CONNECT;
            std::unique_ptr<TouchpadDevice> touchpad(new TouchpadDevice(*this, ++lastDeviceId));
            touchpadConnectEvent.deviceId = touchpad->getId();
            touchpadConnectEvent.deviceType = Controller::Type::TOUCHPAD;
            touchpadDevice = touchpad.get();
            inputDevices.insert(std::make_pair(touchpad->getId(), std::move(touchpad)));
            addEvent(touchpadConnectEvent);

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

        InputSystemAndroid::~InputSystemAndroid()
        {
            JNIEnv* jniEnv;

            if (javaVM->GetEnv(reinterpret_cast<void**>(&jniEnv), JNI_VERSION_1_6) == JNI_OK)
            {
                if (inputDeviceClass) jniEnv->DeleteGlobalRef(inputDeviceClass);
            }
        }

        void InputSystemAndroid::executeCommand(Command command)
        {
            switch (command.type)
            {
            case Command::Type::START_DEVICE_DISCOVERY:
                break;
            case Command::Type::STOP_DEVICE_DISCOVERY:
                break;
            case Command::Type::SET_ABSOLUTE_DPAD_VALUES:
            {
                break;
            }
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
                        mouseDevice->handleButtonPress(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchpadDevice->handleTouchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleButtonPress(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, pointerIndex);
                        touchpadDevice->handleTouchBegin(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchpadDevice->handleTouchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                        toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchpadDevice->handleTouchMove(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchpadDevice->handleTouchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        jint pointerId = jniEnv->CallIntMethod(event, getPointerIdMethod, 0);
                        touchpadDevice->handleTouchEnd(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
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
                        mouseDevice->handleButtonRelease(Mouse::Button::LEFT, engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), 0);
                        return true;
                    }
                    else if (toolType == AMOTION_EVENT_TOOL_TYPE_FINGER ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_STYLUS ||
                             toolType == AMOTION_EVENT_TOOL_TYPE_ERASER)
                    {
                        touchpadDevice->handleTouchCancel(static_cast<uint64_t>(pointerId), engine->getWindow()->convertWindowToNormalizedLocation(ouzel::Vector2(x, y)), pressure);
                        return true;
                    }

                    break;
                }
            }

            return false;
        }
    } // namespace input
} // namespace ouzel
