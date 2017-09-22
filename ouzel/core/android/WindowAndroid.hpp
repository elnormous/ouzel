// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "core/Window.hpp"

namespace ouzel
{
    class WindowAndroid: public Window
    {
        friend Engine;
    public:
        virtual ~WindowAndroid();

        void handleResize(const Size2& newSize);
        void handleSurfaceChange(jobject surface);
        void handleSurfaceDestroy();

        ANativeWindow* getNativeWindow() const { return window; }

    protected:
        WindowAndroid();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth);

        ANativeWindow* window = nullptr;
    };
}
