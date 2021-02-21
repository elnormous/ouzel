// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWANDROID_HPP
#define OUZEL_CORE_NATIVEWINDOWANDROID_HPP

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../NativeWindow.hpp"

namespace ouzel::core::android
{
    class NativeWindow final: public core::NativeWindow
    {
    public:
        NativeWindow(const std::function<void(const Event&)>& initCallback,
                     const std::string& newTitle);
        ~NativeWindow() override;

        void handleResize(const Size2U& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        auto getNativeWindow() const noexcept { return window; }

    private:
        void executeCommand(const Command& command) final;

        ANativeWindow* window = nullptr;
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWANDROID_HPP
