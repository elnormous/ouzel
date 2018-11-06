// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef NATIVEWINDOWANDROID_HPP
#define NATIVEWINDOWANDROID_HPP

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/NativeWindow.hpp"

namespace ouzel
{
    class NativeWindowAndroid final: public NativeWindow
    {
    public:
        NativeWindowAndroid(EventHandler& initEventHandler,
                            const std::string& newTitle);
        ~NativeWindowAndroid();

        void handleResize(const Size2& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        ANativeWindow* getNativeWindow() const { return window; }

    private:
        ANativeWindow* window = nullptr;
    };
}

#endif // NATIVEWINDOWANDROID_HPP
