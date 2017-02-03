// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/Window.h"

namespace ouzel
{
    class WindowAndroid: public Window
    {
        friend Engine;
    public:
        virtual ~WindowAndroid();

        void handleResize(const Size2& newSize);

        ANativeWindow* getNativeWindow() const { return window; }

    protected:
        WindowAndroid(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);

        ANativeWindow* window = nullptr;
    };
}
