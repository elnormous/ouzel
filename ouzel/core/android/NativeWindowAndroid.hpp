// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWANDROID_HPP
#define OUZEL_CORE_NATIVEWINDOWANDROID_HPP

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/NativeWindow.hpp"

namespace ouzel
{
    class NativeWindowAndroid final: public NativeWindow
    {
    public:
        NativeWindowAndroid(const std::function<void(const Event&)>& initCallback,
                            const std::string& newTitle);
        ~NativeWindowAndroid();

        void executeCommand(const Command& command) override;

        void handleResize(const Size<2, uint32_t>& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        ANativeWindow* getNativeWindow() const { return window; }

    private:
        ANativeWindow* window = nullptr;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWANDROID_HPP
