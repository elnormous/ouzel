// Ouzel by Elviss Strazdins

#include "../../../core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include <array>
#include <utility>
#include "OGLRenderDeviceEm.hpp"
#include "../../../core/Window.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::opengl::emscripten
{
    namespace
    {
        EM_BOOL contextCallback(int eventType, const void* reserved, void* userData)
        {
            const auto renderDevice = static_cast<RenderDevice*>(userData);

            if (eventType == EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST)
            {
                renderDevice->handleContextLost();
                return EM_TRUE;
            }
            else if (eventType == EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED)
            {
                renderDevice->handleContextRestored();
                return EM_TRUE;
            }

            return EM_FALSE;
        }
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(settings, initWindow, initCallback)
    {
        emscripten_set_webglcontextlost_callback("#canvas", this, EM_TRUE, contextCallback);
        emscripten_set_webglcontextrestored_callback("#canvas", this, EM_TRUE, contextCallback);

        embedded = true;

        constexpr std::array<std::pair<int, ApiVersion>, 2> openGlVersions = {
            std::pair(2, ApiVersion{3, 0}),
            std::pair(1, ApiVersion{2, 0})
        };

        for (const auto& openGlVersion : openGlVersions)
        {
            EmscriptenWebGLContextAttributes attrs;
            emscripten_webgl_init_context_attributes(&attrs);

            attrs.alpha = true;
            attrs.depth = settings.depth;
            attrs.stencil = settings.stencil;
            attrs.antialias = settings.sampleCount > 0;
            attrs.majorVersion = openGlVersion.first;
            attrs.minorVersion = 0;

            webGLContext = emscripten_webgl_create_context("#canvas", &attrs);

            if (webGLContext)
            {
                apiVersion = openGlVersion.second;
                logger.log(Log::Level::info) << "WebGL " << openGlVersion.first << " context created";
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
    }

    RenderDevice::~RenderDevice()
    {
        if (webGLContext)
            emscripten_webgl_destroy_context(webGLContext);
    }

    void RenderDevice::handleContextLost()
    {
        for (const auto& resource : resources)
            if (resource) resource->invalidate();
    }

    void RenderDevice::handleContextRestored()
    {
        constexpr std::array<std::pair<int, ApiVersion>, 2> openGlVersions = {
            std::pair(2, ApiVersion{3, 0}),
            std::pair(1, ApiVersion{2, 0})
        };

        for (const auto& openGlVersion : openGlVersions)
        {
            EmscriptenWebGLContextAttributes attrs;
            emscripten_webgl_init_context_attributes(&attrs);

            attrs.alpha = true;
            attrs.depth = depth;
            attrs.stencil = stencil;
            attrs.antialias = sampleCount > 0;
            attrs.majorVersion = openGlVersion.first;
            attrs.minorVersion = 0;

            webGLContext = emscripten_webgl_create_context("#canvas", &attrs);

            if (webGLContext)
            {
                apiVersion = openGlVersion.second;
                logger.log(Log::Level::info) << "WebGL " << openGlVersion.first << " context created";
                break;
            }
        }

        if (!webGLContext)
            throw std::runtime_error("Failed to create WebGL context");

        if (const auto result = emscripten_webgl_make_context_current(webGLContext); result != EMSCRIPTEN_RESULT_SUCCESS)
            throw std::runtime_error("Failed to make WebGL context current");

        for (const auto& resource : resources)
            if (resource) resource->restore();
    }
}

#endif
