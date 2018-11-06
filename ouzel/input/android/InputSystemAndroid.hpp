// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUTSYSTEMANDROID_HPP
#define OUZEL_INPUTSYSTEMANDROID_HPP

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
            explicit InputSystemAndroid(EventHandler& initEventHandler);
            ~InputSystemAndroid();

            void executeCommand(const Command& command) override;

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDevice* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            jboolean handleTouchEvent(jobject event);

        private:
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
        };
    }
}

#endif // OUZEL_INPUTSYSTEMANDROID_HPP
