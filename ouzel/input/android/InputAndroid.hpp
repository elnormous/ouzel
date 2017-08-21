// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <jni.h>
#include "input/Input.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputAndroid: public Input
        {
            friend Engine;
        public:
            virtual ~InputAndroid();

            static KeyboardKey convertKeyCode(jint keyCode);
            jboolean handleTouchEvent(jobject event);

        protected:
            InputAndroid();

            JavaVM* javaVM = nullptr;
            jclass inputDeviceClass = nullptr;
            jmethodID getDeviceIdsMethod = nullptr;
            jmethodID getActionMethod = nullptr;
            jmethodID getPointerIdMethod = nullptr;
            jmethodID getToolTypeMethod = nullptr;
            jmethodID getXMethod = nullptr;
            jmethodID getYMethod = nullptr;
        };
    } // namespace input
} // namespace ouzel
