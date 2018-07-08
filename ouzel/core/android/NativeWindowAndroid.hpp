// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/NativeWindow.hpp"

namespace ouzel
{
    class Window;

    class NativeWindowAndroid: public NativeWindow
    {
        friend Window;
    public:
        virtual ~NativeWindowAndroid();

        void handleResize(const Size2& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        ANativeWindow* getNativeWindow() const { return window; }

    protected:
        NativeWindowAndroid(const std::string& newTitle);

        ANativeWindow* window = nullptr;
    };
}
