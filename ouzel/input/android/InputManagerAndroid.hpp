// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <jni.h>
#include "input/InputManager.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputManagerAndroid: public InputManager
        {
            friend Engine;
        public:
            virtual ~InputManagerAndroid();

            static KeyboardKey convertKeyCode(jint keyCode);
            jboolean handleTouchEvent(jobject event);

        private:
            InputManagerAndroid();

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
    } // namespace input
} // namespace ouzel
