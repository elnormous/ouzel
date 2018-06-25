// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <bcm_host.h>
#include "core/WindowResource.hpp"

typedef struct {
    DISPMANX_ELEMENT_HANDLE_T element;
    int width; /* This is necessary because dispmanx elements are not queriable. */
    int height;
 } EGL_DISPMANX_WINDOW_T;

namespace ouzel
{
    class Window;

    class WindowResourceRasp: public WindowResource
    {
        friend Window;
    public:
        virtual ~WindowResourceRasp();

        const EGL_DISPMANX_WINDOW_T& getNativeWindow() const { return nativewindow; }

    protected:
        WindowResourceRasp();
        virtual void init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        DISPMANX_DISPLAY_HANDLE_T display = DISPMANX_NO_HANDLE;
        EGL_DISPMANX_WINDOW_T nativewindow;
    };
}
