// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include "OGLRenderDeviceEm.hpp"
#include "../../../utils/Utils.hpp"

namespace ouzel::graphics::opengl::emscripten
{
    RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(initCallback)
    {
        embedded = true;
    }

    RenderDevice::~RenderDevice()
    {
        if (webGLContext)
            emscripten_webgl_destroy_context(webGLContext);
    }

    void RenderDevice::init(core::Window& newWindow,
                            const Size2U& newSize,
                            const Settings& settings)
    {
        apiVersion = ApiVersion(2, 0);

        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);

        attrs.alpha = true;
        attrs.depth = settings.depth;
        attrs.stencil = settings.stencil;
        attrs.antialias = settings.sampleCount > 0;

        webGLContext = emscripten_webgl_create_context(0, &attrs);

        if (!webGLContext)
            throw std::runtime_error("Failed to create WebGL context");

        EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(webGLContext);

        if (result != EMSCRIPTEN_RESULT_SUCCESS)
            throw std::runtime_error("Failed to make WebGL context current");

        emscripten_set_main_loop_timing(settings.verticalSync ? EM_TIMING_RAF : EM_TIMING_SETTIMEOUT, 1);

        opengl::RenderDevice::init(newWindow, newSize, settings);
    }
}

#endif
