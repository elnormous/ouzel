// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMANDROID_HPP
#define OUZEL_INPUT_INPUTSYSTEMANDROID_HPP

#include <memory>
#include <jni.h>
#include "../InputSystem.hpp"
#include "GamepadDeviceAndroid.hpp"

namespace ouzel::input::android
{
    class InputSystem final: public input::InputSystem
    {
    public:
        explicit InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback);
        ~InputSystem() override;

        auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
        auto getMouseDevice() const noexcept { return mouseDevice.get(); }
        auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

        jboolean handleTouchEvent(jobject event);
        jboolean handleGenericMotionEvent(jobject event);

    private:
        void executeCommand(const Command& command) final;

        auto getNextDeviceId() noexcept
        {
            ++lastDeviceId.value;
            return lastDeviceId;
        }

        bool updateButtonState(jint newButtonState, jint x, jint y);

        DeviceId lastDeviceId;
        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<MouseDevice> mouseDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;

        JavaVM* javaVm = nullptr;
        jclass inputDeviceClass = nullptr;
        jmethodID getDeviceIdsMethod = nullptr;
        jmethodID getActionMethod = nullptr;
        jmethodID getPointerIdMethod = nullptr;
        jmethodID getToolTypeMethod = nullptr;
        jmethodID getXMethod = nullptr;
        jmethodID getYMethod = nullptr;
        jmethodID getPressureMethod = nullptr;
        jmethodID getAxisValueMethod = nullptr;
        jmethodID getButtonStateMethod = nullptr;
        jint buttonState = 0;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEMANDROID_HPP
