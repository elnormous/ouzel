// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/WindowResource.hpp"

namespace ouzel
{
    class Window;

    class WindowResourceAndroid: public WindowResource
    {
        friend Window;
    public:
        virtual ~WindowResourceAndroid();

        void handleResize(const Size2& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        ANativeWindow* getNativeWindow() const { return window; }

    protected:
        WindowResourceAndroid();
        virtual void init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth);

        ANativeWindow* window = nullptr;
    };
}
