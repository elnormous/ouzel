// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <array>
#include <utility>
#include "OGLRenderDeviceEm.hpp"
#include "../../../core/Window.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::opengl::emscripten
{
    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(settings, initWindow, initCallback)
    {
        embedded = true;

        constexpr std::array<std::pair<int, ApiVersion>, 2> openGLVersions = {
            std::pair(2, ApiVersion(3, 0)),
            std::pair(1, ApiVersion(2, 0))
        };

        for (const auto openGLVersion : openGLVersions)
        {
            EmscriptenWebGLContextAttributes attrs;
            emscripten_webgl_init_context_attributes(&attrs);

            attrs.alpha = true;
            attrs.depth = settings.depth;
            attrs.stencil = settings.stencil;
            attrs.antialias = settings.sampleCount > 0;
            attrs.majorVersion = openGLVersion.first;
            attrs.minorVersion = 0;

            webGLContext = emscripten_webgl_create_context("#canvas", &attrs);

            if (webGLContext)
            {
                apiVersion = openGLVersion.second;
                logger.log(Log::Level::info) << "WebGL " << openGLVersion.first << " context created";
                break;
            }
        }

        if (!webGLContext)
            throw std::runtime_error("Failed to create WebGL context");

        if (const auto result = emscripten_webgl_make_context_current(webGLContext); result != EMSCRIPTEN_RESULT_SUCCESS)
            throw std::runtime_error("Failed to make WebGL context current");

        emscripten_set_main_loop_timing(settings.verticalSync ? EM_TIMING_RAF : EM_TIMING_SETTIMEOUT, 1);

        // emscripten_webgl_get_drawing_buffer_size: https://emscripten.org/docs/api_reference/html5.h.html?highlight=emscripten_webgl_create_context#c.emscripten_webgl_get_drawing_buffer_size

        init(static_cast<GLsizei>(window.getResolution().v[0]),
             static_cast<GLsizei>(window.getResolution().v[1]));

        // TODO: check context lost/restored: https://emscripten.org/docs/api_reference/html5.h.html#c.EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST
    }

    RenderDevice::~RenderDevice()
    {
        if (webGLContext)
            emscripten_webgl_destroy_context(webGLContext);
    }
}

#endif
