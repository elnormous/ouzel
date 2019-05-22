// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMANDROID_HPP
#define OUZEL_INPUT_INPUTSYSTEMANDROID_HPP

#include <memory>
#include <jni.h>
#include "input/InputSystem.hpp"
#include "input/android/GamepadDeviceAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid final: public InputSystem
        {
        public:
            explicit InputSystemAndroid(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemAndroid();

            void executeCommand(const Command& command) final;

            inline KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }
            inline MouseDevice* getMouseDevice() const { return mouseDevice.get(); }
            inline TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            jboolean handleTouchEvent(jobject event);
            jboolean handleGenericMotionEvent(jobject event);

        private:
            bool updateButtonState(jint newButtonState, jint x, jint y);

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<MouseDevice> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            JavaVM* javaVM = nullptr;
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
}

#endif // OUZEL_INPUT_INPUTSYSTEMANDROID_HPP
